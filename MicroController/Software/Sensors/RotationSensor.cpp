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
    {
        Gpio::SetPinDirection(pins[i], Gpio::Dir::Input);
        Gpio::SetPinMode(pins[i], Gpio::Mode::PullDown);
    }        
}

uint8_t Sensors::RotationSensor::getData()
{
    uint8_t measurement = 0b00000000;
    
    for (int i = 0; i < 8; i++) 
    {
        if (Gpio::GetPinValue(pins[i]) == Gpio::Value::High)
        {
            measurement |= (1 << i);
        }
    }
    
    return measurement;
}
