/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/TimerCounter.h"
#include "Sensors/DistanceSensor.h"

using namespace Hardware;
using namespace Gpio;

#define TRIGGER1    Gpio::Pin::D2
#define TRIGGER0    Gpio::Pin::D1
#define ECHO        Gpio::Pin::D0

#define S0          Gpio::Pin::C7
#define S1          Gpio::Pin::C6
#define S2          Gpio::Pin::C5

Gpio::Pin muxPins[3] = { S0, S1, S2 };

Gpio::Pin leds[3] = { Gpio::Pin::A0, Gpio::Pin::A1, Gpio::Pin::A2 };

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    
    for (Gpio::Pin pin : muxPins)
    Gpio::SetPinDirection(pin, Gpio::Dir::Output);
    
    for (Gpio::Pin pin : leds)
    Gpio::SetPinDirection(pin, Gpio::Dir::Output);
}

static void setChannel(uint8_t channel)
{
    for (int i = 0; i < 3; i++)
    Gpio::SetPinValue(muxPins[i], (Gpio::Value) (( channel >> i ) & 0b1));
    _delay_ms(100);
}

int main() 
{
    initialize();
    
    Gpio::Pin pins0[2] = { TRIGGER0, ECHO };
    Gpio::Pin pins1[2] = { TRIGGER1, ECHO };
    //Sensors::DistanceSensor* dss[3] = { new Sensors::DistanceSensor(pins), new Sensors::DistanceSensor(pins), new Sensors::DistanceSensor(pins) };
    Sensors::DistanceSensor* ds0 = new Sensors::DistanceSensor(pins0);
    Sensors::DistanceSensor* ds1 = new Sensors::DistanceSensor(pins1);
    Sensors::DistanceSensor* ds2 = new Sensors::DistanceSensor(pins1);
    volatile float dists[3] = {0.0, 0.0, 0.0};
    //volatile float dist = 0.0;
    
    //TimerCounter::SetClock(TimerCounter::TC::TC0D, TimerCounter::ClockValue::Div256);
    
    while (1)
    {
        setChannel(0);
        //Gpio::SetPinValue(leds[0], Gpio::GetPinValue(ECHO));
        dists[0] = *( (float*) ds0->GetData() );
        _delay_ms(1000);
        
        setChannel(1);
        //Gpio::SetPinValue(leds[1], Gpio::GetPinValue(ECHO));
        dists[1] = *( (float*) ds1->GetData() );
        _delay_ms(1000);
        
        setChannel(2);
        //Gpio::SetPinValue(leds[2], Gpio::GetPinValue(ECHO));
        dists[2] = *( (float*) ds2->GetData() );
        _delay_ms(1000);
    }
}
