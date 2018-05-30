/* 
* RotationSensor.cpp
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel
*/

#include "RotationSensor.h"

 Sensors::RotationSensor::RotationSensor(Gpio::Pin* pins) : Sensor(pins)
{
    for (int i = 0; i < 8; i++) 
        Gpio::SetPinDirection(pins[i], Gpio::Dir::Input);
}

void* Sensors::RotationSensor::GetData()
{
    buffer = 0b00000000;
    
    for (int i = 0; i < 8; i++) 
        buffer |= ( (Gpio::GetPinValue(pins[i]) == Gpio::Value::High) ? 1 << i : 0 );
    
    return &buffer;
}
