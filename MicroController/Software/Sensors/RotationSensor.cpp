/* 
* RotationSensor.cpp
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel
*/

#include "RotationSensor.h"

 Sensors::RotationSensor::RotationSensor(Gpio::Pin* pins) : pins(pins)
{
    for (int i = 0; i < 8; i++) 
        Gpio::SetPinDirection(pins[i], Gpio::Dir::Input);
}

uint8_t Sensors::RotationSensor::getData()
{
    uint8_t measurement = 0b00000000;
    
    for (int i = 0; i < 8; i++) 
        measurement |= ( (Gpio::GetPinValue(pins[i]) == Gpio::Value::High) ? 1 << i : 0 );
    
    return measurement;
}
