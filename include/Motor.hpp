#ifndef MOTOR_H
#define MOTOR_H

//#define DEBUG

#define INFINITE 0xFFFFFF
#define ARCSEC_TO_DEGREE 0.000277778f
#define SIDEREAL_SPEED_ARCSEC 15.04108444f

#include <stdint.h>
#include <AccelStepper.h>

#include "Enums.hpp"

class Motor
{
public:
    static const uint32_t SLOW_MICROSTEPS = 32;
    static const uint32_t FAST_MICROSTEPS = 2;
    static const uint32_t HIGH_SPEED_RATIO = SLOW_MICROSTEPS / FAST_MICROSTEPS;
    static const uint32_t FULL_STEPS_PER_REV = 141000;
    static const uint32_t MICROSTEPS_PER_REV = SLOW_MICROSTEPS * FULL_STEPS_PER_REV;
    static const uint32_t MAX_PULSE_PER_SECOND = 1500;
    static constexpr float MOTOR_ACCEL = 1250.0;
    static constexpr float SIDEREAL_PULSE_PER_SECOND = (float)MICROSTEPS_PER_REV / 360.0 * ARCSEC_TO_DEGREE * SIDEREAL_SPEED_ARCSEC;

    Motor(AxisEnum axis, uint8_t M0, uint8_t M1, uint8_t M2, uint8_t STEP, uint8_t DIR);

    uint32_t getPosition() const;
    uint32_t getTargetPosition() const;
    float getSpeed();
    SlewTypeEnum getSlewType() const;
    SlewSpeedEnum getSlewSpeed() const;
    SlewDirectionEnum getSlewDirection() const;
    bool isMoving() const;

    void setPosition(uint32_t position);
    void setOrigPosition(uint32_t position);
    void setTargetPosition(uint32_t position);
    void setStepPeriod(uint32_t stepPeriod);
    void setSlewType(SlewTypeEnum type);
    void setSlewSpeed(SlewSpeedEnum type);
    void setSlewDir(SlewDirectionEnum type);
    void setMicrosteps(uint8_t s);
    void setMotion(bool moving);

    void tick();
    void longTick();

private:
    AxisEnum _axis;
    uint8_t _M0;
    uint8_t _M1;
    uint8_t _M2;
    uint8_t _STEP;
    uint8_t _DIR;

    AccelStepper _stepper;

    uint32_t _pecPeriod = 0;
    uint32_t _position = 0x800000; // middle of range
    uint32_t _originalPosition = INFINITE;
    uint32_t _targetPosition = INFINITE;
    uint32_t _stepPeriod = 0;
    bool _moving = false;
    bool _toStop = false;

    SlewTypeEnum _type = SlewTypeEnum::NONE;
    SlewSpeedEnum _speed = SlewSpeedEnum::NONE;
    SlewDirectionEnum _dir = SlewDirectionEnum::NONE;
};

#endif /* MOTOR_H */