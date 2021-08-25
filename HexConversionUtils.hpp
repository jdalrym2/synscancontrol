#ifndef HEX_CONVERSION_UTILS_H_
#define HEX_CONVERSION_UTILS_H_

#include <stdint.h>

namespace HexConversionUtils
{
    const uint32_t charToHex(char c)
    {
        switch (c)
        {
        case '0':
            return 0x00;
        case '1':
            return 0x01;
        case '2':
            return 0x02;
        case '3':
            return 0x03;
        case '4':
            return 0x04;
        case '5':
            return 0x05;
        case '6':
            return 0x06;
        case '7':
            return 0x07;
        case '8':
            return 0x08;
        case '9':
            return 0x09;
        case 'A':
            return 0x0A;
        case 'B':
            return 0x0B;
        case 'C':
            return 0x0C;
        case 'D':
            return 0x0D;
        case 'E':
            return 0x0E;
        case 'F':
            return 0x0F;
        default:
            // This shouldn't happen
            return 0x00;
        }
    }

    template <class T>
    const T parseToHex(const char *data, uint32_t len)
    {
        T r = 0;
        if (len == 6)
        {
            r |= ((T)charToHex(data[1]));
            r |= ((T)charToHex(data[0]) << 4);
            r |= ((T)charToHex(data[3]) << 8);
            r |= ((T)charToHex(data[2]) << 12);
            r |= ((T)charToHex(data[5]) << 16);
            r |= ((T)charToHex(data[4]) << 20);
        }
        else if (len == 4)
        {
            r |= ((T)charToHex(data[1]));
            r |= ((T)charToHex(data[0]) << 4);
            r |= ((T)charToHex(data[3]) << 8);
            r |= ((T)charToHex(data[2]) << 12);
        }
        else if (len == 2)
        {
            r |= ((T)charToHex(data[1]));
            r |= ((T)charToHex(data[0]) << 4);
        }
        else if (len == 1)
        {
            r |= ((T)charToHex(data[0]));
        }
        return r;
    }

    template <class T>
    void toHexString(T data, char *output, uint32_t len)
    {
        uint8_t b6, b5, b4, b3, b2, b1;
        b6 = (data & 0xF00000) >> 20;
        b5 = (data & 0xF0000) >> 16;
        b4 = (data & 0xF000) >> 12;
        b3 = (data & 0xF00) >> 8;
        b2 = (data & 0xF0) >> 4;
        b1 = (data & 0xF);

        if (len == 6)
        {
            sprintf(output, "%X%X%X%X%X%X", b2, b1, b4, b3, b6, b5);
        }
        else if (len == 4)
        {
            sprintf(output, "%X%X%X%X", b2, b1, b4, b3);
        }
        else if (len == 2)
        {
            sprintf(output, "%X%X", b2, b1);
        }
    }
}

#endif /*  HEX_CONVERSION_UTILS_H_ */