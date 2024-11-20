/*
 * Project Name: synscancontrol
 * File: Constants.hpp
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
 * Description: Constant and configurable parameters
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

namespace SynScanControl
{

    /* */
    constexpr float ARCSEC_TO_DEGREE = 2.77778E-4;

    /* */
    constexpr float SIDEREAL_SPEED_ARCSEC = 15.04108444;

    /* */
    constexpr uint32_t COMMAND_BUFFER_SIZE = 256;

    /* */
    constexpr uint32_t SERIAL_TIMEOUT_MS = 3000;

    /* */
    // DRV8825 - 1.9us minimum pulse width
    constexpr uint16_t STEPPER_PULSE_WIDTH_US = 4;

    // Motor params
    constexpr uint32_t SLOW_MICROSTEPS = 32;
    constexpr uint32_t FAST_MICROSTEPS = 8;
    constexpr uint32_t HIGH_SPEED_RATIO = SLOW_MICROSTEPS / FAST_MICROSTEPS;

    constexpr uint32_t FULL_STEPS_PER_REV = 141000;
    constexpr uint32_t MICROSTEPS_PER_REV = SLOW_MICROSTEPS * FULL_STEPS_PER_REV;
    constexpr uint32_t MAX_PULSE_PER_SECOND = 20000;
    constexpr float MOTOR_ACCEL = 5000.0;
    constexpr float SIDEREAL_STEP_PER_SECOND = (float)MICROSTEPS_PER_REV / 360.0 * ARCSEC_TO_DEGREE * SIDEREAL_SPEED_ARCSEC;
    constexpr float SIDEREAL_PULSE_PER_STEP = MAX_PULSE_PER_SECOND / SIDEREAL_STEP_PER_SECOND;

    constexpr uint8_t POLARSCOPE_INIT_BRIGHTNESS = 64;
}

#endif /* CONSTANTS_H */