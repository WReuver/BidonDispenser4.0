/* 
* OneWire.cpp
*
* Created: 04-Apr-18 09:20:35
* Author: Robin C. Pel
*/

#include "OneWire.h"

uint8_t Hardware::Communication::OneWire::Initialize(Gpio::Pin pin)
{
    pinFloat(pin);
    while (Gpio::GetPinValue(Gpio::Pin::B0) == Gpio::Value::Low);       // Wait until the pin is high
    
    pinLow(pin);                                                        // Pull the pin low
     _delay_us(500);
     
     pinFloat(pin);                                                     // Let the pin float
     _delay_us(65);
     
     uint8_t response = (uint8_t) Gpio::GetPinValue(pin);               // Read the response
     _delay_us(490);
     
     return response;                                                   // Return the response
}

void Hardware::Communication::OneWire::WriteBit(Gpio::Pin pin, uint8_t bit)
{
    if (bit)
    {
        pinLow(pin);
        _delay_us(10);
        pinFloat(pin);
        _delay_us(55);
    }
    else
    {
        pinLow(pin);
        _delay_us(65);
        pinFloat(pin);
        _delay_us(5);
    }
}

uint8_t Hardware::Communication::OneWire::ReadBit(Gpio::Pin pin)
{
    uint8_t val = 0;
    
    pinLow(pin);
    _delay_us(3);
    
    pinFloat(pin);
    _delay_us(10);
    
    val = (uint8_t) Gpio::GetPinValue(pin);
    _delay_us(53);
    
    return val;
}

void Hardware::Communication::OneWire::Write(Gpio::Pin pin, uint8_t byte)
{
    for (uint8_t mask = 1; mask; mask <<= 1)
    {
        WriteBit(pin, mask & byte);
    }
    
    pinFloat(pin);
}

uint8_t Hardware::Communication::OneWire::Read(Gpio::Pin pin)
{
    uint8_t value = 0;
    
    for (uint8_t mask = 1; mask; mask <<= 1)
    {
        if (ReadBit(pin))
        {
            value |= mask;
        }
    }
    
    return value;
}

void Hardware::Communication::OneWire::pinLow(Gpio::Pin pin)
{
    Gpio::SetPinValue(pin, Gpio::Value::Low);
    Gpio::SetPinDirection(pin, Gpio::Dir::Output);
}

void Hardware::Communication::OneWire::pinFloat(Gpio::Pin pin)
{
    Gpio::SetPinDirection(pin, Gpio::Dir::Input);
    Gpio::SetPinValue(pin, Gpio::Value::High);
}
