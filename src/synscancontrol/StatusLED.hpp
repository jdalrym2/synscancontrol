/*
 * Project Name: synscancontrol
 * File: StatusLED.hpp
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
 * Description: Control logic for power LED light, to show some status
 */
#ifndef STATUSLED_H
#define STATUSLED_H

#include <stdint.h>

#include <Arduino.h>

#include "Constants.hpp"
#include "Logger.hpp"

namespace SynScanControl
{
    class StatusLED
    {
    public:
        /* We'll do something clever and use this enum as a LUT for how to blink the LED.
         * We'll cycle through 8 (status), where the LED on/off state is determined by the
         * corresponding bit.
         */
        enum class BlinkStatus
        {
            BLINK_SLOW = (uint8_t)0b11110000,
            BLINK_FAST = (uint8_t)0b11111110,
            NONE = (uint8_t)0b11111111
        };

    public:
        StatusLED(uint8_t PIN, uint8_t PWM_CHANNEL, Logger *logger)
        {
            _pin = PIN;
            _pwm_channel = PWM_CHANNEL;
            _logger = logger;
            _counter = 0;
            _lut = (uint8_t)BlinkStatus::NONE;

            _timerConfig.arg = this;
            _timerConfig.callback = reinterpret_cast<esp_timer_cb_t>(tick);
            _timerConfig.dispatch_method = ESP_TIMER_TASK;
            _timerConfig.name = "StatusLED::tick";
        };

        inline uint8_t getPWMChannel() { return _pwm_channel; }
        inline uint8_t getLUT() { return _lut; }
        inline void setBlinkStatus(BlinkStatus status) { _lut = (uint8_t)status; }
        inline uint8_t incrementCounter()
        {
            _counter = (_counter + 1) % 8;
            return _counter;
        };

        void begin()
        {
            ledcSetup(_pwm_channel, 5000, 8);
            ledcAttachPin(_pin, _pwm_channel);
            esp_timer_create(&_timerConfig, &_timer);
            esp_timer_start_periodic(_timer, 200000);
        }

        static void tick(void *arg)
        {
            StatusLED *obj = (StatusLED *)arg;
            ledcWrite(obj->getPWMChannel(), 255 * ((obj->getLUT() >> obj->incrementCounter()) & 1));
        }

    private:
        uint8_t _pin;
        uint8_t _pwm_channel;
        Logger *_logger;
        volatile uint8_t _counter;
        uint8_t _lut;
        esp_timer_handle_t _timer;
        esp_timer_create_args_t _timerConfig;
    };
} // namespace SynScanControl

#endif /* STATUSLED_H */