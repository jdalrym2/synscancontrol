/*
 * Project Name: synscancontrol
 * File: InterruptStepper.cpp
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
 *
 * Re-uses code from the AccelStepper project, licensed via GPLv3.
 * For more info see README.md.
 */
#include <Arduino.h>
#include "InterruptStepper.hpp"

using namespace SynScanControl;

InterruptStepper::InterruptStepper(uint8_t STEP, uint8_t DIR, uint32_t FREQ, bool DIR_REVERSE)
{
    _STEP = STEP;
    _DIR = DIR;
    _FREQ = FREQ;
    _DIR_REVERSE = DIR_REVERSE;
}

int32_t InterruptStepper::getPosition()
{
    return _pos;
}

int32_t InterruptStepper::getTargetPosition()
{
    return _targetPos;
}

float InterruptStepper::getSpeed()
{
    return _speed;
}

uint32_t InterruptStepper::getPulsesPerStep()
{
    return _pulsesPerStep;
}

int32_t InterruptStepper::stepsToStop()
{
    return _stepsToStop;
}

// Inspired from AccelStepper::setCurrentPosition
void InterruptStepper::initPosition(int32_t position)
{
    _targetPos = _pos = position;
    _n = 0;
    _stepInterval = 0;
    _speed = 0.0;
}

void InterruptStepper::setPosition(int32_t position)
{
    _pos = position;
}

// Inspired from AccelStepper:moveTo
void InterruptStepper::setTargetPosition(int32_t targetPos)
{
    if (_targetPos != targetPos)
    {
        _targetPos = targetPos;
        computeNewSpeed();
        // compute new n?
    }
}

// Inspired from AccelStepper::setMaxSpeed
void InterruptStepper::setMaxSpeed(float speed)
{
    if (speed < 0.0)
        speed = -speed;
    if (_maxSpeed != speed)
    {
        _maxSpeed = speed;
        _cmin = 1000000.0 / speed;
        // Recompute _n from current speed and adjust speed if accelerating or cruising
        if (_n > 0)
        {
            _n = (long)((_speed * _speed) / (2.0 * _accel)); // Equation 16
            computeNewSpeed();
        }
    }
}

// Inspired from AccelStepper::setAcceleration
void InterruptStepper::setAcceleration(float accel)
{
    if (accel == 0.0)
        return;
    if (accel < 0.0)
        accel = -accel;
    if (_accel != accel)
    {
        // Recompute _n per Equation 17
        _n = _n * (_accel / accel);
        // New c0 per Equation 7, with correction per Equation 15
        _c0 = 0.676 * sqrt(2.0 / accel) * 1000000.0; // Equation 15
        _accel = accel;
        computeNewSpeed();
    }
}

// Inspired from AccelStepper::runSpeed
void InterruptStepper::run()
{
    if (_dir == SlewDirectionEnum::CW)
    {
        // Clockwise
        _pos += 1;
    }
    else
    {
        // Anticlockwise
        _pos -= 1;
    }
    step();
}

// Inspired from AccelStepper:step1 (for stepper drivers)
// But assumes direction gets set earlier
void InterruptStepper::step()
{
    GPIO.out_w1ts = ((uint32_t)1 << _STEP); // digitalWrite(_STEP, 1)
    delayMicroseconds(STEPPER_PULSE_WIDTH_US);
    GPIO.out_w1tc = ((uint32_t)1 << _STEP); // digitalWrite(_STEP, 0)
}

// Inspired from AccelStepper:distanceToGo()
// supports infinite distances via the
// STEPPER_[N]INFINITE static members
int32_t InterruptStepper::distanceToGo()
{
    if (_targetPos >= STEPPER_INFINITE)
        return STEPPER_INFINITE;
    if (_targetPos <= STEPPER_NINFINITE)
        return STEPPER_NINFINITE;
    return _targetPos - _pos;
};

// Inspired from AccelStepper::computeNewSpeed
void InterruptStepper::computeNewSpeed()
{
    int32_t distanceTo = distanceToGo();
    _stepsToStop = (int32_t)((_speed * _speed) / (2.0 * _accel)); // Equation 16

    if (distanceTo == 0 && _stepsToStop <= 1)
    {
        _speed = 0.0;
        _pulsesPerStep = 0;
        _n = 0;
        return;
    }

    if (distanceTo > 0)
    {
        // We are anticlockwise from the target
        // Need to go clockwise from here, maybe decelerate now
        if (_n > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if ((_stepsToStop >= distanceTo) || _dir == SlewDirectionEnum::CCW)
                _n = -_stepsToStop; // Start deceleration
        }
        else if (_n < 0)
        {
            // Currently decelerating, need to accel again?
            if ((_stepsToStop < distanceTo) && _dir == SlewDirectionEnum::CW)
                _n = -_n; // Start accceleration
        }
    }
    else if (distanceTo < 0)
    {
        // We are clockwise from the target
        // Need to go anticlockwise from here, maybe decelerate
        if (_n > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if ((_stepsToStop >= -distanceTo) || _dir == SlewDirectionEnum::CW)
                _n = -_stepsToStop; // Start deceleration
        }
        else if (_n < 0)
        {
            // Currently decelerating, need to accel again?
            if ((_stepsToStop < -distanceTo) && _dir == SlewDirectionEnum::CCW)
                _n = -_n; // Start accceleration
        }
    }

    // Need to accelerate or decelerate
    if (_n == 0)
    {
        // First step from stopped
        _cn = _c0;
        _dir = (distanceTo > 0) ? SlewDirectionEnum::CW : SlewDirectionEnum::CCW;
        _setDirectionPin();
    }
    else
    {
        // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
        _cn = _cn - ((2.0 * _cn) / ((4.0 * _n) + 1)); // Equation 13
        _cn = max(_cn, _cmin);                        // _cmin dictated by max speed
    }
    _n++;
    _stepInterval = _cn;
    _pulsesPerStep = _cn * (float)_FREQ / 1000000.0;
    _speed = 1000000.0 / _cn;
    if (_dir == SlewDirectionEnum::CCW)
        _speed = -_speed;
}

bool InterruptStepper::isRunning()
{
    return !(_speed == 0.0 && _targetPos == _pos);
}

void InterruptStepper::_setDirectionPin()
{
    if ((_dir == SlewDirectionEnum::CW) != _DIR_REVERSE)
    {
        GPIO.out_w1ts = ((uint32_t)1 << _DIR); // digitalWrite(_DIR, 1)
    }
    else
    {
        GPIO.out_w1tc = ((uint32_t)1 << _DIR); // digitalWrite(_DIR, 0)
    }
}