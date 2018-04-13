/* 
* TemperatureSensor.cpp
*
* Created: 19-Mar-18 14:38:58
* Author: Robin C. Pel
*/

#include "TemperatureSensor.h"

Sensors::TemperatureSensor::~TemperatureSensor() {}

void* Sensors::TemperatureSensor::GetData()
{
    if ( initializationSequence() ) return nullptr;
    convertData();
    
    if ( initializationSequence() ) return nullptr;
    buffer = rawToCelsius(readRawTemperature());
    
	return &buffer;
}

bool Sensors::TemperatureSensor::SetResolution(Resolution resolution)
{
    return false;       // Probably won't be added
}

uint8_t Sensors::TemperatureSensor::initializationSequence()
{
    uint8_t result = Hardware::Communication::OneWire::Initialize(pins[0]);
    OneWire::Write(pins[0], (uint8_t) RomCommand::Skip);
    return result;
}

void Sensors::TemperatureSensor::convertData()
{
    OneWire::Write(pins[0], (uint8_t) FunctionCommand::Convert);
    while (!OneWire::ReadBit(pins[0]));
}

int16_t Sensors::TemperatureSensor::readRawTemperature()
{
    OneWire::Write(pins[0], (uint8_t) FunctionCommand::ReadScratchPad);
    int16_t rawTemperature = 0;
    
    rawTemperature = OneWire::Read(pins[0]);
    rawTemperature += (int16_t) OneWire::Read(pins[0]) * 256;
    
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
