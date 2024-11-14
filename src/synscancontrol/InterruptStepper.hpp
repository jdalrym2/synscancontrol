/*
 * Project Name: synscancontrol
 * File: InterruptStepper.hpp
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
 * Description: Manages low-level stepper motor hardware logic
 */
#ifndef INTERRUPT_STEPPER_H
#define INTERRUPT_STEPPER_H

#include <stdint.h>
#include <limits.h>
#include "Enums.hpp"

namespace SynScanControl
{
    class InterruptStepper
    {
    public:
        InterruptStepper() {};
        InterruptStepper(uint8_t STEP, uint8_t DIR, uint32_t FREQ, bool DIR_REVERSE);

        int32_t getPosition();
        int32_t getTargetPosition();
        float getSpeed();
        uint32_t getPulsesPerStep();
        int32_t stepsToStop();

        void initPosition(int32_t position);
        void setPosition(int32_t position);
        void setTargetPosition(int32_t targetPos);
        void setMaxSpeed(float speed);
        void setAcceleration(float accel);
        void run();
        void step();
        int32_t distanceToGo();
        void computeNewSpeed();
        bool isRunning();

        void moveToInfinity() { setTargetPosition(STEPPER_INFINITE); };
        void moveToNInfinity() { setTargetPosition(STEPPER_NINFINITE); };

    private:
        void _setDirectionPin();

    public:
        static const int32_t STEPPER_INFINITE = std::numeric_limits<int32_t>::max() / 2;
        static const int32_t STEPPER_NINFINITE = std::numeric_limits<int32_t>::min() / 2;

    private:
        uint8_t _STEP;
        uint8_t _DIR;
        uint32_t _FREQ;
        bool _DIR_REVERSE = false;

        float _accel = 0.0;
        volatile float _speed = 0.0;
        float _maxSpeed = 0.0;
        volatile int32_t _pos = 0;
        int32_t _targetPos = 0;
        volatile int32_t _n = 0;
        volatile float _c0 = 0.0;
        volatile float _cn = 0.0;
        volatile float _cmin = 1.0;
        volatile uint32_t _stepInterval = 0;
        volatile uint32_t _pulsesPerStep = 0;
        volatile int32_t _stepsToStop = 0;
        volatile SlewDirectionEnum _dir = SlewDirectionEnum::CW;
    };
} // namespace SynScanControl

#endif /* INTERRUPT_STEPPER_H */