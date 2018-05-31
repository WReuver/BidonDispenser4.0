/* 
* CoolingController.cpp
*
* Created: 19-Mar-18 15:27:07
* Author: Robin C. Pel
*/

#include "CoolingController.h"

Controllers::CoolingController::CoolingController(Gpio::Pin* temperatureSensorPins, Gpio::Pin* fanGroupPins, TC coolingTimerCounter)
{
    // Initialize the pins
    Gpio::Pin ts0[1] = { temperatureSensorPins[0] };
    Gpio::Pin ts1[1] = { temperatureSensorPins[1] };
    Gpio::Pin ts2[1] = { temperatureSensorPins[2] };
    
    temperatureSensor[0] = new TemperatureSensor(ts0);
    temperatureSensor[1] = new TemperatureSensor(ts1);
    temperatureSensor[2] = new TemperatureSensor(ts2);
    
    fanGroup[0] = fanGroupPins[0];
    fanGroup[1] = fanGroupPins[1];
    
    // Initialize the timercounter
    TimerCounter::SetClock(coolingTimerCounter, ClockValue::Div1);                          // Start the clock for the timercounter and set the prescaler to 1
    TimerCounter::SetWaveformGenMode(coolingTimerCounter, WaveformGenMode::SingleSlope);    // Set the waveform generation mode to Single slope PWM
    TimerCounter::EnableOnPin(coolingTimerCounter, Gpio::GetPinNumber(fanGroup[0]));        // Enable the TC signal on the fangroup 0 pin
    TimerCounter::EnableOnPin(coolingTimerCounter, Gpio::GetPinNumber(fanGroup[1]));        // Enable the TC signal on the fangroup 1 pin
    TimerCounter::SetPeriod(coolingTimerCounter, 532);                                      // Set the period to 532 (Source Clock / (Prescaler * (Period + 1)) = 60.037)
}

void Controllers::CoolingController::updateFanSpeed()
{
    gatherTemperatures();
    
    if (lowerTargetTemperature < lowerTemperature) 
    {
        // Start the fans?
    }
    else
    {
        // Stop the fans?
    }
    
}

void Controllers::CoolingController::setLowerTargetTemperature(float targetTemp)
{
    lowerTargetTemperature = targetTemp;
}

void Controllers::CoolingController::gatherTemperatures()
{
    lowerTemperature = temperatureSensor[0]->getData();
    middleTemperature = temperatureSensor[1]->getData();
    upperTemperature = temperatureSensor[2]->getData();
}

