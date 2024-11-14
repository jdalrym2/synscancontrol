#include <Arduino.h>

#ifdef OTA_UPDATES
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

#ifdef UDP_LOGGING
#include <WiFi.h>
#include <AsyncUDP.h>
#endif

#include "Logger.hpp"
#include "Enums.hpp"
#include "Motor.hpp"
#include "PolarScopeLED.hpp"
#include "CommandHandler.hpp"

#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
const char *ssid = "YOUR_SSID_HERE";
const char *password = "YOUR_PASSWORD_HERE";
bool wifiConnected = true;
#endif

#ifdef UDP_LOGGING
AsyncUDP udp;
#endif

// RA motor pinouts
#define RA_M0 12
#define RA_M1 14
#define RA_M2 27
#define RA_STEP 26
#define RA_DIR 25

// DEC motor pinouts
#define DEC_M0 33
#define DEC_M1 32
#define DEC_M2 5
#define DEC_STEP 18
#define DEC_DIR 19

// Autoguider pinouts
#define RA_POS_PIN 36
#define DEC_POS_PIN 39
#define RA_NEG_PIN 34
#define DEC_NEG_PIN 35

// LED pinouts
#define PWR_LED 4
#define SCOPE_LED 15
#define BUILT_IN_LED 2

// LED PWM channels
#define PWR_LED_PWM 0
#define SCOPE_LED_PWM 1
#define BUILT_IN_LED_PWM 2

// Serial pinouts
#define SERIAL2_RX 16
#define SERIAL2_TX 17

using namespace SynScanControl;

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
CommandHandler cmdHandler(&Serial2, &raMotor, &decMotor, &polarScopeLED, &logger);

// Motor fast tick (hardware interrupt)
void tick()
{
#ifndef UDP_LOGGING
    decMotor.tick();
    raMotor.tick();
#endif
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
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, SERIAL2_RX, SERIAL2_TX);

#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
    // Connect to WiFi
    // TODO: this doesn't seem to handle not finding an access point
    // correctly. However, if it temporarly connects to an access point
    // and then it is lost, I've found that the code behaves correctly.
    //
    // TL;DR For now, disable the WiFi features for use outside of your
    // WiFi network.
    WiFi.mode(WIFI_STA);
    Serial.println("Starting WiFi...");
    WiFi.begin(ssid, password);
    wifiConnected = (WiFi.waitForConnectResult() == WL_CONNECTED);
    if (!wifiConnected)
    {
        Serial.println("Connection Failed!");
    }
    else
    {
        Serial.println("Ready");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
#endif

#ifdef UDP_LOGGING
    logger.addUDPHandler(&udp, 6309);
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
    logger.addHardwareSerialHandler(&Serial);

#ifdef OTA_UPDATES
    ArduinoOTA
        .onStart([]()
                 {
                     String type;
                     if (ArduinoOTA.getCommand() == U_FLASH)
                         type = "sketch";
                     else // U_SPIFFS
                         type = "filesystem";

                     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                     Serial.println("Start updating " + type);

                     // Disable hardware timer
                     timerAlarmDisable(tickTimer);
                     timerDetachInterrupt(tickTimer); })
        .onEnd([]()
               { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
                     Serial.printf("Error[%u]: ", error);
                     if (error == OTA_AUTH_ERROR)
                         Serial.println("Auth Failed");
                     else if (error == OTA_BEGIN_ERROR)
                         Serial.println("Begin Failed");
                     else if (error == OTA_CONNECT_ERROR)
                         Serial.println("Connect Failed");
                     else if (error == OTA_RECEIVE_ERROR)
                         Serial.println("Receive Failed");
                     else if (error == OTA_END_ERROR)
                         Serial.println("End Failed"); });

    if (wifiConnected)
        ArduinoOTA.begin();

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
        ArduinoOTA.handle();
#endif
}