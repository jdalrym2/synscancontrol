#include <Arduino.h>
#include <sstream>

#include "Motor.hpp"

Motor::Motor(AxisEnum axis, uint8_t M0, uint8_t M1, uint8_t M2, uint8_t STEP, uint8_t DIR, uint32_t startPos, Logger *logger)
{
    _axis = axis;
    _M0 = M0;
    _M1 = M1;
    _M2 = M2;
    _STEP = STEP;
    _DIR = DIR;
    _stepper = CustomAccelStepper(AccelStepper::DRIVER, STEP, DIR);
    _position = startPos;
    _stepperPosition = _computeStepperPosition(startPos);
    _logger = logger;
}

void Motor::begin()
{
    setMicrosteps(SLOW_MICROSTEPS);
    _stepper.setPinsInverted(false, false, false);
    _stepper.setAcceleration(MOTOR_ACCEL);
    _stepper.setMaxSpeed((float)MAX_PULSE_PER_SECOND / 2); // Tweaking this to avoid stall behavior at fast speeds
    _stepper.setCurrentPosition(_stepperPosition);
    _stepper.moveTo(_stepperTargetPosition);
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
    return abs(_stepper.speed());
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
    _stepperPosition = _computeStepperPosition(_position);
}

void Motor::setStepperPosition(long position)
{
    _position = _computePosition(position);
    _stepperPosition = position;
}

void Motor::setTargetPosition(uint32_t position)
{
    _targetPosition = position;
    _stepperTargetPosition = _computeStepperPosition(position);
    _stepper.moveTo(_stepperTargetPosition);

    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Current position: 0x" << std::hex << getPosition();
    _logger->debug(&log);
    log.str("");
    log << "Axis: " << int(_axis) << "; Setting target position (reference) to: 0x" << std::hex << _targetPosition << ", 0x" << std::hex << _stepperTargetPosition;
    _logger->debug(&log);
}

void Motor::setStepperTargetPosition(long position)
{
    _targetPosition = _computePosition(position);
    _stepperTargetPosition = position;
    _stepper.moveTo(_stepperTargetPosition);

    // Debug
    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Current position: 0x" << std::hex << getPosition();
    _logger->debug(&log);
    log.str("");
    log << "Axis: " << int(_axis) << "; Setting target position (stepper) to: 0x" << std::hex << _targetPosition << ", 0x" << std::hex << _stepperTargetPosition;
    _logger->debug(&log);
}

void Motor::setStepPeriod(uint32_t stepPeriod)
{

    // Debug
    std::ostringstream log;
    log << "Axis: " << int(_axis) << "; Setting step period to: " << stepPeriod;
    _logger->debug(&log);

    _stepPeriod = stepPeriod;
    float speed = (stepPeriod > 0) ? (float)MAX_PULSE_PER_SECOND / stepPeriod : MAX_PULSE_PER_SECOND;
    _stepper.setMaxSpeed(speed);
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
                setStepperTargetPosition(STEPPER_INFINITE);
            }
            else
            {
                setStepperTargetPosition(STEPPER_NINFINITE);
            }
        }
        else if (getSlewType() == SlewTypeEnum::GOTO)
        {
            setStepPeriod(0);
        }
    }
    else
    {
        _toStop = true;
        uint32_t stepsToStop = (uint32_t)((_stepper.speed() * _stepper.speed()) / (2.0 * MOTOR_ACCEL)) + 1;
        if (stepsToStop != 0)
            stepsToStop -= 1; // I think

        // Debug
        std::ostringstream log;
        log << "Axis: " << int(_axis) << "; About to stop! Speed: " << _stepper.speed() << ", Steps to stop: " << stepsToStop;
        _logger->debug(&log);

        if (getSlewDirection() == SlewDirectionEnum::CW)
        {
            setStepperTargetPosition(_stepper.currentPosition() + stepsToStop);
        }
        else
        {
            setStepperTargetPosition(_stepper.currentPosition() - stepsToStop);
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
        // Similar to AccelStepper::Run
        if (_stepper.runSpeed())
        {
            _stepper.computeNewSpeed();
            _steps++;
        }
    }
}

void Motor::longTick()
{
    // Sync stepper position with Accelstepper
    setStepperPosition(_stepper.currentPosition());

    // Debug
    if (_moving)
    {
        std::ostringstream log;
        log << "Axis: " << int(_axis) << "; STGT: 0x" << std::hex << _stepper.targetPosition() << ";";
        log << " DTG: " << std::dec << _stepper.distanceToGo() << ";";
        log << " SPOS: 0x" << std::hex << _stepper.currentPosition() << ";";
        log << " POS: 0x" << std::hex << getPosition() << ";";
        log << " TOSTOP: " << std::dec << _toStop << "; MOVING: " << _moving << ";";
        log << " _STEPS: " << std::dec << _steps;
        _logger->debug(&log);
    }

    if (_moving)
    {
        if (_toStop && !_stepper.isRunning())
        {
            _toStop = false;
            _moving = false;
        }

        if (_type == SlewTypeEnum::GOTO && !_stepper.isRunning())
        {
            _toStop = false;
            _moving = false;
        }
    }
}

long Motor::_computeStepperPosition(uint32_t currentPosition)
{
    long stepperPosition;
    if (currentPosition == POSITION_NINFINITE)
        stepperPosition = STEPPER_NINFINITE;
    else if (currentPosition == POSITION_INFINITE)
        stepperPosition = STEPPER_INFINITE;
    else
    {

        while (currentPosition > MAX_POSITION)
            currentPosition -= MICROSTEPS_PER_REV;
        while (currentPosition < MIN_POSITION)
            currentPosition += MICROSTEPS_PER_REV;

        stepperPosition = STEPPER_MID_POSITION;
        if (_speed == SlewSpeedEnum::FAST)
            stepperPosition += (long)(currentPosition - MID_POSITION) / (long)HIGH_SPEED_RATIO;
        else
            stepperPosition += (long)(currentPosition - MID_POSITION);
    }
    return stepperPosition;
}

uint32_t Motor::_computePosition(long stepperPosition)
{
    uint32_t currentPosition;
    if (stepperPosition == STEPPER_NINFINITE)
        currentPosition = POSITION_NINFINITE;
    else if (stepperPosition == STEPPER_INFINITE)
        currentPosition = POSITION_INFINITE;
    else
    {
        currentPosition = MID_POSITION;
        if (_speed == SlewSpeedEnum::FAST)
            currentPosition -= HIGH_SPEED_RATIO * (STEPPER_MID_POSITION - stepperPosition);
        else
            currentPosition -= (STEPPER_MID_POSITION - stepperPosition);

        while (currentPosition > MAX_POSITION)
            currentPosition -= MICROSTEPS_PER_REV;
        while (currentPosition < MIN_POSITION)
            currentPosition += MICROSTEPS_PER_REV;
    }
    return currentPosition;
}