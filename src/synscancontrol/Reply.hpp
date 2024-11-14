#ifndef REPLY_H
#define REPLY_H

#include <Arduino.h>

#include "HexConversionUtils.hpp"

namespace SynScanControl
{
    class Reply
    {
    protected:
        Reply() {}

    public:
        virtual ~Reply() {}
        virtual void toStringStream(std::ostringstream *s);
        void send(HardwareSerial *s)
        {
            std::ostringstream out;
            toStringStream(&out);
            s->write(out.str().c_str());
        }
    };

    class EmptyReply : public Reply
    {
    public:
        void toStringStream(std::ostringstream *s) override
        {
            (*s) << "=\r";
        }
    };

    class PositionReply : public Reply
    {
    private:
        char _data[7];

    public:
        PositionReply()
        {
            memset(_data, 0, 7);
        }

        void setData(uint32_t data, uint32_t len)
        {
            toHexString<uint32_t>(data, _data, len);
        }

        void toStringStream(std::ostringstream *s) override
        {
            (*s) << '=' << _data << '\r';
        }
    };

    class DataReply : public Reply
    {
    private:
        char _data[7];

    public:
        DataReply()
        {
            memset(_data, 0, 7);
        }

        void setData(uint32_t data, uint32_t len)
        {
            toHexString<uint32_t>(data, _data, len);
        }

        void toStringStream(std::ostringstream *s) override
        {
            (*s) << '=' << _data << '\r';
        }
    };

    class VersionReply : public Reply
    {
    private:
        char _version[7];

    public:
        VersionReply()
        {
            memset(_version, 0, 7);
        }

        void setVersion(uint8_t major, uint8_t minor, uint8_t micro, uint8_t patch)
        {
            sprintf(_version, "%X%X%X%X00", major, minor, micro, patch);
        }

        void toStringStream(std::ostringstream *s) override
        {
            (*s) << '=' << _version << '\r';
        }
    };

    class ErrorReply : public Reply
    {
    private:
        ErrorEnum _errorCode;

    public:
        ErrorReply(ErrorEnum errorCode)
        {
            _errorCode = errorCode;
        }

        void setError(ErrorEnum errorCode)
        {
            _errorCode = errorCode;
        }

        void toStringStream(std::ostringstream *s) override
        {
            char ret[11];
            sprintf(ret, "!%X\r", (int)_errorCode);
            (*s) << ret;
        }
    };

    class StatusReply : public Reply
    {
    private:
        /*
        StatusReply byte breakdown:
         B0:0: 1=Tracking 0=Goto
         B0:1: 1=CCW 0=CW
         B0:2: 1=Fast 0=Slow
         B1:0: 1=Running 0=Stopped
         B1:1: 1=Blocked 0=Normal
         B2:0: 0 = Not Init 1 = Init done
         B2:1: 1 = Level switch on
         */
        uint8_t b0 = 0;
        uint8_t b1 = 0;
        uint8_t b2 = 0;

    public:
        StatusReply() {}

        void setSlewMode(SlewTypeEnum mode)
        {
            if (mode == SlewTypeEnum::GOTO)
            {
                b0 &= ~0x01;
            }
            else if (mode == SlewTypeEnum::TRACKING)
            {
                b0 |= 0x01;
            }
        }

        void setDirection(SlewDirectionEnum dir)
        {
            if (dir == SlewDirectionEnum::CW)
            {
                b0 &= ~0x02;
            }
            else if (dir == SlewDirectionEnum::CCW)
            {
                b0 |= 0x02;
            }
        }
        void setSpeedMode(SlewSpeedEnum speed)
        {
            if (speed == SlewSpeedEnum::FAST)
            {
                b0 |= 0x04;
            }
            else if (speed == SlewSpeedEnum::SLOW)
            {
                b0 &= ~0x04;
            }
        }

        void setRunning(bool running)
        {
            if (running)
            {
                b1 |= 0x01;
            }
            else
            {
                b1 &= ~0x01;
            }
        }

        void setBlocked(bool blocked)
        {
            if (blocked)
            {
                b1 |= 0x02;
            }
            else
            {
                b1 &= ~0x02;
            }
        }

        void setInitDone(bool init)
        {
            if (init)
            {
                b2 |= 0x01;
            }
            else
            {
                b2 &= ~0x01;
            }
        }

        void toStringStream(std::ostringstream *s) override
        {
            char ret[11];
            sprintf(ret, "=%X%X%X\r", b0, b1, b2);
            (*s) << ret;
        }
    };

    class ExtendedStatusReply : public Reply
    {
    private:
#define PEC_TRAINING 1
#define PEC_TRACKING 2
#define SUPPORT_DUAL_ENC 1
#define SUPPORT_PPEC 2
#define SUPPORT_ORIGINAL_POSITION_IDX 4
#define SUPPORT_EQAZ_MODE 8
#define HAS_POLAR_LED 1
#define TWO_AXES_SEPARATE 2
#define SUPPORT_TORQUE 3

        uint8_t b0 = 0;
        uint8_t b1 = 0;
        uint8_t b2 = 0;

    public:
        ExtendedStatusReply() {}

        void setPecTraining(bool enabled)
        {
            if (enabled)
            {
                b0 |= PEC_TRAINING;
            }
            else
            {
                b0 &= ~PEC_TRAINING;
            }
        }

        void setPecTracking(bool enabled)
        {
            if (enabled)
            {
                b0 |= PEC_TRACKING;
            }
            else
            {
                b0 &= ~PEC_TRACKING;
            }
        }

        void setDualEncSupport(bool enabled)
        {
            if (enabled)
            {
                b1 |= SUPPORT_DUAL_ENC;
            }
            else
            {
                b1 &= ~SUPPORT_DUAL_ENC;
            }
        }

        void setPPECSupport(bool enabled)
        {
            if (enabled)
            {
                b1 |= SUPPORT_PPEC;
            }
            else
            {
                b1 &= ~SUPPORT_PPEC;
            }
        }

        void setOriginalIdxPosSupport(bool enabled)
        {
            if (enabled)
            {
                b1 |= SUPPORT_ORIGINAL_POSITION_IDX;
            }
            else
            {
                b1 &= ~SUPPORT_ORIGINAL_POSITION_IDX;
            }
        }

        void setEQAZModeSupport(bool enabled)
        {
            if (enabled)
            {
                b1 |= SUPPORT_EQAZ_MODE;
            }
            else
            {
                b1 &= ~SUPPORT_EQAZ_MODE;
            }
        }

        void setHasPolarLed(bool enabled)
        {
            if (enabled)
            {
                b2 |= HAS_POLAR_LED;
            }
            else
            {
                b2 &= ~HAS_POLAR_LED;
            }
        }

        void setTwoAxesSeparate(bool enabled)
        {
            if (enabled)
            {
                b2 |= TWO_AXES_SEPARATE;
            }
            else
            {
                b2 &= ~TWO_AXES_SEPARATE;
            }
        }

        void setTorqueSelectionSupport(bool enabled)
        {
            if (enabled)
            {
                b2 |= SUPPORT_TORQUE;
            }
            else
            {
                b2 &= ~SUPPORT_TORQUE;
            }
        }

        void toStringStream(std::ostringstream *s) override
        {
            char ret[11];
            sprintf(ret, "=%X%X%X000\r", b0, b1, b2);
            (*s) << ret;
        }
    };
} // namespace SynScanControl

#endif /* REPLY_H */