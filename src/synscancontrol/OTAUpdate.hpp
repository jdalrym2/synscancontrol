/*
 * Project Name: synscancontrol
 * File: OTAUpdate.hpp
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
 * Description: Provides optional OTA firmware flashing capability
 */

#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#ifdef OTA_UPDATES

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

namespace SynScanControl
{
    namespace OTA
    {
        void onStart(hw_timer_t *tickTimer, HardwareSerial *s)
        {
            s->println("Begin OTA command" + (ArduinoOTA.getCommand() == U_FLASH) ? "U_FLASH" : "U_SPIFFS");

            // Disable hardware timer
            timerAlarmDisable(tickTimer);
            timerDetachInterrupt(tickTimer);
        }

        void onEnd(HardwareSerial *s)
        {
            s->println("\nEnd");
        }

        void onProgress(uint32_t progress, uint32_t total, HardwareSerial *s)
        {
            s->printf("Progress: %u%%\r", (progress / (total / 100)));
        }

        void onError(ota_error_t error, HardwareSerial *s)
        {
            s->printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
            {
                s->println("Auth Failed");
            }
            else if (error == OTA_BEGIN_ERROR)
            {
                s->println("Begin Failed");
            }
            else if (error == OTA_CONNECT_ERROR)
            {
                s->println("Connect Failed");
            }
            else if (error == OTA_RECEIVE_ERROR)
            {
                s->println("Receive Failed");
            }
            else if (error == OTA_END_ERROR)
            {
                s->println("End Failed");
            }
        }
    }

    void setupOTA(hw_timer_t *tickTimer, HardwareSerial *s)
    {
        ArduinoOTA.onStart([tickTimer, s]()
                           { OTA::onStart(tickTimer, s); })
            .onEnd([s]
                   { OTA::onEnd(s); })
            .onProgress([s](uint32_t progress, uint32_t total)
                        { OTA::onProgress(progress, total, s); })
            .onError([s](ota_error_t error)
                     { OTA::onError(error, s); });
    }

    void beginOTA()
    {
        ArduinoOTA.begin();
    }

    void handleOTA()
    {
        ArduinoOTA.handle();
    }
}

#endif

#endif /* OTA_UPDATE_H */