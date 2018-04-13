/* 
* RotationSensor.cpp
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel
*/

#include "RotationSensor.h"

 Sensors::RotationSensor::RotationSensor(Hardware::Gpio::Pin* pins) : Sensor(pins)
{
    Hardware::Gpio::SetPinDirection(pins[0], Hardware::Gpio::Dir::Input);
}

Sensors::RotationSensor::~RotationSensor() {}

void* Sensors::RotationSensor::GetData()
{
    buffer = Hardware::Gpio::GetPinValue(pins[0]) == Hardware::Gpio::Value::High;
    return &buffer;
}
