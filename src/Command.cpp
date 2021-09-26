#include "Command.hpp"
#include "HexConversionUtils.hpp"

AxisEnum Command::parseAxis(char c)
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

CommandEnum Command::getCommand() const
{
    return _cmd;
}

AxisEnum Command::getAxis() const
{
    return _axis;
}

bool Command::hasInitialized() const
{
    return _has_init;
}

bool GetterCommand::parse(const char *data, uint16_t len)
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

// TODO: pulseguide support

SetPositionCommand::SetPositionCommand()
{
    _cmd = CommandEnum::SET_POSITION_CMD;
}

bool SetPositionCommand::parse(const char *data, uint16_t len)
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

uint32_t SetPositionCommand::getPosition() const
{
    return _position;
}

InitializationDoneCommand::InitializationDoneCommand()
{
    _cmd = CommandEnum::INITIALIZATION_DONE_CMD;
}

bool InitializationDoneCommand::parse(const char *data, uint16_t len)
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

SetMotionModeCommand::SetMotionModeCommand()
{
    _cmd = CommandEnum::SET_MOTION_MODE_CMD;
}

bool SetMotionModeCommand::parse(const char *data, uint16_t len)
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

SlewTypeEnum SetMotionModeCommand::getType() const
{
    return _type;
}

SlewSpeedEnum SetMotionModeCommand::getSpeed() const
{
    return _speed;
}

SlewDirectionEnum SetMotionModeCommand::getDir() const
{
    return _dir;
}

SetGotoTargetCommand::SetGotoTargetCommand()
{
    _cmd = CommandEnum::SET_GOTO_TARGET_CMD;
}

bool SetGotoTargetCommand::parse(const char *data, uint16_t len)
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

uint32_t SetGotoTargetCommand::getPosition() const
{
    return _position;
}

SetGotoTargetIncrementCommand::SetGotoTargetIncrementCommand()
{
    _cmd = CommandEnum::SET_GOTO_TARGET_INCREMENT_CMD;
}

bool SetGotoTargetIncrementCommand::parse(const char *data, uint16_t len)
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

uint32_t SetGotoTargetIncrementCommand::getIncrement() const
{
    return _increment;
}

SetBreakPointIncrementCommand::SetBreakPointIncrementCommand()
{
    _cmd = CommandEnum::SET_BREAKPOINT_INCREMENT_CMD;
}

bool SetBreakPointIncrementCommand::parse(const char *data, uint16_t len)
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

uint32_t SetBreakPointIncrementCommand::getIncrement() const
{
    return _increment;
}

SetStepPeriodCommand::SetStepPeriodCommand()
{
    _cmd = CommandEnum::SET_STEP_PERIOD_CMD;
}

bool SetStepPeriodCommand::parse(const char *data, uint16_t len)
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

uint32_t SetStepPeriodCommand::getPeriod() const
{
    return _period;
}

StartMotionCommand::StartMotionCommand()
{
    _cmd = CommandEnum::START_MOTION_CMD;
}

bool StartMotionCommand::parse(const char *data, uint16_t len)
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

StopMotionCommand::StopMotionCommand()
{
    _cmd = CommandEnum::STOP_MOTION_CMD;
}

bool StopMotionCommand::parse(const char *data, uint16_t len)
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

InstantStopCommand::InstantStopCommand()
{
    _cmd = CommandEnum::INSTANT_STOP_CMD;
}

bool InstantStopCommand::parse(const char *data, uint16_t len)
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

SetSwitchCommand::SetSwitchCommand()
{
    _cmd = CommandEnum::SET_SWITCH_CMD;
}

bool SetSwitchCommand::parse(const char *data, uint16_t len)
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

SetAudioguideSpeedCommand::SetAudioguideSpeedCommand()
{
    _cmd = CommandEnum::SET_AUTOGUIDE_SPEED_CMD;
}

bool SetAudioguideSpeedCommand::parse(const char *data, uint16_t len)
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

SetPolarLEDBrightnessCommand::SetPolarLEDBrightnessCommand()
{
    _cmd = CommandEnum::SET_POLAR_LED_BRIGHTNESS_CMD;
}

bool SetPolarLEDBrightnessCommand::parse(const char *data, uint16_t len)
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

GetCountsPerRevCommand::GetCountsPerRevCommand()
{
    _cmd = CommandEnum::GET_COUNTS_PER_REV_CMD;
}

GetTimerFreqCommand::GetTimerFreqCommand()
{
    _cmd = CommandEnum::GET_TIMER_FREQ_CMD;
}

GetGotoTargetPositionCommand::GetGotoTargetPositionCommand()
{
    _cmd = CommandEnum::GET_GOTO_TARGET_CMD;
}

GetStepPeriodCommand::GetStepPeriodCommand()
{
    _cmd = CommandEnum::GET_STEP_PERIOD_CMD;
}

GetPositionCommand::GetPositionCommand()
{
    _cmd = CommandEnum::GET_POSITION_CMD;
}

GetStatusCommand::GetStatusCommand()
{
    _cmd = CommandEnum::GET_STATUS_CMD;
}

