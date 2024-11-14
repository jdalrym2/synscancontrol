#ifndef MOTOR_H
#define MOTOR_H

// #define DEBUG

#define ARCSEC_TO_DEGREE 0.000277778f
#define SIDEREAL_SPEED_ARCSEC 15.04108444f

#include <stdint.h>
#include "InterruptStepper.hpp"
#include "Logger.hpp"
#include "Enums.hpp"

namespace SynScanControl
{
    class Motor
    {
    public:
        static const uint32_t SLOW_MICROSTEPS = 32;
        static const uint32_t FAST_MICROSTEPS = 8;
        static const uint32_t HIGH_SPEED_RATIO = SLOW_MICROSTEPS / FAST_MICROSTEPS;
        static const uint32_t FULL_STEPS_PER_REV = 141000;
        static const uint32_t MICROSTEPS_PER_REV = SLOW_MICROSTEPS * FULL_STEPS_PER_REV;
        static const uint32_t MAX_PULSE_PER_SECOND = 20000;
        static constexpr float MOTOR_ACCEL = 5000.0;
        static constexpr float SIDEREAL_STEP_PER_SECOND = (float)MICROSTEPS_PER_REV / 360.0 * ARCSEC_TO_DEGREE * SIDEREAL_SPEED_ARCSEC;
        static constexpr float SIDEREAL_PULSE_PER_STEP = MAX_PULSE_PER_SECOND / SIDEREAL_STEP_PER_SECOND;

        // Note we only have 24 bits to store position given the Synscan protocol
        // Note that this position and Accelstepper positions may be different,
        // based on whether or not we are doing slow or fast microstepping
        static const uint32_t POSITION_INFINITE = 0xFFFFFF;
        static const uint32_t POSITION_NINFINITE = 0x000000;

        static const long STEPPER_INFINITE = std::numeric_limits<long>::max() / 2;
        static const long STEPPER_NINFINITE = std::numeric_limits<long>::min() / 2;

        Motor(AxisEnum axis, uint8_t M0, uint8_t M1, uint8_t M2, uint8_t STEP, uint8_t DIR, uint32_t startPos, bool reversed, Logger *logger);

        void begin();
        uint32_t getPosition() const;
        uint32_t getTargetPosition() const;
        float getSpeed();
        SlewTypeEnum getSlewType() const;
        SlewSpeedEnum getSlewSpeed() const;
        SlewDirectionEnum getSlewDirection() const;
        bool isMoving() const;

        void setPosition(uint32_t position);
        void setTargetPosition(uint32_t position);
        void setStepPeriod(uint32_t stepPeriod);
        void setSlewType(SlewTypeEnum type);
        void setSlewSpeed(SlewSpeedEnum type);
        void setSlewDir(SlewDirectionEnum type);
        void setMotion(bool moving);
        void setMicrosteps(uint8_t s);

        void doAStep();
        void step();
        int32_t stepperDistanceToGo();
        void computeNewSpeed();

        void tick();
        void longTick();

        bool useAccel() { return (_type == SlewTypeEnum::GOTO || _speed == SlewSpeedEnum::FAST); };

    private:
        AxisEnum _axis;
        uint8_t _M0;
        uint8_t _M1;
        uint8_t _M2;
        uint8_t _STEP;
        uint8_t _DIR;

        InterruptStepper _stepper;
        Logger *_logger;

        uint32_t _ticker = 0;
        bool _moving = false;
        bool _toStop = false;

        uint32_t _pecPeriod = 0;
        uint32_t _stepPeriod = 6;
        volatile uint32_t _position = 0x800000;
        uint32_t _maxPosition = _position + MICROSTEPS_PER_REV / 2;
        uint32_t _minPosition = _position - MICROSTEPS_PER_REV / 2;
        uint32_t _targetPosition = POSITION_INFINITE;

        SlewTypeEnum _type = SlewTypeEnum::NONE;
        SlewSpeedEnum _speed = SlewSpeedEnum::NONE;
        SlewDirectionEnum _dir = SlewDirectionEnum::NONE;
    };
} // namespace SynScanControl

#endif /* MOTOR_H */