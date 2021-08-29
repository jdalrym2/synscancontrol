#include "Command.hpp"

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