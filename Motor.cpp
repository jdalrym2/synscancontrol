#include "Motor.hpp"

Motor::Motor(AxisEnum axis)
{
    _axis = axis;
}

uint32_t Motor::getPosition() const
{
    return _position;
}

SlewDirectionEnum Motor::getSlewDirection() const
{
    return _dir;
}

bool Motor::isMoving() const
{
    return _moving;
}

void Motor::setPosition(uint32_t position)
{
    _position = position;
}

void Motor::setOrigPosition(uint32_t position)
{
    _originalPosition = position;
}

void Motor::setTargetPosition(uint32_t position)
{
    _targetPosition = position;
}

void Motor::setStepPeriod(uint32_t stepPeriod)
{
    _stepPeriod = stepPeriod;
}

void Motor::setSlewType(SlewTypeEnum type)
{
    _type = type;
}

void Motor::setSlewSpeed(SlewSpeedEnum speed)
{
    _speed = speed;
}

void Motor::setSlewDir(SlewDirectionEnum dir)
{
    _dir = dir;
}