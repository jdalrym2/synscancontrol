#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Enums.hpp"
#include "Motor.hpp"
#include "Reply.hpp"

#define COMMAND_BUFFER_SIZE 256

class CommandHandler
{
public:
    CommandHandler(SoftwareSerial *serial, HardwareSerial *serialOut, Motor *raMotor, Motor *decMotor);
    ~CommandHandler(){};
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
    Reply *_processMessage();
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
    while (_serialIn->available() > 0)
    {
        char inChar = _serialIn->read();
        if (inChar == _startChar)
            clearBuffer();
        if (inChar == _endChar)
        {
            // Process the message and get a reply
            Reply *reply = _processMessage();

            // Send the reply
            if (reply)
                reply->send(_serialOut);
            delete reply;

            // Finish by clearing buffer
            clearBuffer();
        }
        else if (_buffer_idx < COMMAND_BUFFER_SIZE)
        {
            _buffer[_buffer_idx++] = inChar;
            _buffer[_buffer_idx] = '\0';
        }
    }
}

void CommandHandler::clearBuffer()
{
    _buffer_idx = 0;
    _buffer[0] = '\0';
}

Reply *CommandHandler::_processMessage()
{
    // We need *at least* ":[commandChar]\r" to do something
    if (_buffer_idx < 2)
        return nullptr;

    Reply *reply = nullptr;
    switch (_buffer[1])
    {
    case (char)CommandEnum::SET_POSITION_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::INITIALIZATION_DONE_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_MOTION_MODE_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_GOTO_TARGET_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_GOTO_TARGET_INCREMENT_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_STEP_PERIOD_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::START_MOTION_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::STOP_MOTION_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::INSTANT_STOP_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_SWITCH_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_AUTOGUIDE_SPEED_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::SET_POLAR_LED_BRIGHTNESS_CMD:
    {
        reply = new EmptyReply();
        break;
    }
    case (char)CommandEnum::GET_COUNTS_PER_REV_CMD:
    {
        DataReply *data_reply = new DataReply();
        // TODO: replace with actual counts per revolution
        data_reply->setData(0, 6);
        reply = data_reply;
        break;
    }
    case (char)CommandEnum::GET_TIMER_FREQ_CMD:
    {
        // TODO: understand this better
        DataReply *data_reply = new DataReply();
        data_reply->setData(50000, 6);
        reply = data_reply;
        break;
    }
    case (char)CommandEnum::GET_GOTO_TARGET_CMD:
    {
        PositionReply *position_reply = new PositionReply();
        // TODO: replace with motor goto target for this axis
        position_reply->setData(0x800000, 6);
        reply = position_reply;
        break;
    }
    case (char)CommandEnum::GET_STEP_PERIOD_CMD:
    {
        // TODO: replace with motor step period for this axis
        DataReply *data_reply = new DataReply();
        data_reply->setData(0, 6);
        reply = data_reply;
        break;
    }
    case (char)CommandEnum::GET_POSITION_CMD:
    {
        PositionReply *position_reply = new PositionReply();
        // TODO: set with actual position for this motor
        position_reply->setData(0x800000, 6);
        reply = position_reply;
        break;
    }
    case (char)CommandEnum::GET_STATUS_CMD:
    {
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
    case (char)CommandEnum::GET_HIGH_SPEED_RATIO_CMD:
    {
        DataReply *data_reply = new DataReply();
        // TODO: make this a define / configurable somewhere
        data_reply->setData(2, 2);
        reply = data_reply;
        break;
    }
    case (char)CommandEnum::GET_SIDEREAL_PERIOD_CMD:
    {
        DataReply *data_reply = new DataReply();
        // TODO: replace with motor sidereal period for this axis
        data_reply->setData(0, 6);
        reply = data_reply;
        break;
    }
    case (char)CommandEnum::GET_AXIS_POSITION_CMD:
    {
        PositionReply *position_reply = new PositionReply();
        // TODO: replace with actual motor position for this axis
        position_reply->setData(0x800000, 6);
        reply = position_reply;
        break;
    }
    case (char)CommandEnum::GET_VERSION_CMD:
    {
        VersionReply *version_reply = new VersionReply();
        // TODO: make these constants elsewhere?
        version_reply->setVersion(2, 0, 5, 1);
        reply = version_reply;
        break;
    }
    case (char)CommandEnum::GET_PEC_PERIOD_CMD:
    {
        // TODO: replace with motor PEC period
        DataReply *data_reply = new DataReply();
        data_reply->setData(0, 6);
        reply = data_reply;
        break;
    }
    case (char)CommandEnum::GET_EXTENDED_STATUS_CMD:
    {
        // TODO: check if we are getting the correct extended status request
        ExtendedStatusReply *ex_status_reply = new ExtendedStatusReply();
        ex_status_reply->setDualEncSupport(false);
        ex_status_reply->setEQAZModeSupport(false);
        ex_status_reply->setHasPolarLed(false);
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

#endif /* COMMAND_HANDLER_H_ */