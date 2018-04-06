/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#include "includes.h"

#include "Hardware/Gpio.h"
#include "Hardware/SystemClock.h"
#include "Sensors/TemperatureSensor.h"

using namespace Hardware;
using namespace Sensors;

Sensor* sensors[2];

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
}

int main() 
{
    initialize();
	Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Output);
	
    volatile float realTemp = -100;
    
    Gpio::Pin pins[1] = {Gpio::Pin::B0};
    //TemperatureSensor* ts = new TemperatureSensor(pins);
    sensors[0] = new TemperatureSensor(pins);
    
	Gpio::TogglePinValue(Gpio::Pin::A0);
    _delay_ms(500);
    
    void* dataLocation = sensors[0]->GetData();
    
    realTemp = *((float*) dataLocation);
        
    Gpio::TogglePinValue(Gpio::Pin::A0);
    _delay_ms(500);
    
    delete sensors[0];
    
}
