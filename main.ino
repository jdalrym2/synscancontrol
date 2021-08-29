#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Enums.hpp"
#include "Motor.hpp"

#include "CommandHandler.hpp"

SoftwareSerial Serial1(2, 3);
Motor raMotor(AxisEnum::AXIS_RA);
Motor decMotor(AxisEnum::AXIS_DEC);
CommandHandler cmdHandler(&Serial1, (HardwareSerial *)&Serial, &raMotor, &decMotor);

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.listen();
}

void loop()
{
    cmdHandler.processSerial();
}