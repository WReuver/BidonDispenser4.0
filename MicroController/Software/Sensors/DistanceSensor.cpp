/*
* DistanceSensor.cpp
*
* Created: 19-Mar-18 14:38:47
* Author: Robin C. Pel
*/

#include "DistanceSensor.h"

// Precision calculation (cm/tick) = (1 / ((F_CPU / Presc) / 1000000)) / 58
// Presc =  256:   0.13793103448 cm/tick
// Presc = 1024:   0.55172413793 cm/tick

Sensors::DistanceSensor::DistanceSensor(Gpio::Pin* triggerPin Gpio::Pin echoPin, Gpio::Pin* multiplexPin) : 
    triggerPin(triggerPin),
    echoPin(echoPin),
    multiplexPin(multiplexPin)
{
    for (int i = 0; i < 2; i++) 
        Gpio::SetPinDirection(triggerPin[i], Gpio::Dir::Output);
    
    Gpio::SetPinDirection(echoPin, Gpio::Dir::Input);
     
    for (int i = 0; i < 4; i++) 
        Gpio::SetPinDirection(multiplexPin[i], Gpio::Dir::Output);
}

float* Sensors::DistanceSensor::getData()
{
    sendTtl(pins[0]);
    uint16_t echo = getPulseWidth(pins[1]);
    buffer = ticksToCentimeters(1024, echo);
    
    return buffer;
}

uint16_t Sensors::DistanceSensor::getSimpleData()
{
    
}

void Sensors::DistanceSensor::setMuxChannel(uint8_t channel)
{
    
}

void Sensors::DistanceSensor::sendTtl(Gpio::Pin pin)
{
    Hardware::Gpio::SetPinValue(pin, Gpio::Value::High);
    _delay_us(15);
    Hardware::Gpio::SetPinValue(pin, Gpio::Value::Low);
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
