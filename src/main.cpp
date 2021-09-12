#include <Arduino.h>

#include "Enums.hpp"
#include "Motor.hpp"
#include "CommandHandler.hpp"

#define RA_M0 12
#define RA_M1 14
#define RA_M2 27
#define RA_STEP 25
#define RA_DIR 26

#define SERIAL1_RX 16
#define SERIAL1_TX 17

Motor raMotor(AxisEnum::AXIS_RA, RA_M0, RA_M1, RA_M2, RA_STEP, RA_DIR);
//Motor decMotor(AxisEnum::AXIS_DEC);
CommandHandler cmdHandler(&Serial2, &raMotor, &raMotor); // TODO: change second motor to DEC

unsigned long longTickTimer;

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);
    longTickTimer = millis();
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
}