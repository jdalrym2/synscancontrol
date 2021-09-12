#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

#include "Command.hpp"
#include "Motor.hpp"
#include "Reply.hpp"

#define COMMAND_BUFFER_SIZE 256

class CommandHandler
{
public:
    CommandHandler(HardwareSerial *commSerial, Motor *raMotor, Motor *decMotor);
    void processSerial();
    void clearBuffer();
    Motor *getMotorForAxis(AxisEnum axis);

private:
    HardwareSerial *_commSerial;
    Motor *_raMotor;
    Motor *_decMotor;
    char _buffer[COMMAND_BUFFER_SIZE + 1];
    uint16_t _buffer_idx = 0;
    const char _startChar = ':';
    const char _endChar = '\r';

private:
    Reply *_processCommand(Command *command);
};

#endif /* COMMAND_HANDLER_H */