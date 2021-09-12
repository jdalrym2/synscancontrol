#ifndef COMMAND_H
#define COMMAND_H

#include "Enums.hpp"
#include "HexConversionUtils.hpp"

class Command
{
protected:
    CommandEnum _cmd = CommandEnum::UNKNOWN_CMD;
    AxisEnum _axis = AxisEnum::AXIS_NONE;
    bool _has_init = false;

protected:
    Command(){};

    static AxisEnum parseAxis(char c)
    {
        switch (c)
        {
        case '1':
            return AxisEnum::AXIS_RA;
        case '2':
            return AxisEnum::AXIS_DEC;
        case '3':
            return AxisEnum::AXIS_BOTH;
        default:
            return AxisEnum::AXIS_NONE;
        }
    }

public:
    virtual bool parse(const char *data, uint16_t len);

    CommandEnum getCommand() const
    {
        return _cmd;
    }

    AxisEnum getAxis() const
    {
        return _axis;
    }

    bool hasInitialized() const
    {
        return _has_init;
    }
};

class GetterCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 3;

public:
    GetterCommand(){};

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

// TODO: pulseguide support

class SetPositionCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 9;
    uint32_t _position = 0;

public:
    SetPositionCommand()
    {
        _cmd = CommandEnum::SET_POSITION_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _position = HexConversionUtils::parseToHex<uint32_t>(data + 3, 6);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }

    uint32_t getPosition() const
    {
        return _position;
    }
};

class InitializationDoneCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 3;

public:
    InitializationDoneCommand()
    {
        _cmd = CommandEnum::INITIALIZATION_DONE_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class SetMotionModeCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 5;
    SlewTypeEnum _type = SlewTypeEnum::NONE;
    SlewSpeedEnum _speed = SlewSpeedEnum::NONE;
    SlewDirectionEnum _dir = SlewDirectionEnum::NONE;

public:
    SetMotionModeCommand()
    {
        _cmd = CommandEnum::SET_MOTION_MODE_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    // Parse axis
                    _axis = parseAxis(data[2]);

                    // Parse slew type and speed
                    char mode = data[3];
                    switch (mode)
                    {
                    case '0':
                        _type = SlewTypeEnum::GOTO;
                        _speed = SlewSpeedEnum::FAST;
                        _has_init = true;
                        success = true;
                        break;
                    case '1':
                        _type = SlewTypeEnum::TRACKING;
                        _speed = SlewSpeedEnum::SLOW;
                        _has_init = true;
                        success = true;
                        break;
                    case '2':
                        _type = SlewTypeEnum::GOTO;
                        _speed = SlewSpeedEnum::SLOW;
                        _has_init = true;
                        success = true;
                        break;
                    case '3':
                        _type = SlewTypeEnum::TRACKING;
                        _speed = SlewSpeedEnum::FAST;
                        _has_init = true;
                        success = true;
                        break;
                    default:
                        // Bad mode
                        break;
                    }

                    // Parse slew direction
                    char dir = data[4] & 0x01;
                    if (dir)
                    {
                        _dir = SlewDirectionEnum::CCW;
                    }
                    else
                    {
                        _dir = SlewDirectionEnum::CW;
                    }
                }
            }
        }
        return success;
    }

    SlewTypeEnum getType() const
    {
        return _type;
    }

    SlewSpeedEnum getSpeed() const
    {
        return _speed;
    }

    SlewDirectionEnum getDir() const
    {
        return _dir;
    }
};

class SetGotoTargetCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 9;
    uint32_t _position = 0;

public:
    SetGotoTargetCommand()
    {
        _cmd = CommandEnum::SET_GOTO_TARGET_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _position = HexConversionUtils::parseToHex<uint32_t>(data + 3, 6);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }

    uint32_t getPosition() const
    {
        return _position;
    }
};

class SetGotoTargetIncrementCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 9;
    uint32_t _increment = 0;

public:
    SetGotoTargetIncrementCommand()
    {
        _cmd = CommandEnum::SET_GOTO_TARGET_INCREMENT_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _increment = HexConversionUtils::parseToHex<uint32_t>(data + 3, 6);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }

    uint32_t getIncrement() const
    {
        return _increment;
    }
};

class SetStepPeriodCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 9;
    uint32_t _period = 0;

public:
    SetStepPeriodCommand()
    {
        _cmd = CommandEnum::SET_STEP_PERIOD_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _period = HexConversionUtils::parseToHex<uint32_t>(data + 3, 6);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }

    uint32_t getPeriod() const
    {
        return _period;
    }
};

class StartMotionCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 3;

