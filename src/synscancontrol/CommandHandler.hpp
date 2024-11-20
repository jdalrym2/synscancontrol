/*
 * Project Name: synscancontrol
 * File: CommandHandler.hpp
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
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

#include "Command.hpp"
#include "Constants.hpp"
#include "Motor.hpp"
#include "PolarScopeLED.hpp"
#include "Reply.hpp"
#include "Logger.hpp"

namespace SynScanControl
{
    class CommandHandler
    {
    public:
        CommandHandler(HardwareSerial *commSerial, Motor *raMotor, Motor *decMotor, PolarScopeLED *polarScopeLED, Logger *logger);
        void processSerial();
        void clearBuffer();
        Motor *getMotorForAxis(AxisEnum axis);

    private:
        HardwareSerial *_commSerial;
        Motor *_raMotor;
        Motor *_decMotor;
        PolarScopeLED *_polarScopeLED;
        char _buffer[COMMAND_BUFFER_SIZE + 1];
        uint16_t _buffer_idx = 0;
        const char _startChar = ':';
        const char _endChar = '\r';
        bool _serialStarted = false;
        uint32_t _timeoutCounter = 0;

        Logger *_logger;

    private:
        Reply *_processCommand(Command *command);
    };

} // namespace SynScanControl

#endif /* COMMAND_HANDLER_H */