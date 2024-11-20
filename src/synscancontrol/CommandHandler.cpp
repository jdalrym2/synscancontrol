/*
 * Project Name: synscancontrol
 * File: CommandHandler.cpp
 *
 * Copyright (C) 2024 Jon Dalrymple
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jon Dalrymple
 * Created: 13 November 2024
 * Description: Provides business logic for responding to SynScan commands
 */
#include "CommandHandler.hpp"

using namespace SynScanControl;

CommandHandler::CommandHandler(HardwareSerial *commSerial,
                               Motor *raMotor, Motor *decMotor, PolarScopeLED *polarScopeLED, Logger *logger)
{
    _commSerial = commSerial;
    _raMotor = raMotor;
    _decMotor = decMotor;
    _polarScopeLED = polarScopeLED;
    _logger = logger;
}

void CommandHandler::processSerial()
{
    // Process available serial
    while (_commSerial->available() > 0)
    {
        // We are processing serial!
        _serialStarted = true;
        _timeoutCounter = millis();

        // Read in a character
        char inChar = _commSerial->read();

        // If it's the start character, clear the buffer
        if (inChar == _startChar)
            clearBuffer();

        // If it's the end character, process a message
        // We need *at least* ":[commandChar][axisNum]" to do something
        // so check this length
        if (inChar == _endChar && _buffer_idx > 2)
        {
            // Log the command we got
            /*std::ostringstream log;
            log << "Received command: " << _buffer;
            _logger->debug(&log);*/

            // Process the message and get a reply
            Command *cmd = CommandFactory::parse(_buffer, _buffer_idx);
            if (cmd)
            {
                if (cmd->parse(_buffer, _buffer_idx))
                {
                    // Command successfully parsed
                    // Process it and generate a reply
                    Reply *reply = _processCommand(cmd);

                    // Send the reply
                    if (reply)
                    {
                        // Log the reply we are giving
                        /*std::ostringstream log;
                        log << "Sending reply: ";
                        reply->toStringStream(&log);
                        _logger->debug(&log);*/

                        reply->send(_commSerial);
                    }
                    else
                    {
                        _logger->error("Failed to come up with a reply!");
                    }

                    // We are done processing the reply
                    delete reply;
                }
                else
                {
                    _logger->error("Error parsing command!");
                    _logger->error("===");
                    _logger->error(_buffer);
                    _logger->error("===");
                }
            }
            else
            {
                _logger->error("Command factory returned nullptr!");
                _logger->error("===");
                _logger->error(_buffer);
                _logger->error("===");
            }

            // We are done processing the command
            delete cmd;

            // Finish by clearing buffer
            clearBuffer();
        }
        else
        {
            // Otherwise the message is incomplete,
            // add the character to the buffer
            if (_buffer_idx < COMMAND_BUFFER_SIZE)
            {
                _buffer[_buffer_idx++] = inChar;
                _buffer[_buffer_idx] = '\0';
            }
        }
    }

    // Serial timeout handling
    if (_serialStarted && (millis() - _timeoutCounter > SERIAL_TIMEOUT_MS))
    {
        _logger->info("Serial timeout reached!");
        _serialStarted = false;
        _raMotor->setMotion(false);
        _decMotor->setMotion(false);
    }
}

void CommandHandler::clearBuffer()
{
    // Reset the buffer cursor to the first index
    _buffer_idx = 0;
    _buffer[0] = '\0';
}

Motor *CommandHandler::getMotorForAxis(AxisEnum axis)
{
    if (axis == AxisEnum::AXIS_DEC)
    {
        return _decMotor;
    }
    else
    {
        return _raMotor;
    }
}

