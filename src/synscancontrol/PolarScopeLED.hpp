/*
 * Project Name: synscancontrol
 * File: PolarScopeLED.hpp
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
 * Description: Control logic for the polar scope LED light :)
 */
#ifndef POLARSCOPELED_H
#define POLARSCOPELED_H

#include <stdint.h>
#include <Logger.hpp>

namespace SynScanControl
{
    class PolarScopeLED
    {
    public:
        PolarScopeLED(uint8_t PIN, uint8_t PWM_CHANNEL, Logger *logger)
        {
            _pin = PIN;
            _pwm_channel = PWM_CHANNEL;
            _logger = logger;
        };

        void begin()
        {
            ledcAttachPin(_pin, _pwm_channel);
            ledcSetup(_pwm_channel, 5000, 8);
            setBrightness(_brightness);
        }

        void setBrightness(uint8_t pwm)
        {
            // Log
            std::ostringstream log;
            log << "Setting polar scope LED PWM to: 0x" << std::hex << int(pwm);
            _logger->debug(&log);

            ledcWrite(_pwm_channel, pwm);
        }

    private:
        uint8_t _pin;
        uint8_t _pwm_channel;
        Logger *_logger;
        uint8_t _brightness = 64;
    };
} // namespace SynScanControl

#endif /* POLARSCOPELED_H */