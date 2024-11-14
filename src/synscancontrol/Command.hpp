/*
 * Project Name: synscancontrol
 * File: Command.hpp
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
 * Description: Provides parsing functions for SynScan serial commands
 */
#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>

#include "Enums.hpp"

namespace SynScanControl
{
    class Command
    {
    protected:
        CommandEnum _cmd = CommandEnum::UNKNOWN_CMD;
        AxisEnum _axis = AxisEnum::AXIS_NONE;
        bool _has_init = false;

    protected:
        Command() {};
        static AxisEnum parseAxis(char c);

    public:
        virtual ~Command() {}

        virtual bool parse(const char *data, uint16_t len);

        CommandEnum getCommand() const;
        AxisEnum getAxis() const;
        bool hasInitialized() const;
    };

    class GetterCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 3;

    public:
        GetterCommand() {};

        bool parse(const char *data, uint16_t len) override;
    };

    // TODO: pulseguide support

    class SetPositionCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 9;
        uint32_t _position = 0;

    public:
        SetPositionCommand();

        bool parse(const char *data, uint16_t len) override;
        uint32_t getPosition() const;
    };

    class InitializationDoneCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 3;

    public:
        InitializationDoneCommand();
        bool parse(const char *data, uint16_t len) override;
    };

    class SetMotionModeCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 5;
        SlewTypeEnum _type = SlewTypeEnum::NONE;
        SlewSpeedEnum _speed = SlewSpeedEnum::NONE;
        SlewDirectionEnum _dir = SlewDirectionEnum::NONE;

    public:
        SetMotionModeCommand();

        bool parse(const char *data, uint16_t len) override;

        SlewTypeEnum
        getType() const;
        SlewSpeedEnum getSpeed() const;
        SlewDirectionEnum getDir() const;
    };

    class SetGotoTargetCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 9;
        uint32_t _position = 0;

    public:
        SetGotoTargetCommand();

        bool parse(const char *data, uint16_t len) override;
        uint32_t getPosition() const;
    };

    class SetGotoTargetIncrementCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 9;
        uint32_t _increment = 0;

    public:
        SetGotoTargetIncrementCommand();

        bool parse(const char *data, uint16_t len) override;
        uint32_t getIncrement() const;
    };

    class SetBreakPointIncrementCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 9;
        uint32_t _increment = 0;

    public:
        SetBreakPointIncrementCommand();

        bool parse(const char *data, uint16_t len) override;
        uint32_t getIncrement() const;
    };

    class SetStepPeriodCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 9;
        uint32_t _period = 0;

    public:
        SetStepPeriodCommand();

        bool parse(const char *data, uint16_t len) override;
        uint32_t getPeriod() const;
    };

    class StartMotionCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 3;

    public:
        StartMotionCommand();

        bool parse(const char *data, uint16_t len) override;
    };

    class StopMotionCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 3;

    public:
        StopMotionCommand();

        bool parse(const char *data, uint16_t len) override;
    };

    class InstantStopCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 3;

    public:
        InstantStopCommand();

        bool parse(const char *data, uint16_t len) override;
    };

    class SetSwitchCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 4;
        bool _active = false;

    public:
        SetSwitchCommand();

        bool parse(const char *data, uint16_t len) override;
    };

    class SetAudioguideSpeedCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 4;
        uint32_t _speed = 1000; // 1000 = x1 speed

    public:
        SetAudioguideSpeedCommand();

        bool parse(const char *data, uint16_t len) override;
    };

    class SetPolarLEDBrightnessCommand : public Command
    {
    private:
        static const uint16_t MSG_SIZE = 5;
        uint8_t _value = 0;

    public:
        SetPolarLEDBrightnessCommand();
        uint8_t getValue() const;
        bool parse(const char *data, uint16_t len) override;
    };

    class GetCountsPerRevCommand : public GetterCommand
    {
    public:
        GetCountsPerRevCommand();
    };

    class GetTimerFreqCommand : public GetterCommand
    {
    public:
        GetTimerFreqCommand();
    };

    class GetGotoTargetPositionCommand : public GetterCommand
    {
    public:
        GetGotoTargetPositionCommand();
    };

    class GetStepPeriodCommand : public GetterCommand
    {
    public:
        GetStepPeriodCommand();
    };

    class GetPositionCommand : public GetterCommand
    {
    public:
        GetPositionCommand();
    };

    class GetStatusCommand : public GetterCommand
    {
    public:
        GetStatusCommand();
    };

    class GetHighSpeedRatioCommand : public GetterCommand
    {
    public:
        GetHighSpeedRatioCommand();
    };

    class GetSiderealPeriodCommand : public GetterCommand
    {
    public:
        GetSiderealPeriodCommand();
    };

    class GetAxisPositionCommand : public GetterCommand
    {
    public:
        GetAxisPositionCommand();
    };

    class GetVersionCommand : public GetterCommand
    {
    public:
        GetVersionCommand();
    };

    class GetPECPeriodCommand : public GetterCommand
    {
    public:
        GetPECPeriodCommand();
    };

    class GetExtendedStatusCommand : public Command
    {
    public:
        enum class StatusType
        {
            POSITION,
            STATUS_EX,
            NONE
        };

    private:
        static const uint16_t MSG_SIZE = 9;
        StatusType _type = StatusType::NONE;

    public:
        GetExtendedStatusCommand();

        bool parse(const char *data, uint16_t len) override;
    };

    class CommandFactory
    {
    private:
        CommandFactory() {};

    public:
        static Command *parse(const char *data, uint16_t len);
    };

} // namespace SynScanControl

#endif /* COMMAND_H */