public:
    StartMotionCommand()
    {
        _cmd = CommandEnum::START_MOTION_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class StopMotionCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 3;

public:
    StopMotionCommand()
    {
        _cmd = CommandEnum::STOP_MOTION_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class InstantStopCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 3;

public:
    InstantStopCommand()
    {
        _cmd = CommandEnum::INSTANT_STOP_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class SetSwitchCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 4;
    bool _active = false;

public:
    SetSwitchCommand()
    {
        _cmd = CommandEnum::SET_SWITCH_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _active = HexConversionUtils::parseToHex<uint32_t>(data + 3, 1);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class SetAudioguideSpeedCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 4;
    uint32_t _speed = 1000; // 1000 = x1 speed

public:
    SetAudioguideSpeedCommand()
    {
        _cmd = CommandEnum::SET_AUTOGUIDE_SPEED_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    uint32_t val = HexConversionUtils::parseToHex<uint32_t>(data + 3, 1);
                    switch (val)
                    {
                    case 0:
                        _speed = 1000;
                        break;
                    case 1:
                        _speed = 750;
                        break;
                    case 2:
                        _speed = 500;
                        break;
                    case 3:
                        _speed = 250;
                        break;
                    case 4:
                        _speed = 125;
                        break;
                    default:
                        _speed = 1000;
                        break;
                    }
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class SetPolarLEDBrightnessCommand : public Command
{
private:
    static const uint16_t MSG_SIZE = 5;
    uint16_t _value = 0;

public:
    SetPolarLEDBrightnessCommand()
    {
        _cmd = CommandEnum::SET_POLAR_LED_BRIGHTNESS_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    _value = HexConversionUtils::parseToHex<uint32_t>(data + 3, 2);
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class GetCountsPerRevCommand : public GetterCommand
{
public:
    GetCountsPerRevCommand()
    {
        _cmd = CommandEnum::GET_COUNTS_PER_REV_CMD;
    }
};

class GetTimerFreqCommand : public GetterCommand
{
public:
    GetTimerFreqCommand()
    {
        _cmd = CommandEnum::GET_TIMER_FREQ_CMD;
    }
};

class GetGotoTargetPositionCommand : public GetterCommand
{
public:
    GetGotoTargetPositionCommand()
    {
        _cmd = CommandEnum::GET_GOTO_TARGET_CMD;
    }
};

class GetStepPeriodCommand : public GetterCommand
{
public:
    GetStepPeriodCommand()
    {
        _cmd = CommandEnum::GET_STEP_PERIOD_CMD;
    }
};

class GetPositionCommand : public GetterCommand
{
public:
    GetPositionCommand()
    {
        _cmd = CommandEnum::GET_POSITION_CMD;
    }
};

class GetStatusCommand : public GetterCommand
{
public:
    GetStatusCommand()
    {
        _cmd = CommandEnum::GET_STATUS_CMD;
    }
};

class GetHighSpeedRatioCommand : public GetterCommand
{
public:
    GetHighSpeedRatioCommand()
    {
        _cmd = CommandEnum::GET_HIGH_SPEED_RATIO_CMD;
    }
};

class GetSiderealPeriodCommand : public GetterCommand
{
public:
    GetSiderealPeriodCommand()
    {
        _cmd = CommandEnum::GET_SIDEREAL_PERIOD_CMD;
    }
};

class GetAxisPositionCommand : public GetterCommand
{
public:
    GetAxisPositionCommand()
    {
        _cmd = CommandEnum::GET_AXIS_POSITION_CMD;
    }
};

class GetVersionCommand : public GetterCommand
{
public:
    GetVersionCommand()
    {
        _cmd = CommandEnum::GET_VERSION_CMD;
    }
};

class GetPECPeriodCommand : public GetterCommand
{
public:
    GetPECPeriodCommand()
    {
        _cmd = CommandEnum::GET_PEC_PERIOD_CMD;
    }
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
    GetExtendedStatusCommand()
    {
        _cmd = CommandEnum::GET_EXTENDED_STATUS_CMD;
    }

    bool parse(const char *data, uint16_t len) override
    {
        bool success = false;
        if (len == MSG_SIZE)
        {
            if (data[0] == ':')
            {
                char header = data[1];
                if (header == (char)_cmd)
                {
                    _axis = parseAxis(data[2]);
                    uint32_t val = HexConversionUtils::parseToHex<uint32_t>(data + 3, 6);
                    if (val == 1)
                    {
                        _type = StatusType::STATUS_EX;
                    }
                    else if (val == 0)
                    {
                        _type = StatusType::POSITION;
                    }
                    _has_init = true;
                    success = true;
                }
            }
        }
        return success;
    }
};

class CommandFactory
{
private:
    CommandFactory(){};

public:
    static Command *parse(const char *data, uint16_t len);
};

#endif /* COMMAND_H */