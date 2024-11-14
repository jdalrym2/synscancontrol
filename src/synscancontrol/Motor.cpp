#include <Arduino.h>
#include <sstream>

#include "Motor.hpp"

Motor::Motor(AxisEnum axis, uint8_t M0, uint8_t M1, uint8_t M2, uint8_t STEP, uint8_t DIR, uint32_t startPos, bool dirReverse, Logger *logger)
{
    _axis = axis;
    _M0 = M0;
    _M1 = M1;
    _M2 = M2;
    _STEP = STEP;
    _DIR = DIR;
    _stepper = InterruptStepper(STEP, DIR, MAX_PULSE_PER_SECOND, dirReverse);
    _position = startPos;
    _maxPosition = startPos + MICROSTEPS_PER_REV / 2;
    _minPosition = startPos - MICROSTEPS_PER_REV / 2;
    _logger = logger;
}

void Motor::begin()
{
    setMicrosteps(SLOW_MICROSTEPS);
    _stepper.setAcceleration(MOTOR_ACCEL);
    _stepper.setMaxSpeed(MAX_PULSE_PER_SECOND / 2);
    _stepper.initPosition(0);
    _stepper.setTargetPosition(0);
}

uint32_t Motor::getPosition() const
{
    return _position;
}

uint32_t Motor::getTargetPosition() const
{
    return _targetPosition;
}

float Motor::getSpeed()
{
    return abs(_stepper.getSpeed());
}

SlewTypeEnum Motor::getSlewType() const
{
    return _type;
}

