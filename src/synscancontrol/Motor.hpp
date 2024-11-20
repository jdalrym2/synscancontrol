/*
 * Project Name: synscancontrol
 * File: [Filename.cpp]
 *
 * Copyright (C) 2024 Jon Dalrymple
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jon Dalrymple
 * Created: 13 November 2024
 * Description: Manages high-level stepper motor control logic
 */
#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "InterruptStepper.hpp"
#include "Constants.hpp"
#include "Logger.hpp"
#include "Enums.hpp"

namespace SynScanControl
{
    class Motor
    {
    public:
        // Note we only have 24 bits to store position given the Synscan protocol
        // Note that this position and Accelstepper positions may be different,
        // based on whether or not we are doing slow or fast microstepping
        static const uint32_t POSITION_INFINITE = 0xFFFFFF;
        static const uint32_t POSITION_NINFINITE = 0x000000;
        static const int32_t STEPPER_INFINITE = std::numeric_limits<int32_t>::max() / 2;
        static const int32_t STEPPER_NINFINITE = std::numeric_limits<int32_t>::min() / 2;

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