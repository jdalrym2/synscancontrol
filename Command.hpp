#ifndef COMMAND_H_
#define COMMAND_H_

#include "Enums.hpp"
#include "HexConversionUtils.hpp"

class Command
{
protected:
    CommandEnum _cmd = CommandEnum::UNKNOWN_CMD;
    AxisEnum _axis = AxisEnum::AXIS_NONE;

protected:
    Command();

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
    virtual ~Command(){};

    CommandEnum getCommand() const
    {
        return _cmd;
    }

    AxisEnum getAxis() const
    {
        return _axis;
    }
};

// TODO: pulseguide support

class SetPositionCommand : public Command
{
private:
    static const uint32_t MSG_SIZE = 9;
    uint32_t _position;

public:
    SetPositionCommand()
    {
        _cmd = CommandEnum::SET_POSITION_CMD;
    }

    uint32_t getPosition() const
    {
        return _position;
    }
};

#endif /* COMMAND_H_ */