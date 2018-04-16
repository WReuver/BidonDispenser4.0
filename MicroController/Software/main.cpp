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
using namespace Gpio;

Gpio::Pin sensorPins[2] = { Gpio::Pin::A0, Gpio::Pin::A1 };
Gpio::Pin muxPins[3] = { Gpio::Pin::E5, Gpio::Pin::E6, Gpio::Pin::E7 };

// USART
#define RX          Gpio::Pin::C2
#define TX          Gpio::Pin::C3

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    for (Gpio::Pin pin : muxPins) Gpio::SetPinDirection(pin, Gpio::Dir::Output);
    //for (Gpio::Pin pin : sensorPins) Gpio::SetPinDirection(pin, Gpio::Dir::Input);
    
    Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Input);
    Gpio::SetPinDirection(Gpio::Pin::A1, Gpio::Dir::Input);
    
    PORTA.INTCTRL  |= 0b00001111;
    PORTA.PIN0CTRL |= 0b00000001;
    PORTA.PIN1CTRL |= 0b00000001;
    PORTA.INT0MASK |= 0b00000001;
    PORTA.INT1MASK |= 0b00000010;
}

static void setChannel(uint8_t channel)
{
    for (int i = 0; i < 3; i++) Gpio::SetPinValue(muxPins[i], (Gpio::Value) (( channel >> i ) & 0b1));
    _delay_ms(1);
}

int main() 
{
    initialize();
    
    //// USART
    //// Step 1
    //Gpio::SetPinValue(TX, Value::High);
    //
    //// Step 2
    //Gpio::SetPinDirection(TX, Dir::Output);
    //Gpio::SetPinDirection(RX, Dir::Input);
    //
    //// Step 3
    //USARTD0.BAUDCTRLA   = 0b00000000;
    //USARTD0.BAUDCTRLB   = 0b00000000;
    //USARTD0.CTRLA       = 0b00000000;
    //USARTD0.CTRLB       = 0b00000000;
    //USARTD0.CTRLC       = 0b00000000;
    //USARTD0.DATA        = 0b00000000;
    //USARTD0.STATUS      = 0b00000000;
    
    
    TimerCounter::SetClock(TimerCounter::TC::TC0D, TimerCounter::ClockValue::Div1);
    TimerCounter::SetWaveformGenMode(TimerCounter::TC::TC0D, TimerCounter::WaveformGenMode::SingleSlope);
    TimerCounter::EnableOnPin(TimerCounter::TC::TC0D, Gpio::PinNo::Pin3);
    TimerCounter::SetPeriod(TimerCounter::TC::TC0D, 532);
    PORTA.INTFLAGS = 0b11;
    
    // Infinite Loop
    while (1)
    {
        _delay_ms(1000);
        setChannel(0);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 50, Gpio::PinNo::Pin3);
        while (!(PORTA.INTFLAGS & 0b10));
        PORTA.INTFLAGS = 0b11;
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 0, Gpio::PinNo::Pin3);
        
        _delay_ms(1000);
        setChannel(1);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 50, Gpio::PinNo::Pin3);
        while (!(PORTA.INTFLAGS & 0b01));
        PORTA.INTFLAGS = 0b11;
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 0, Gpio::PinNo::Pin3);
        
        /*
        _delay_ms(1000);
        setChannel(0);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 50, Gpio::PinNo::Pin3);
        while (PORTA.INTFLAGS & 0b10);
        PORTA.INTFLAGS = 0b11;
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 0, Gpio::PinNo::Pin3);
        
        _delay_ms(1000);
        setChannel(1);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 50, Gpio::PinNo::Pin3);
        while (PORTA.INTFLAGS & 0b01);
        PORTA.INTFLAGS = 0b11;
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 0, Gpio::PinNo::Pin3);
        */
        
    }
}
