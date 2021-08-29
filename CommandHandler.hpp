#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Motor.hpp"
#include "Reply.hpp"
#include "Enums.hpp"
#include "Command.hpp"

#define COMMAND_BUFFER_SIZE 256

class CommandHandler
{
public:
    CommandHandler(SoftwareSerial *serial, HardwareSerial *serialOut, Motor *raMotor, Motor *decMotor);
    void processSerial();
    void clearBuffer();

private:
    SoftwareSerial *_serialIn;
    HardwareSerial *_serialOut;
    Motor *_raMotor;
    Motor *_decMotor;
    char _buffer[COMMAND_BUFFER_SIZE + 1];
    uint16_t _buffer_idx = 0;
    const char _startChar = ':';
    const char _endChar = '\r';

private:
    Reply *_processCommand(Command *command);
};

CommandHandler::CommandHandler(SoftwareSerial *serialIn, HardwareSerial *serialOut,
                               Motor *raMotor, Motor *decMotor)
{
    _serialIn = serialIn;
    _serialOut = serialOut;
    _raMotor = raMotor;
    _decMotor = decMotor;
}

void CommandHandler::processSerial()
{
    // TODO: does this take too long?
    while (_serialIn->available() > 0)
    {
        // Read in a character
        char inChar = _serialIn->read();

        // If it's the start character, clear the buffer
        if (inChar == _startChar)
            clearBuffer();

        // If it's the end character, process a message
        // We need *at least* ":[commandChar][axisNum]" to do something
        // so check this length
        if (inChar == _endChar && _buffer_idx > 2)
        {
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
                    // TODO: this probably takes too long
                    // create a Serial output buffer and send when available
                    if (reply)
                    {
                        reply->send(_serialOut);
                    }
                    else
                    {
                        // TODO: debug only
                        Serial.println("Failed to come up with a reply!");
                    }

                    // We are done processing the reply
                    delete reply;
                }
                else
                {
                    // TODO: debug only
                    Serial.println("Error parsing command!");
                }
            }
            else
            {
                // TODO: debug only (when this is a failure condition)
                Serial.println("Command factory returned nullptr! We probably just don't support the command yet.");
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
}

void CommandHandler::clearBuffer()
{
    // Reset the buffer cursor to the first index
    _buffer_idx = 0;
    _buffer[0] = '\0';
}

Reply *CommandHandler::_processCommand(Command *cmd)
{
    Reply *reply = nullptr;

    // Figure out which command we got, do the processing, and generate a reply
    switch (cmd->getCommand())
    {
    case CommandEnum::SET_POSITION_CMD:
    {
        SetPositionCommand *thisCmd = (SetPositionCommand *)cmd;

        // Set motor position
        if (thisCmd->getAxis() == AxisEnum::AXIS_RA)
        {
            _raMotor->setPosition(thisCmd->getPosition());
        }
        else if (thisCmd->getAxis() == AxisEnum::AXIS_DEC)
        {
            _decMotor->setPosition(thisCmd->getPosition());
        }

        reply = new EmptyReply();
        break;
    }
    case CommandEnum::INITIALIZATION_DONE_CMD:
    {
        InitializationDoneCommand *thisCmd = (InitializationDoneCommand *)cmd;
        // No processing to be done!
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_MOTION_MODE_CMD:
    {
        SetMotionModeCommand *thisCmd = (SetMotionModeCommand *)cmd;

        // Set motor motion mode
        if (thisCmd->getAxis() == AxisEnum::AXIS_RA)
        {
            _raMotor->setSlewType(thisCmd->getType());
            _raMotor->setSlewSpeed(thisCmd->getSpeed());
            _raMotor->setSlewDir(thisCmd->getDir());
        }
        else if (thisCmd->getAxis() == AxisEnum::AXIS_DEC)
        {
            _decMotor->setSlewType(thisCmd->getType());
            _decMotor->setSlewSpeed(thisCmd->getSpeed());
            _decMotor->setSlewDir(thisCmd->getDir());
        }

        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_GOTO_TARGET_CMD:
    {
        SetGotoTargetCommand *thisCmd = (SetGotoTargetCommand *)cmd;
        if (thisCmd->getAxis() == AxisEnum::AXIS_RA)
        {
            _raMotor->setOrigPosition(_raMotor->getPosition());
            _raMotor->setTargetPosition(thisCmd->getPosition());
        }
        else if (thisCmd->getAxis() == AxisEnum::AXIS_DEC)
        {
            _decMotor->setOrigPosition(_decMotor->getPosition());
            _decMotor->setTargetPosition(thisCmd->getPosition());
        }

        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_GOTO_TARGET_INCREMENT_CMD:
    {
        SetGotoTargetIncrementCommand *thisCmd = (SetGotoTargetIncrementCommand *)cmd;
        if (thisCmd->getAxis() == AxisEnum::AXIS_RA)
        {
            uint32_t curPosition = _raMotor->getPosition();
            uint32_t increment = thisCmd->getIncrement();
            _raMotor->setOrigPosition(curPosition);
            if (_raMotor->getSlewDirection() == SlewDirectionEnum::CW)
            {
                _raMotor->setTargetPosition(curPosition + increment);
            }
            else
            {
                _raMotor->setTargetPosition(curPosition - increment);
            }
        }
        else if (thisCmd->getAxis() == AxisEnum::AXIS_DEC)
        {
            uint32_t curPosition = _decMotor->getPosition();
            uint32_t increment = thisCmd->getIncrement();
            _decMotor->setOrigPosition(curPosition);
            if (_decMotor->getSlewDirection() == SlewDirectionEnum::CW)
            {
                _decMotor->setTargetPosition(curPosition + increment);
            }
            else
            {
                _decMotor->setTargetPosition(curPosition - increment);
            }
        }

        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_STEP_PERIOD_CMD:
    {
        SetStepPeriodCommand *thisCmd = (SetStepPeriodCommand *)cmd;
        if (thisCmd->getAxis() == AxisEnum::AXIS_RA)
        {
            _raMotor->setStepPeriod(thisCmd->getPeriod());
        }
        else if (thisCmd->getAxis() == AxisEnum::AXIS_DEC)
        {
            _decMotor->setStepPeriod(thisCmd->getPeriod());
        }

        reply = new EmptyReply();
        break;
    }
    case CommandEnum::START_MOTION_CMD:
    {
        StartMotionCommand *thisCmd = (StartMotionCommand *)cmd;
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::STOP_MOTION_CMD:
    {
        StopMotionCommand *thisCmd = (StopMotionCommand *)cmd;
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::INSTANT_STOP_CMD:
    {
        InstantStopCommand *thisCmd = (InstantStopCommand *)cmd;
        thisCmd->parse(_buffer, _buffer_idx);
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_SWITCH_CMD:
    {
        SetSwitchCommand *thisCmd = (SetSwitchCommand *)cmd;
        // Not supported, do not process
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_AUTOGUIDE_SPEED_CMD:
    {
        SetAudioguideSpeedCommand *thisCmd = (SetAudioguideSpeedCommand *)cmd;
        // TODO: if we want
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::SET_POLAR_LED_BRIGHTNESS_CMD:
    {
        SetPolarLEDBrightnessCommand *thisCmd = (SetPolarLEDBrightnessCommand *)cmd;
        // TODO: if we want
        reply = new EmptyReply();
        break;
    }
    case CommandEnum::GET_COUNTS_PER_REV_CMD:
    {
        GetCountsPerRevCommand *thisCmd = (GetCountsPerRevCommand *)cmd;
        DataReply *data_reply = new DataReply();
        // TODO: replace with actual counts per revolution
        data_reply->setData(1000, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_TIMER_FREQ_CMD:
    {
        GetTimerFreqCommand *thisCmd = (GetTimerFreqCommand *)cmd;
        // TODO: understand this better
        DataReply *data_reply = new DataReply();
        data_reply->setData(50000, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_GOTO_TARGET_CMD:
    {
        GetGotoTargetPositionCommand *thisCmd = (GetGotoTargetPositionCommand *)cmd;
        PositionReply *position_reply = new PositionReply();
        // TODO: replace with motor goto target for this axis
        position_reply->setData(0x800000, 6);
        reply = position_reply;
        break;
    }
    case CommandEnum::GET_STEP_PERIOD_CMD:
    {
        GetStepPeriodCommand *thisCmd = (GetStepPeriodCommand *)cmd;
        // TODO: replace with motor step period for this axis
        DataReply *data_reply = new DataReply();
        data_reply->setData(1, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_POSITION_CMD:
    {
        GetPositionCommand *thisCmd = (GetPositionCommand *)cmd;
        PositionReply *position_reply = new PositionReply();
        // TODO: set with actual position for this motor
        position_reply->setData(0x800000, 6);
        reply = position_reply;
        break;
    }
    case CommandEnum::GET_STATUS_CMD:
    {
        GetStatusCommand *thisCmd = (GetStatusCommand *)cmd;

        StatusReply *status_reply = new StatusReply();

        // TODO: understand these better
        status_reply->setInitDone(true);
        status_reply->setBlocked(false);

        // TODO: replace with motor state variables for this axis
        status_reply->setRunning(false);
        status_reply->setSlewMode(SlewTypeEnum::TRACKING);
        status_reply->setSpeedMode(SlewSpeedEnum::SLOW);
        status_reply->setDirection(SlewDirectionEnum::CW);
        reply = status_reply;
        break;
    }
    case CommandEnum::GET_HIGH_SPEED_RATIO_CMD:
    {
        GetHighSpeedRatioCommand *thisCmd = (GetHighSpeedRatioCommand *)cmd;
        DataReply *data_reply = new DataReply();
        // TODO: make this a define / configurable somewhere
        data_reply->setData(2, 2);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_SIDEREAL_PERIOD_CMD:
    {
        GetSiderealPeriodCommand *thisCmd = (GetSiderealPeriodCommand *)cmd;
        DataReply *data_reply = new DataReply();
        // TODO: replace with motor sidereal period for this axis
        data_reply->setData(1000, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_AXIS_POSITION_CMD:
    {
        GetAxisPositionCommand *thisCmd = (GetAxisPositionCommand *)cmd;
        PositionReply *position_reply = new PositionReply();
        // TODO: replace with actual motor position for this axis
        position_reply->setData(0x800000, 6);
        reply = position_reply;
        break;
    }
    case CommandEnum::GET_VERSION_CMD:
    {
        GetVersionCommand *thisCmd = (GetVersionCommand *)cmd;
        VersionReply *version_reply = new VersionReply();
        // TODO: make these constants elsewhere?
        version_reply->setVersion(2, 0, 5, 1);
        reply = version_reply;
        break;
    }
    case CommandEnum::GET_PEC_PERIOD_CMD:
    {
        GetPECPeriodCommand *thisCmd = (GetPECPeriodCommand *)cmd;
        // TODO: replace with motor PEC period
        DataReply *data_reply = new DataReply();
        data_reply->setData(0, 6);
        reply = data_reply;
        break;
    }
    case CommandEnum::GET_EXTENDED_STATUS_CMD:
    {
        GetExtendedStatusCommand *thisCmd = (GetExtendedStatusCommand *)cmd;

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

#endif /* COMMAND_HANDLER_H */