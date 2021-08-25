#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Enums.hpp"
#include "Motor.hpp"
#include "CommandHandler.hpp"

SoftwareSerial Serial1(2, 3);
Motor myRaMotor(AxisEnum::AXIS_RA);
Motor myDecMotor(AxisEnum::AXIS_DEC);
CommandHandler cmdHandler(&Serial1, (HardwareSerial *)&Serial, &myRaMotor, &myDecMotor);

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