Reply *CommandHandler::_processCommand(Command *cmd)
{
    Reply *reply = nullptr;
    Motor *thisMotor = getMotorForAxis(cmd->getAxis());

    // Figure out which command we got, do the processing, and generate a reply
    switch (cmd->getCommand())
    {
    case CommandEnum::SET_POSITION_CMD:
    {
        SetPositionCommand *thisCmd = (SetPositionCommand *)cmd;

        // Set motor position
        if (!thisMotor->isMoving())
        {
            thisMotor->setPosition(thisCmd->getPosition());
            reply = new EmptyReply();
        }
        else
        {
            reply = new ErrorReply(ErrorEnum::MOTOR_NOT_STOPPED_ERROR);
        }
        break;
    }
    case CommandEnum::INITIALIZATION_DONE_CMD:
    {
        // InitializationDoneCommand *thisCmd = (InitializationDoneCommand *)cmd;
        // No processing to be done atm
        // This can change if we want to block commands until initalization is complete
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_MOTION_MODE_CMD:
    {
        SetMotionModeCommand *thisCmd = (SetMotionModeCommand *)cmd;

        // Set motor motion mode
        if (!thisMotor->isMoving())
        {
            thisMotor->setSlewType(thisCmd->getType());
            thisMotor->setSlewSpeed(thisCmd->getSpeed());
            thisMotor->setSlewDir(thisCmd->getDir());
            reply = new EmptyReply();
        }
        else
        {
            reply = new ErrorReply(ErrorEnum::MOTOR_NOT_STOPPED_ERROR);
        }
        break;
    }
    case CommandEnum::SET_GOTO_TARGET_CMD:
    {
        SetGotoTargetCommand *thisCmd = (SetGotoTargetCommand *)cmd;

        // Set GOTO target position
        if (!thisMotor->isMoving())
        {
            thisMotor->setTargetPosition(thisCmd->getPosition());
            reply = new EmptyReply();
        }
        else
        {
            reply = new ErrorReply(ErrorEnum::MOTOR_NOT_STOPPED_ERROR);
        }
        break;
    }
    case CommandEnum::SET_GOTO_TARGET_INCREMENT_CMD:
    {
        SetGotoTargetIncrementCommand *thisCmd = (SetGotoTargetIncrementCommand *)cmd;
        if (!thisMotor->isMoving())
        {
            uint32_t curPosition = thisMotor->getPosition();
            uint32_t increment = thisCmd->getIncrement();
            if (thisMotor->getSlewDirection() == SlewDirectionEnum::CW)
            {
                thisMotor->setTargetPosition(curPosition + increment);
            }
            else
            {
                thisMotor->setTargetPosition(curPosition - increment);
            }
            reply = new EmptyReply();
        }
        else
        {
            reply = new ErrorReply(ErrorEnum::MOTOR_NOT_STOPPED_ERROR);
        }
        break;
    }
    case CommandEnum::SET_BREAKPOINT_INCREMENT_CMD:
    {
        // SetBreakPointIncrementCommand *thisCmd = (SetBreakPointIncrementCommand *)cmd;
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_STEP_PERIOD_CMD:
    {
        SetStepPeriodCommand *thisCmd = (SetStepPeriodCommand *)cmd;
        thisMotor->setStepPeriod(thisCmd->getPeriod());
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::START_MOTION_CMD:
    {
        // StartMotionCommand *thisCmd = (StartMotionCommand *)cmd;
        if (!thisMotor->isMoving())
        {
            thisMotor->setMotion(true);
            reply = new EmptyReply();
        }
        else if (thisMotor->getSlewSpeed() == SlewSpeedEnum::NONE)
        {
            reply = new ErrorReply(ErrorEnum::NOT_INITIALIZED_ERROR);
        }
        else
        {
            reply = new ErrorReply(ErrorEnum::MOTOR_NOT_STOPPED_ERROR);
        }
        break;
    }
    case CommandEnum::STOP_MOTION_CMD:
    {
        // StopMotionCommand *thisCmd = (StopMotionCommand *)cmd;
        if (thisMotor->isMoving())
        {
            thisMotor->setMotion(false);
        }
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::INSTANT_STOP_CMD:
    {
        // InstantStopCommand *thisCmd = (InstantStopCommand *)cmd;
        // TODO
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_SWITCH_CMD:
    {
        // SetSwitchCommand *thisCmd = (SetSwitchCommand *)cmd;
        // Not supported, do not process
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_AUTOGUIDE_SPEED_CMD:
    {
        // SetAutoguideSpeedCommand *thisCmd = (SetAutoguideSpeedCommand *)cmd;
        // TODO: if we want
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_POLAR_LED_BRIGHTNESS_CMD:
    {
        SetPolarLEDBrightnessCommand *thisCmd = (SetPolarLEDBrightnessCommand *)cmd;
        _polarScopeLED->setBrightness(thisCmd->getValue());
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::GET_COUNTS_PER_REV_CMD:
    {
        // GetCountsPerRevCommand *thisCmd = (GetCountsPerRevCommand *)cmd;
        DataReply *data_reply = new DataReply();
        data_reply->setData(MICROSTEPS_PER_REV, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_TIMER_FREQ_CMD:
    {
        // GetTimerFreqCommand *thisCmd = (GetTimerFreqCommand *)cmd;
        DataReply *data_reply = new DataReply();
        data_reply->setData(MAX_PULSE_PER_SECOND, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_GOTO_TARGET_CMD:
    {
        // GetGotoTargetPositionCommand *thisCmd = (GetGotoTargetPositionCommand *)cmd;
        PositionReply *position_reply = new PositionReply();
        position_reply->setData(thisMotor->getTargetPosition(), 6);
        reply = position_reply;
        break;
    }
    case CommandEnum::GET_STEP_PERIOD_CMD:
    {
        // GetStepPeriodCommand *thisCmd = (GetStepPeriodCommand *)cmd;
        DataReply *data_reply = new DataReply();
        data_reply->setData(thisMotor->getSpeed(), 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_POSITION_CMD:
    {
        // GetPositionCommand *thisCmd = (GetPositionCommand *)cmd;
        PositionReply *position_reply = new PositionReply();
        position_reply->setData(thisMotor->getPosition(), 6);
        reply = position_reply;
        break;
    }
    case CommandEnum::GET_STATUS_CMD:
    {
        // GetStatusCommand *thisCmd = (GetStatusCommand *)cmd;

        StatusReply *status_reply = new StatusReply();

        // TODO: understand these better
        status_reply->setInitDone(true);
        status_reply->setBlocked(false);

        status_reply->setRunning(thisMotor->isMoving());
        status_reply->setSlewMode(thisMotor->getSlewType());
        status_reply->setSpeedMode(thisMotor->getSlewSpeed());
        status_reply->setDirection(thisMotor->getSlewDirection());
        reply = status_reply;
        break;
    }
    case CommandEnum::GET_HIGH_SPEED_RATIO_CMD:
    {
        // GetHighSpeedRatioCommand *thisCmd = (GetHighSpeedRatioCommand *)cmd;
        DataReply *data_reply = new DataReply();
        data_reply->setData(HIGH_SPEED_RATIO, 2);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_SIDEREAL_PERIOD_CMD:
    {
        // GetSiderealPeriodCommand *thisCmd = (GetSiderealPeriodCommand *)cmd;
        DataReply *data_reply = new DataReply();
        data_reply->setData(SIDEREAL_PULSE_PER_STEP, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_AXIS_POSITION_CMD:
    {
        // GetAxisPositionCommand *thisCmd = (GetAxisPositionCommand *)cmd;
        PositionReply *position_reply = new PositionReply();
        position_reply->setData(thisMotor->getPosition(), 6);
        reply = position_reply;
        break;
    }
    case CommandEnum::GET_VERSION_CMD:
    {
        // GetVersionCommand *thisCmd = (GetVersionCommand *)cmd;
        VersionReply *version_reply = new VersionReply();
        // TODO: make these constants elsewhere?
        version_reply->setVersion(2, 0, 5, 1);
        reply = version_reply;
        break;
    }
    case CommandEnum::GET_PEC_PERIOD_CMD:
    {
        // GetPECPeriodCommand *thisCmd = (GetPECPeriodCommand *)cmd;
        // TODO: replace with motor PEC period, if this is something we will do
        DataReply *data_reply = new DataReply();
        data_reply->setData(0, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_EXTENDED_STATUS_CMD:
    {
        // GetExtendedStatusCommand *thisCmd = (GetExtendedStatusCommand *)cmd;

        // TODO: check if we are getting the correct extended status request
        ExtendedStatusReply *ex_status_reply = new ExtendedStatusReply();
        ex_status_reply->setDualEncSupport(false);
        ex_status_reply->setEQAZModeSupport(false);
        ex_status_reply->setHasPolarLed(true);
        ex_status_reply->setOriginalIdxPosSupport(false);
        ex_status_reply->setPPECSupport(false);
        ex_status_reply->setPecTracking(false);
        ex_status_reply->setPecTraining(false);
        ex_status_reply->setTorqueSelectionSupport(false);
        ex_status_reply->setTwoAxesSeparate(false);
        reply = ex_status_reply;
        break;
    }
    default:
        break;
    }
    return reply;
}