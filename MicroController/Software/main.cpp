/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/TimerCounter.h"

using namespace Hardware;

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Input);
    Gpio::SetPinDirection(Gpio::Pin::B0, Gpio::Dir::Output);
}

int main() 
{
    initialize();
    
    //TCC0.CTRLA = 0b00000001;
    TimerCounter::SetClock(TimerCounter::TC::TC0C, TimerCounter::ClockValue::Div1);
    
    //TCC0.CTRLB = 0b00010011;
    TimerCounter::SetWaveformGenMode(TimerCounter::TC::TC0C, TimerCounter::WaveformGenMode::SingleSlope);
    TimerCounter::EnableOnPin(TimerCounter::TC::TC0C, Gpio::PinNo::Pin0);
    
    //TCC0.PER   = 532;              // Max amount of ticks
    TimerCounter::SetPeriod(TimerCounter::TC::TC0C, 532);
    
    
    //TCD0.CTRLA = 0b00000111;        // Prescaler val = 1024
    TimerCounter::SetClock(TimerCounter::TC::TC0D, TimerCounter::ClockValue::Div1024);
    
    //TCD0.PER = 0x7A12;
    TimerCounter::SetPeriod(TimerCounter::TC::TC0D, 0x7A12);
    
    // Counter test
    while (1)
    {
        Gpio::TogglePinValue(Gpio::Pin::B0);
        while (TCD0.CNT < 0x7A12);
        Gpio::TogglePinValue(Gpio::Pin::B0);
        while (TCD0.CNT < 0x7A12);
    }
    
    // PWM test
    //while (1)
    //{
        ////TCC0.CCA = TCC0.PER / 2;
        //status = TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0C, 50, Gpio::PinNo::Pin0);
        //_delay_ms(500);
        //while (Gpio::GetPinValue(Gpio::Pin::A0) == Gpio::Value::Low);
        ////_delay_us(10);
        ////while (Gpio::GetPinValue(Gpio::Pin::A0) == Gpio::Value::Low);
        ////TCC0.CCA = 0;
        //status = TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0C, 0, Gpio::PinNo::Pin0);
        //_delay_ms(3000);
    //}
    
    
}
