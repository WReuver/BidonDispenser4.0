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

// Precision calculation (cm/tick) = (1 / ((F_CPU / Presc) / 1000000)) / 58
// Presc =  256:   0.13793103448 cm/tick
// Presc = 1024:   0.55172413793 cm/tick

Sensors::DistanceSensor::DistanceSensor(Gpio::Pin* pins) : Sensor(pins)
{
    Gpio::SetPinDirection(pins[0], Gpio::Dir::Output);
    Gpio::SetPinDirection(pins[1], Gpio::Dir::Input);
}

void* Sensors::DistanceSensor::GetData()
{
    sendTtl(pins[0]);
    uint16_t echo = getPulseWidth(pins[1]);
    buffer = ticksToCentimeters(1024, echo);
    
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
    TimerCounter::ClearCount(TimerCounter::GetGenericTC());
    while (Gpio::GetPinValue(pin) == Gpio::Value::High);
    return TimerCounter::GetCount(TimerCounter::GetGenericTC());
}

float Sensors::DistanceSensor::ticksToCentimeters(uint16_t prescval, uint16_t ticks)
{
    float us = TimerCounter::TicksToMicoSeconds(prescval, ticks);
    return us / 58.0;
}
