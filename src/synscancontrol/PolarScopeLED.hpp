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