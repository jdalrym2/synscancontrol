#include <Arduino.h>
#include <AltSoftSerial.h>

#include "Enums.hpp"
#include "Motor.hpp"

#include "CommandHandler.hpp"

#define RA_M0 13
#define RA_M1 12
#define RA_M2 11
#define RA_STEP 3
#define RA_DIR 4

AltSoftSerial Serial1;
Motor raMotor(AxisEnum::AXIS_RA, RA_M0, RA_M1, RA_M2, RA_STEP, RA_DIR);
//Motor decMotor(AxisEnum::AXIS_DEC);
CommandHandler cmdHandler(&Serial1, (HardwareSerial *)&Serial, &raMotor, &raMotor); // TODO: change second motor to DEC

unsigned long longTickTimer;

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
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