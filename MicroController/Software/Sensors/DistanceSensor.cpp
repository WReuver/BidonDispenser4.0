/* 
* DistanceSensor.cpp
*
* Created: 19-Mar-18 14:38:47
* Author: Robin C. Pel
*/

#include "DistanceSensor.h"

using namespace Hardware;

// pins[0] = Trigger
// pins[1] = Echo

 Sensors::DistanceSensor::DistanceSensor(Gpio::Pin* pins) : Sensor(pins)
{
    Gpio::SetPinDirection(pins[0], Gpio::Dir::Output);
    Gpio::SetPinDirection(pins[1], Gpio::Dir::Input);
}

Sensors::DistanceSensor::~DistanceSensor() {}

void* Sensors::DistanceSensor::GetData()
{
    TimerCounter::SetClock(TimerCounter::TC::TC0D, TimerCounter::ClockValue::Div256);       // Start the clock for TC0D and set the prescaler to 256
    _delay_us(10);                                                                          // Wait a bit
    
    sendTtl(pins[0]);
    uint16_t echo = getPulseWidth(pins[1]);
    buffer = ticksToCentimeters(256, echo);
    
    return &buffer;
}

void Sensors::DistanceSensor::sendTtl(Hardware::Gpio::Pin pin)
{
    Hardware::Gpio::SetPinValue(pin, Hardware::Gpio::Value::High);
    _delay_us(15);
    Hardware::Gpio::SetPinValue(pin, Hardware::Gpio::Value::Low);
}

uint16_t Sensors::DistanceSensor::getPulseWidth(Gpio::Pin pin)
{
    while (Gpio::GetPinValue(pin) == Gpio::Value::Low);
    TimerCounter::ClearCount(TimerCounter::TC::TC0D);
    while (Gpio::GetPinValue(pin) == Gpio::Value::High);
    return TimerCounter::GetCount(TimerCounter::TC::TC0D);
}

float Sensors::DistanceSensor::ticksToCentimeters(uint16_t prescval, uint16_t ticks)
{
    float us = TimerCounter::TicksToMicoSeconds(prescval, ticks);
    return us / 58.0;
}
