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

    /* Arcsecond -> degree multiplier (i.e. 1/60/60) */
    constexpr float ARCSEC_TO_DEGREE = 2.77778E-4;

    /* Sidereal speed in arcseconds / second */
    constexpr float SIDEREAL_SPEED_ARCSEC = 15.04108444;

    /* Buffer size for serial commands
     * 256 is plenty for the Synscan serial interface,
     * but this can be increased to support longer commands
     * at the expense of available memory.
     */
    constexpr uint32_t COMMAND_BUFFER_SIZE = 256;

    /* Stepper driver pulse width in microseconds
     *
     * The minimum pulse width for the DRV8825 per
     * spec is 1.9us, so this must be larger than that,
     * ideally a few times larger.
     */
    constexpr uint16_t STEPPER_PULSE_WIDTH_US = 4;

    /* Halt the motors if we don't get any serial
     * commands in this amount of milliseconds.
     *
     * This requires the connected device to be at
     * least querying the status of the mount faster
     * than this interval.
     */
    constexpr uint32_t SERIAL_TIMEOUT_MS = 3000;

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

    // RA motor pinouts
    constexpr uint8_t RA_M0 = 12;
    constexpr uint8_t RA_M1 = 14;
    constexpr uint8_t RA_M2 = 27;
    constexpr uint8_t RA_STEP = 26;
    constexpr uint8_t RA_DIR = 25;

    // DEC motor pinouts
    constexpr uint8_t DEC_M0 = 33;
    constexpr uint8_t DEC_M1 = 32;
    constexpr uint8_t DEC_M2 = 5;
    constexpr uint8_t DEC_STEP = 18;
    constexpr uint8_t DEC_DIR = 19;

    // Autoguider pinouts
    constexpr uint8_t RA_POS_PIN = 36;
    constexpr uint8_t DEC_POS_PIN = 39;
    constexpr uint8_t RA_NEG_PIN = 34;
    constexpr uint8_t DEC_NEG_PIN = 35;

    // LED pinouts
    constexpr uint8_t PWR_LED = 4;
    constexpr uint8_t SCOPE_LED = 15;
    constexpr uint8_t BUILT_IN_LED = 2;

    // LED PWM channels
    constexpr uint8_t PWR_LED_PWM = 0;
    constexpr uint8_t SCOPE_LED_PWM = 1;
    constexpr uint8_t BUILT_IN_LED_PWM = 2;

    // Serial pinouts and UART numbers
    constexpr uint8_t SERIAL_LOGGER_UART = 0;
    constexpr uint8_t SERIAL_SYNSCAN_UART = 1;
    constexpr uint8_t SERIAL_SYNSCAN_RX = 16;
    constexpr uint8_t SERIAL_SYNSCAN_TX = 17;

#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
    const char ssid[] = "YOUR_SSID_HERE";
    const char password[] = "YOUR_PASSWORD_HERE";
#endif

#ifdef UDP_LOGGING
    constexpr uint16_t UDP_LOGGER_PORT = 6309;
#endif

}

#endif /* CONSTANTS_H */