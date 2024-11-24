/*
 * Project Name: synscancontrol
 * File: UDPLogger.hpp
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
 * Description: Provides optional UDP logging capability
 */
#ifndef UDP_LOGGER_H
#define UDP_LOGGER_H

#ifdef UDP_LOGGING

#include <Arduino.h>
#include <AsyncUDP.h>
#include <WiFi.h>

#include "Logger.hpp"

class UDPLoggerHandler : public LoggerHandler
{
public:
    UDPLoggerHandler(uint16_t udpPort, HardwareSerial *s = nullptr)
    {
        _udpPort = udpPort;
        /*if (_udp->connect(IPAddress(255, 255, 255, 255), _udpPort))
        {
            if (s != nullptr)
                s->println("UDP connected");
        }*/
    }
    void log(const char *msg) override
    {
        // _udp->broadcastTo(msg, _udpPort);
    }

private:
    uint16_t _udpPort = 0;
    AsyncUDP *_udp;
};

#endif

#endif /* UDP_LOGGER_H */