GetHighSpeedRatioCommand::GetHighSpeedRatioCommand()
{
    _cmd = CommandEnum::GET_HIGH_SPEED_RATIO_CMD;
}

GetSiderealPeriodCommand::GetSiderealPeriodCommand()
{
    _cmd = CommandEnum::GET_SIDEREAL_PERIOD_CMD;
}
GetAxisPositionCommand::GetAxisPositionCommand()
{
    _cmd = CommandEnum::GET_AXIS_POSITION_CMD;
}

GetVersionCommand::GetVersionCommand()
{
    _cmd = CommandEnum::GET_VERSION_CMD;
}

GetPECPeriodCommand::GetPECPeriodCommand()
{
    _cmd = CommandEnum::GET_PEC_PERIOD_CMD;
}

GetExtendedStatusCommand::GetExtendedStatusCommand()
{
    _cmd = CommandEnum::GET_EXTENDED_STATUS_CMD;
}

bool GetExtendedStatusCommand::parse(const char *data, uint16_t len)
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

Command *CommandFactory::parse(const char *data, uint16_t len)
{
    // We need a buffer of at least length 2 to determine
    // the command type (any errors associated with a buffer
    // of invalid length greater than 2 will be handled
    // downstream)
    if (len < 2)
    {
        return nullptr;
    }
    Command *cmd = nullptr;
    switch (data[1])
    {
    case (char)CommandEnum::SET_POSITION_CMD:
    {
        cmd = new SetPositionCommand();
        break;
    }
    case (char)CommandEnum::INITIALIZATION_DONE_CMD:
    {
        cmd = new InitializationDoneCommand();
        break;
    }
    case (char)CommandEnum::SET_MOTION_MODE_CMD:
    {
        cmd = new SetMotionModeCommand();
        break;
    }
    case (char)CommandEnum::SET_GOTO_TARGET_CMD:
    {
        cmd = new SetGotoTargetCommand();
        break;
    }
    case (char)CommandEnum::SET_GOTO_TARGET_INCREMENT_CMD:
    {
        cmd = new SetGotoTargetIncrementCommand();
        break;
    }
    case (char)CommandEnum::SET_BREAKPOINT_INCREMENT_CMD:
    {
        cmd = new SetBreakPointIncrementCommand();
        break;
    }
    case (char)CommandEnum::SET_STEP_PERIOD_CMD:
    {
        cmd = new SetStepPeriodCommand();
        break;
    }
    case (char)CommandEnum::START_MOTION_CMD:
    {
        cmd = new StartMotionCommand();
        break;
    }
    case (char)CommandEnum::STOP_MOTION_CMD:
    {
        cmd = new StopMotionCommand();
        break;
    }
    case (char)CommandEnum::INSTANT_STOP_CMD:
    {
        cmd = new InstantStopCommand();
        break;
    }
    case (char)CommandEnum::SET_SWITCH_CMD:
    {
        cmd = new SetSwitchCommand();
        break;
    }
    case (char)CommandEnum::SET_AUTOGUIDE_SPEED_CMD:
    {
        cmd = new SetAudioguideSpeedCommand();
        break;
    }
    case (char)CommandEnum::SET_POLAR_LED_BRIGHTNESS_CMD:
    {
        cmd = new SetPolarLEDBrightnessCommand;
        break;
    }
    case (char)CommandEnum::GET_COUNTS_PER_REV_CMD:
    {
        cmd = new GetCountsPerRevCommand();
        break;
    }
    case (char)CommandEnum::GET_TIMER_FREQ_CMD:
    {
        cmd = new GetTimerFreqCommand();
        break;
    }
    case (char)CommandEnum::GET_GOTO_TARGET_CMD:
    {
        cmd = new GetGotoTargetPositionCommand();
        break;
    }
    case (char)CommandEnum::GET_STEP_PERIOD_CMD:
    {
        cmd = new GetStepPeriodCommand();
        break;
    }
    case (char)CommandEnum::GET_POSITION_CMD:
    {
        cmd = new GetPositionCommand();
        break;
    }
    case (char)CommandEnum::GET_STATUS_CMD:
    {
        cmd = new GetStatusCommand();
        break;
    }
    case (char)CommandEnum::GET_HIGH_SPEED_RATIO_CMD:
    {
        cmd = new GetHighSpeedRatioCommand();
        break;
    }
    case (char)CommandEnum::GET_SIDEREAL_PERIOD_CMD:
    {
        cmd = new GetSiderealPeriodCommand();
        break;
    }
    case (char)CommandEnum::GET_AXIS_POSITION_CMD:
    {
        cmd = new GetAxisPositionCommand();
        break;
    }
    case (char)CommandEnum::GET_VERSION_CMD:
    {
        cmd = new GetVersionCommand();
        break;
    }
    case (char)CommandEnum::GET_PEC_PERIOD_CMD:
    {
        cmd = new GetPECPeriodCommand();
        break;
    }
    case (char)CommandEnum::GET_EXTENDED_STATUS_CMD:
    {
        cmd = new GetExtendedStatusCommand();
        break;
    }
    default:
        break;
    }
    return cmd;
}