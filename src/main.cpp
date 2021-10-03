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
#include "CommandHandler.hpp"

#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
const char *ssid = "CenturyLink3000";
const char *password = "cy3z46ex7fyhmk";
#endif

#ifdef UDP_LOGGING
AsyncUDP udp;
#endif

#define RA_M0 12
#define RA_M1 14
#define RA_M2 27
#define RA_STEP 25
#define RA_DIR 26

#define SERIAL2_RX 16
#define SERIAL2_TX 17

Logger logger;

Motor raMotor(AxisEnum::AXIS_RA, RA_M0, RA_M1, RA_M2, RA_STEP, RA_DIR, &logger);
//Motor decMotor(AxisEnum::AXIS_DEC);
CommandHandler cmdHandler(&Serial2, &raMotor, &raMotor); // TODO: change second motor to DEC

unsigned long longTickTimer;

void setup()
{
    setCpuFrequencyMhz(240);
    pinMode(RA_M0, OUTPUT);
    pinMode(RA_M1, OUTPUT);
    pinMode(RA_M2, OUTPUT);
    pinMode(RA_STEP, OUTPUT);
    pinMode(RA_DIR, OUTPUT);

    raMotor.begin();

    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, SERIAL2_RX, SERIAL2_TX);
    longTickTimer = millis();

    logger.addHardwareSerialHandler(&Serial);

#if defined(OTA_UPDATES) || defined(UDP_LOGGING)
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif

#ifdef UDP_LOGGING
    logger.addUDPHandler(&udp, 6309);
#endif

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
                 })
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
                         Serial.println("End Failed");
                 });

    ArduinoOTA.begin();

#endif
}

void loop()
{
    cmdHandler.processSerial();
    raMotor.tick();

    if (millis() - longTickTimer > 100)
    {
        longTickTimer = millis();
        raMotor.longTick();
    }

#ifdef OTA_UPDATES
    ArduinoOTA.handle();
#endif
}