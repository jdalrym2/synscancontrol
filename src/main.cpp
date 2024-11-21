/*
 * Project Name: synscancontrol
 * File: main.cpp
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
 * Description: synscancontrol firmware entrypoint
 */
#define NO_GLOBAL_SERIAL // allows us to rename Serial / Serial2 to more meaningful names

#include <WiFi.h>
#include <Arduino.h>

#include "Constants.hpp"
#include "Logger.hpp"
#include "Enums.hpp"
#include "Motor.hpp"
#include "OTAUpdate.hpp"
#include "PolarScopeLED.hpp"
#include "CommandHandler.hpp"
#include "UDPLogger.hpp"

using namespace SynScanControl;

// Serial ports
HardwareSerial SerialLogger(SERIAL_LOGGER_UART);
HardwareSerial SerialSynScan(SERIAL_SYNSCAN_UART);

// State variables
bool wifiConnected = false;

// Hardware timers
hw_timer_t *tickTimer = nullptr;

// Software timers
unsigned long longTickTimer = 0;

// Logger
Logger logger;

// Motors
Motor raMotor(AxisEnum::AXIS_RA, RA_M0, RA_M1, RA_M2, RA_STEP, RA_DIR, 0x800000, false, &logger);
Motor decMotor(AxisEnum::AXIS_DEC, DEC_M0, DEC_M1, DEC_M2, DEC_STEP, DEC_DIR, 0x913640, true, &logger);

// Polar Scope LED
PolarScopeLED polarScopeLED(SCOPE_LED, SCOPE_LED_PWM, &logger);

// Serial Command handler
CommandHandler cmdHandler(&SerialSynScan, &raMotor, &decMotor, &polarScopeLED, &logger);

// Motor fast tick (hardware interrupt)
void tick()
{
    decMotor.tick();
    raMotor.tick();
}

// Motor slow tick (loop)
void longTick()
{
    decMotor.longTick();
    raMotor.longTick();
}

void setup()
{
    // Set CPU frequency
    setCpuFrequencyMhz(240);

    // Set output pins
    pinMode(RA_M0, OUTPUT);
    pinMode(RA_M1, OUTPUT);
    pinMode(RA_M2, OUTPUT);
    pinMode(RA_STEP, OUTPUT);
    pinMode(RA_DIR, OUTPUT);
    pinMode(DEC_M0, OUTPUT);
    pinMode(DEC_M1, OUTPUT);
    pinMode(DEC_M2, OUTPUT);
    pinMode(DEC_STEP, OUTPUT);
    pinMode(DEC_DIR, OUTPUT);
    pinMode(RA_POS_PIN, INPUT);
    pinMode(DEC_POS_PIN, INPUT);
    pinMode(RA_NEG_PIN, INPUT);
    pinMode(DEC_NEG_PIN, INPUT);

    // Setup serial ports
    SerialLogger.begin(115200);
    SerialSynScan.begin(9600, SERIAL_8N1, SERIAL_SYNSCAN_RX, SERIAL_SYNSCAN_TX);

#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
    // Connect to WiFi
    // TODO: this doesn't seem to handle not finding an access point
    // correctly. However, if it temporarly connects to an access point
    // and then it is lost, I've found that the code behaves correctly.
    //
    // TL;DR For now, disable the WiFi features for use outside of your
    // WiFi network.
    WiFi.mode(WIFI_STA);
    SerialLogger.println("Starting WiFi...");
    WiFi.begin(ssid, password);
    wifiConnected = (WiFi.waitForConnectResult() == WL_CONNECTED);
    if (!wifiConnected)
    {
        SerialLogger.println("Connection Failed!");
    }
    else
    {
        SerialLogger.println("Ready");
        SerialLogger.print("IP address: ");
        SerialLogger.println(WiFi.localIP());
    }
#endif

    // Setup LED pins
    polarScopeLED.begin(); // handles polar scope LED
    ledcAttachPin(PWR_LED, PWR_LED_PWM);
    ledcSetup(PWR_LED_PWM, 5000, 8);
    ledcAttachPin(BUILT_IN_LED, BUILT_IN_LED_PWM);
    ledcSetup(BUILT_IN_LED_PWM, 5000, 8);

    // Static brightness for now
    // We can do way cooler things to show status, etc, later
    ledcWrite(PWR_LED_PWM, 255);

    // Setup motor tick timers
    tickTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(tickTimer, &tick, true);
    timerAlarmWrite(tickTimer, 50, true);
    timerAlarmEnable(tickTimer);

    // Setup motors
    decMotor.begin();
    raMotor.begin();

    // Setup slow timer
    longTickTimer = millis();

    // Configure logger
    logger.addHandler(new HardwareSerialLoggerHandler(&SerialLogger));

#ifdef UDP_LOGGING
    logger.addHandler(new UDPLoggerHandler(UDP_LOGGER_PORT, &SerialLogger));
#endif

#ifdef OTA_UPDATES
    // Setup OTA
    setupOTA(tickTimer, &SerialLogger);
    if (wifiConnected)
        beginOTA();
#endif

    logger.debug("Logging started!");
}

void loop()
{
    // Process serial port
    cmdHandler.processSerial();

    // Process slow (long tick) timer
    if (millis() - longTickTimer > 100)
    {
        longTickTimer = millis();
        longTick();
    }

#ifdef OTA_UPDATES
    if (wifiConnected)
        handleOTA();
#endif
}