SlewSpeedEnum Motor::getSlewSpeed() const
{
    return _speed;
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

void Motor::setTargetPosition(uint32_t position)
{

    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Current position: 0x" << std::hex << getPosition();
    _logger->debug(&log);
    log.str("");
    log << "Axis: " << int(_axis) << "; Setting target position (reference) to: 0x" << std::hex << position;
    _logger->debug(&log);

    _targetPosition = position;
}

void Motor::setStepPeriod(uint32_t stepPeriod)
{
    // Debug
    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Setting step period to: " << stepPeriod;
    _logger->debug(&log);

    _stepPeriod = (stepPeriod <= 4) ? 4 : stepPeriod;
}

void Motor::setSlewType(SlewTypeEnum type)
{
    _type = type;

    // Debug
    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Setting slew type to: ";
    if (type == SlewTypeEnum::GOTO)
        log << "GOTO";
    else if (type == SlewTypeEnum::TRACKING)
        log << "TRACKING";
    else if (type == SlewTypeEnum::NONE)
        log << "NONE";
    _logger->debug(&log);
}

void Motor::setSlewSpeed(SlewSpeedEnum speed)
{
    if (speed == SlewSpeedEnum::FAST)
        setMicrosteps(FAST_MICROSTEPS);
    else
        setMicrosteps(SLOW_MICROSTEPS);

    _speed = speed;

    // Debug
    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Setting slew speed to: ";
    if (speed == SlewSpeedEnum::FAST)
        log << "FAST";
    else if (speed == SlewSpeedEnum::SLOW)
        log << "SLOW";
    else if (speed == SlewSpeedEnum::NONE)
        log << "NONE";
    _logger->debug(&log);
}

void Motor::setSlewDir(SlewDirectionEnum dir)
{
    _dir = dir;

    // Debug
    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Setting slew direction to: ";
    if (dir == SlewDirectionEnum::CCW)
        log << "CCW";
    else if (dir == SlewDirectionEnum::CW)
        log << "CW";
    else if (dir == SlewDirectionEnum::NONE)
        log << "NONE";
    _logger->debug(&log);
}

void Motor::setMotion(bool moving)
{
    if (moving)
    {
        _moving = true;
        _toStop = false;
        if (getSlewType() == SlewTypeEnum::TRACKING)
        {
            if (getSlewDirection() == SlewDirectionEnum::CW)
            {
                _stepper.moveToInfinity();
            }
            else
            {
                _stepper.moveToNInfinity();
            }
        }
        else if (getSlewType() == SlewTypeEnum::GOTO)
        {
            // Move as fast as possible
            setStepPeriod(6);

            // Determine the number of steps at
            // the given direction to move the
            // stepper motor to the target position
            uint32_t numSteps = 0;
            if (_targetPosition > _position && _dir == SlewDirectionEnum::CW)
            {
                numSteps = _targetPosition - _position;
            }
            else if (_targetPosition <= _position && _dir == SlewDirectionEnum::CW)
            {
                numSteps = (_maxPosition - _position) + (_targetPosition - _minPosition);
            }
            else if (_targetPosition > _position && _dir == SlewDirectionEnum::CCW)
            {
                numSteps = (_position - _minPosition) + (_maxPosition - _targetPosition);
            }
            else
            {
                numSteps = _position - _targetPosition;
            }

            if (_speed == SlewSpeedEnum::FAST)
                numSteps /= HIGH_SPEED_RATIO;

            _stepper.setPosition(0);
            if (_dir == SlewDirectionEnum::CW)
            {
                _stepper.setTargetPosition(numSteps);
            }
            else
            {
                _stepper.setTargetPosition(-numSteps);
            }
        }
    }
    else
    {
        _toStop = true;

        // Debug
        /*std::ostringstream log;
        log << "Axis: " << int(_axis) << "; About to stop! Speed: " << _stepper.getSpeed() << ", Steps to stop: " << _stepper.stepsToStop();
        _logger->debug(&log);*/

        if (getSlewDirection() == SlewDirectionEnum::CW)
        {
            _stepper.setTargetPosition(_stepper.getPosition() + _stepper.stepsToStop());
        }
        else
        {
            _stepper.setTargetPosition(_stepper.getPosition() - _stepper.stepsToStop());
        }
    }
}

void Motor::setMicrosteps(uint8_t s)
{
    switch (s)
    {
    case 1:
        digitalWrite(_M0, LOW);
        digitalWrite(_M1, LOW);
        digitalWrite(_M2, LOW);
        break;
    case 2:
        digitalWrite(_M0, HIGH);
        digitalWrite(_M1, LOW);
        digitalWrite(_M2, LOW);
        break;
    case 4:
        digitalWrite(_M0, LOW);
        digitalWrite(_M1, HIGH);
        digitalWrite(_M2, LOW);
        break;
    case 8:
        digitalWrite(_M0, HIGH);
        digitalWrite(_M1, HIGH);
        digitalWrite(_M2, LOW);
        break;
    case 16:
        digitalWrite(_M0, LOW);
        digitalWrite(_M1, LOW);
        digitalWrite(_M2, HIGH);
        break;
    case 32:
        digitalWrite(_M0, HIGH);
        digitalWrite(_M1, HIGH);
        digitalWrite(_M2, HIGH);
        break;
    default:
        digitalWrite(_M0, HIGH);
        digitalWrite(_M1, HIGH);
        digitalWrite(_M2, HIGH);
        break;
    }
}

void Motor::tick()
{
    if (_moving)
    {
        // Return if we do not wish to perform a step
        if (useAccel())
        {
            if (!_stepper.getPulsesPerStep())
                return;
            if (++_ticker % _stepper.getPulsesPerStep() > 0)
                return;
        }
        else if (++_ticker % _stepPeriod > 0)
            return;

        // Do the step
        _stepper.run();

        // Implement accel / decel (GOTO only)
        if (useAccel())
            _stepper.computeNewSpeed();

        // Adjust position counter
        uint32_t numSteps = 1;
        if (_speed == SlewSpeedEnum::FAST)
            numSteps *= HIGH_SPEED_RATIO;
        if (_dir == SlewDirectionEnum::CW)
        {
            _position += numSteps;
            if (_position > _maxPosition)
                _position -= MICROSTEPS_PER_REV;
        }
        else
        {
            _position -= numSteps;
            if (_position < _minPosition)
                _position += MICROSTEPS_PER_REV;
        }
    }
}

void Motor::longTick()
{
    if (_moving)
    {
        // Debug
        /*if (int(_axis) == 2)
        {
            std::ostringstream log;
            log << "Axis: " << int(_axis) << ";";
            log << " TGT: 0x" << std::hex << _stepper.getTargetPosition() << ";";
            log << " POS: 0x" << std::hex << _stepper.getPosition() << ";";
            log << " DTG: " << std::dec << _stepper.distanceToGo() << ";";
            log << " SPEED: " << std::dec << _stepper.getSpeed() << ";";
            log << " PPS: " << std::dec << _stepper.getPulsesPerStep() << ";";
            log << " STS: " << std::dec << _stepper.stepsToStop() << ";";
            log << " TOSTOP: " << std::dec << _toStop << ";";
            log << " MOVING: " << _moving << ";";
            _logger->debug(&log);
        }*/

        if ((_toStop && !useAccel()) || !_stepper.isRunning())
        {
            _toStop = false;
            _moving = false;
            _stepper.setPosition(0);
        }
    }
}