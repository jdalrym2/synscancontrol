#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

#include "Enums.hpp"

class Motor
{
public:
    Motor(AxisEnum axis);
    ~Motor(){};

    uint32_t getPosition() const;

    void setPosition(uint32_t position);

private:
    AxisEnum _axis;
    uint32_t _pecPeriod = 0;
    uint32_t _position = 0x800000; // middle of range
    uint32_t _targetPosition = 0xFFFFFF;
    uint32_t _originalPosition = 0xFFFFFF;
};

Motor::Motor(AxisEnum axis)
{
    _axis = axis;
}

uint32_t Motor::getPosition() const
{
    return _position;
}

void Motor::setPosition(uint32_t position)
{
    _position = position;
}

#endif /* MOTOR_H_ */