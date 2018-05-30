/* 
* TemperatureSensor.cpp
*
* Created: 19-Mar-18 14:38:58
* Author: Robin C. Pel
*/

#include "TemperatureSensor.h"

Sensors::TemperatureSensor::TemperatureSensor(Hardware::Gpio::Pin* pins) : pins(pins) {}

float Sensors::TemperatureSensor::getData()
{
    if ( initializationSequence() ) return 50.0;
    
    convertData();
    
    if ( initializationSequence() ) return 50.0;
    
	return rawToCelsius(readRawTemperature());
}

uint8_t Sensors::TemperatureSensor::initializationSequence()
{
    uint8_t result = Communication::OneWire::Initialize(pins[0]);
    Communication::OneWire::Write(pins[0], (uint8_t) RomCommand::Skip);
    return result;
}

void Sensors::TemperatureSensor::convertData()
{
    Communication::OneWire::Write(pins[0], (uint8_t) FunctionCommand::Convert);
    while (!Communication::OneWire::ReadBit(pins[0]));
}

int16_t Sensors::TemperatureSensor::readRawTemperature()
{
    Communication::OneWire::Write(pins[0], (uint8_t) FunctionCommand::ReadScratchPad);
    int16_t rawTemperature = 0;
    
    rawTemperature = Communication::OneWire::Read(pins[0]);
    rawTemperature += (int16_t) Communication::OneWire::Read(pins[0]) * 256;
    
    return rawTemperature;
}

float Sensors::TemperatureSensor::rawToCelsius(int16_t raw)
{
    return (float) raw / resolutionDivider();
}

uint8_t Sensors::TemperatureSensor::resolutionDivider()
{
    uint8_t divider = 1;
    
    for (int i = 0; i <= (uint8_t)resolution; i++)
    {
        divider *= 2;
    }
    
    return divider;
}
