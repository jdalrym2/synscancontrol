#include "Motor.hpp"

Motor::Motor(AxisEnum axis, uint8_t M0, uint8_t M1, uint8_t M2, uint8_t STEP, uint8_t DIR)
{
    _axis = axis;
    _M0 = M0;
    _M1 = M1;
    _M2 = M2;
    _STEP = STEP;
    _DIR = DIR;

    setMicrosteps(64);
    _stepper = AccelStepper(AccelStepper::DRIVER, STEP, DIR);
    _stepper.setAcceleration(MOTOR_ACCEL);
    _stepper.setMaxSpeed((float)MAX_PULSE_PER_SECOND);
    _stepper.setCurrentPosition(_position);
    _stepper.moveTo(_targetPosition);
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
    _stepper.setCurrentPosition(position);
#ifdef DEBUG
    Serial.print("Setting current position: ");
    Serial.println(position);
#endif
}

void Motor::setOrigPosition(uint32_t position)
{
    _originalPosition = position;
}

void Motor::setTargetPosition(uint32_t position)
{
    _targetPosition = position;
    _stepper.moveTo(position);
}

void Motor::setStepPeriod(uint32_t stepPeriod)
{
    _stepPeriod = (stepPeriod < MAX_PULSE_PER_SECOND) ? stepPeriod : MAX_PULSE_PER_SECOND;
    _stepper.setMaxSpeed((float)stepPeriod);
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

void Motor::setMotion(bool moving)
{
    if (moving)
    {
        _moving = true;
        _toStop = false;
    }
    else
    {
        _toStop = true;
        uint32_t stepsToStop = (uint32_t)((_stepper.speed() * _stepper.speed()) / (2.0 * MOTOR_ACCEL)) + 1;

#ifdef DEBUG
        Serial.println("About to stop!");
        Serial.println(_stepper.speed());
        Serial.println(stepsToStop);
#endif
        if (getSlewDirection() == SlewDirectionEnum::CCW)
        {
            setTargetPosition(_stepper.currentPosition() - stepsToStop);
        }
        else
        {
            setTargetPosition(_stepper.currentPosition() + stepsToStop);
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
        if (getSlewType() == SlewTypeEnum::GOTO)
        {
            // TODO
        }
        if (!_toStop) // implied slew type is "tracking"
        {
            setTargetPosition((getSlewDirection() == SlewDirectionEnum::CCW) ? 0 : INFINITE);
        }
        _stepper.run();
    }
}

void Motor::longTick()
{
    _position = _stepper.currentPosition();

#ifdef DEBUG
    Serial.print("STGT: ");
    Serial.print(_stepper.targetPosition());
    Serial.print("; DTG: ");
    Serial.print(_stepper.distanceToGo());
    Serial.print("; SPOS: ");
    Serial.print(_stepper.currentPosition());
    Serial.print("; POS: ");
    Serial.println(getPosition());
#endif

    if (_toStop && _stepper.distanceToGo() == 0)
    {
        _toStop = false;
        _moving = false;
        setTargetPosition(INFINITE);
    }
}