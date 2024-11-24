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
// allows us to rename Serial / Serial2 to more meaningful names
#define NO_GLOBAL_SERIAL

// Make a USE_WIFI define for convenience
#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
#define USE_WIFI
#endif

#include <Arduino.h>

#ifdef USE_WIFI
#include <WiFi.h>
#endif

#include "Constants.hpp"
#include "Logger.hpp"
#include "Enums.hpp"
#include "Motor.hpp"
#include "OTAUpdate.hpp"
#include "PolarScopeLED.hpp"
#include "StatusLED.hpp"
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
#ifdef UDP_LOGGING
UDPLoggerHandler *udpHandler = nullptr;
#endif

// Motors
Motor raMotor(AxisEnum::AXIS_RA, RA_M0, RA_M1, RA_M2, RA_STEP, RA_DIR, 0x800000, false, &logger);
Motor decMotor(AxisEnum::AXIS_DEC, DEC_M0, DEC_M1, DEC_M2, DEC_STEP, DEC_DIR, 0x913640, true, &logger);

// Power / Status LED
StatusLED statusLED(PWR_LED, PWR_LED_PWM, &logger);

// Polar Scope LED
PolarScopeLED polarScopeLED(SCOPE_LED, SCOPE_LED_PWM, &logger);

// Serial Command handler
CommandHandler cmdHandler(&SerialSynScan, &raMotor, &decMotor, &polarScopeLED, &logger);

// Motor fast tick (hardware interrupt)
void IRAM_ATTR tick()
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
    pinMode(PWR_LED, OUTPUT);
    pinMode(SCOPE_LED, OUTPUT);
    pinMode(BUILT_IN_LED, OUTPUT);
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

    // Setup LED pins
    polarScopeLED.begin();
    statusLED.begin();

    // Built-in LED unused atm
    ledcSetup(BUILT_IN_LED_PWM, 5000, 8);
    ledcAttachPin(BUILT_IN_LED, BUILT_IN_LED_PWM);

    // Setup motor tick timers
    tickTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(tickTimer, &tick, true);
    timerAlarmWrite(tickTimer, 50, true);
    timerAlarmEnable(tickTimer);

    // Setup motors
    decMotor.begin();
    raMotor.begin();

    // Setup slow non-interrupt timer
    longTickTimer = millis();

#ifdef USE_WIFI
    // Async WiFi setup (we don't wait for it to connect)
    SerialLogger.println("Starting WiFi...");
    statusLED.setBlinkStatus(StatusLED::BlinkStatus::BLINK_SLOW);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

    // Configure logger
#ifdef SERIAL_DEBUG
    logger.addHandler(new HardwareSerialLoggerHandler(&SerialLogger));
#endif

#ifdef UDP_LOGGING
    udpHandler = new UDPLoggerHandler(UDP_LOGGER_PORT, &SerialLogger);
    logger.addHandler(udpHandler);
#endif

#ifdef OTA_UPDATES
    setupOTA(tickTimer, &SerialLogger);
#endif

    logger.debug("Logging started!");
}

void loop()
{
    // Process serial port
    cmdHandler.processSerial();

// Check WiFi status
#ifdef USE_WIFI
    if (!wifiConnected && WiFi.status() == WL_CONNECTED)
    {
        SerialLogger.println("WiFi connected: ");
        SerialLogger.println(WiFi.localIP().toString());
        wifiConnected = true;
#ifdef UDP_LOGGING
        udpHandler->connect();
#endif
#ifdef OTA_UPDATES
        beginOTA();
#endif
        statusLED.setBlinkStatus(StatusLED::BlinkStatus::BLINK_FAST);
    }
    else if (wifiConnected && WiFi.status() != WL_CONNECTED)
    {
        SerialLogger.println("WiFi disconnected");
        wifiConnected = false;
#ifdef UDP_LOGGING
        udpHandler->disconnect();
#endif
#ifdef OTA_UPDATES
        endOTA();
#endif
        statusLED.setBlinkStatus(StatusLED::BlinkStatus::BLINK_SLOW);
    }
#endif

    // Process slow (long tick) non-interrupt timer
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