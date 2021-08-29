#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#include "Enums.hpp"

class Motor
{
public:
    Motor(AxisEnum axis);

    uint32_t getPosition() const;
    SlewDirectionEnum getSlewDirection() const;
    bool isMoving() const;

    void setPosition(uint32_t position);
    void setOrigPosition(uint32_t position);
    void setTargetPosition(uint32_t position);
    void setStepPeriod(uint32_t stepPeriod);
    void setSlewType(SlewTypeEnum type);
    void setSlewSpeed(SlewSpeedEnum type);
    void setSlewDir(SlewDirectionEnum type);

private:
    AxisEnum _axis;
    uint32_t _pecPeriod = 0;
    uint32_t _position = 0x800000; // middle of range
    uint32_t _originalPosition = 0xFFFFFF;
    uint32_t _targetPosition = 0xFFFFFF;
    uint32_t _stepPeriod = 0;
    bool _moving = false;

    SlewTypeEnum _type = SlewTypeEnum::NONE;
    SlewSpeedEnum _speed = SlewSpeedEnum::NONE;
    SlewDirectionEnum _dir = SlewDirectionEnum::NONE;
};

#endif /* MOTOR_H */