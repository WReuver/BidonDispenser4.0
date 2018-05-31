/* 
* MotorController.cpp
*
* Created: 19-Mar-18 15:24:44
* Author: Robin C. Pel
*/

#include "MotorController.h"

 Controllers::MotorController::MotorController(Gpio::Pin* rotationSensorPin, TC motorTimerCounter, Gpio::Pin* multiplexPin):
    motorTimerCounter(motorTimerCounter),
    multiplexPin(multiplexPin)
{
    rotationSensor = new RotationSensor(rotationSensorPin);
    
    for (int i = 0; i < 3; i++)
        Gpio::SetPinDirection(multiplexPin[i], Gpio::Dir::Output);
    
    TimerCounter::SetClock(motorTimerCounter, ClockValue::Div1);                        // Start the clock for the timercounter and set the prescaler to 1
    TimerCounter::SetWaveformGenMode(motorTimerCounter, WaveformGenMode::SingleSlope);  // Set the waveform generation mode to Single slope PWM
    TimerCounter::EnableOnPin(motorTimerCounter, Gpio::PinNo::Pin1);                    // Enable the TC signal on Pin 1
    TimerCounter::SetPeriod(motorTimerCounter, 532);                                    // Set the period to 532 (Source Clock / (Prescaler * (Period + 1)) = 60.037)
}

void Controllers::MotorController::rotateMotor(uint8_t motorNumber)
{
    setMuxChannel(motorNumberToMuxChannel[motorNumber]);
    
    TimerCounter::SetDutyCycleOnPin(motorTimerCounter, 50, Gpio::PinNo::Pin1);          // Set the duty cycle on pin 1 to 50%
    _delay_ms(500);                                                                     // Wait a little bit
    while ( rotationSensor->getData() == ( 1 << motorNumber ) );                        // Wait until a gap is seen by the IR sensor
    TimerCounter::SetDutyCycleOnPin(motorTimerCounter, 0, Gpio::PinNo::Pin1);           // Set the duty cycle on pin 0 to 00%
    
    // Done
}

void Controllers::MotorController::setMuxChannel(uint8_t muxChannel)
{
    for (int i = 0; i < 3; i++)
        Gpio::SetPinValue(multiplexPin[i], (Gpio::Value) (( muxChannel >> i ) & 0b1));
    
    _delay_ms(1);       // Just to be sure the multiplexer has switched its channel we'll wait for one millisecond
}
