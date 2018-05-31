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

/* Columns Layout (Top view, from the front):
 * 
 * ColumnNo:             00  01  02  03  04  05  06  07
 * ColumnSectionNo:     [08][09][10][11][12][13][14][15]    <-- Trigger 1
 * ColumnSectionNo:     [00][01][02][03][04][05][06][07]    <-- Trigger 0
 */

Sensors::DistanceSensor::DistanceSensor(Gpio::Pin* triggerPin, Gpio::Pin echoPin, Gpio::Pin* multiplexPin, float emptyDistance) : 
    // Initialize some variables
    triggerPin(triggerPin), 
    echoPin(echoPin), 
    multiplexPin(multiplexPin),
    emptyDistance(emptyDistance)
{
    for (int i = 0; i < 2; i++) 
        Gpio::SetPinDirection(triggerPin[i], Gpio::Dir::Output);        // Configure the trigger pins as output
    
    Gpio::SetPinDirection(echoPin, Gpio::Dir::Input);                   // Configure the echo pin as input
     
    for (int i = 0; i < 4; i++) 
        Gpio::SetPinDirection(multiplexPin[i], Gpio::Dir::Output);      // Configure the mux pins as output
}

float* Sensors::DistanceSensor::getData()
{
    for (int i = 0; i < 16; i++) 
    {
        buffer[i] = getDistance(i);     // Get the distance of each sensor
        _delay_ms(1);                   // Wait a bit to make sure trigger 0 and trigger 1 do not get mixed
    }
    
    return buffer;                      // Return a pointer which points to where the read data is stored
}

uint8_t Sensors::DistanceSensor::getSimpleData()
{
    uint8_t simpleData = 0b00000000;
    getData();
    
    // Check every column
    for (int i = 0; i < 8; i++) 
    {
        if ( (buffer[i] > emptyDistance) || (buffer[i+8] > emptyDistance) )     // Every column consists out of two column sections, an even and an odd section. We need to check them both!
            simpleData |= ( 1 << i );                                           // Bit-shift the column status to the correct bit
    }
    
    return simpleData;
}

void Sensors::DistanceSensor::setMuxChannel(uint8_t channel)
{
    for (int i = 0; i < 4; i++)
        Gpio::SetPinValue(multiplexPin[i], (Gpio::Value) (( channel >> i ) & 0b1));
    
    _delay_ms(1);       // Just to be sure the multiplexer has switched its channel we'll wait for one millisecond
}

void Sensors::DistanceSensor::sendTtl(Gpio::Pin pin)
{
    Gpio::SetPinValue(pin, Gpio::Value::High);      // Set the pin high
    _delay_us(15);                                  // For 15 microseconds
    Gpio::SetPinValue(pin, Gpio::Value::Low);       // And set it low again
}

uint16_t Sensors::DistanceSensor::getPulseWidth(Gpio::Pin pin)
{
    while (Gpio::GetPinValue(pin) == Gpio::Value::Low);                 // Wait for the pin to go high
    TimerCounter::ClearCount(TimerCounter::GetGenericTC());             // Clear the counter
    while (Gpio::GetPinValue(pin) == Gpio::Value::High);                // Wait for the pin to go low
    return TimerCounter::GetCount(TimerCounter::GetGenericTC());        // Return how long the signal was high aka the "pulse width"
}

float Sensors::DistanceSensor::ticksToCentimeters(uint16_t prescval, uint16_t ticks)
{
    float us = TimerCounter::TicksToMicoSeconds(prescval, ticks);       // Convert the amount of ticks to microseconds
    return us / 58.0;                                                   // Calculate the distance in centimeters
}

float Sensors::DistanceSensor::getDistance(uint8_t columnSectionNo)
{
    setMuxChannel(columnSectionNo);                     // Set the correct mux channel
    
    if (columnSectionNo > 7) sendTtl(triggerPin[1]);    // Send the TTL to trigger 1
    else sendTtl(triggerPin[0]);                        // Send the TTL to trigger 0
    
    uint16_t echo = getPulseWidth(echoPin);             // Get the width of the pulse
    return ticksToCentimeters(1024, echo);              // Return the measured distance in centimeters
}
