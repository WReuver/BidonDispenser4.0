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

#define R0          Gpio::Pin::A0
#define R1          Gpio::Pin::A1
Gpio::Pin sensorPins[2] = { R0, R1 };

#define S0          Gpio::Pin::E7
#define S1          Gpio::Pin::E6
#define S2          Gpio::Pin::E5
Gpio::Pin muxPins[3] = { S0, S1, S2 };

// USART
#define RX          Gpio::Pin::C2
#define TX          Gpio::Pin::C3

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
}

static void setChannel(uint8_t channel)
{
    for (int i = 0; i < 3; i++)
    Gpio::SetPinValue(muxPins[i], (Gpio::Value) (( channel >> i ) & 0b1));
    
    _delay_ms(100);
}

int main() 
{
    initialize();
    
    // USART
    // Step 1
    Gpio::SetPinValue(TX, Value::High);
    
    // Step 2
    Gpio::SetPinDirection(TX, Dir::Output);
    Gpio::SetPinDirection(RX, Dir::Input);
    
    // Step 3
    USARTD0.BAUDCTRLA   = 0b00000000;
    USARTD0.BAUDCTRLB   = 0b00000000;
    USARTD0.CTRLA       = 0b00000000;
    USARTD0.CTRLB       = 0b00000000;
    USARTD0.CTRLC       = 0b00000000;
    USARTD0.DATA        = 0b00000000;
    USARTD0.STATUS      = 0b00000000;
    
    
    
    for (Gpio::Pin pin : muxPins)
    Gpio::SetPinDirection(pin, Gpio::Dir::Output);
    
    for (Gpio::Pin pin : sensorPins)
    Gpio::SetPinDirection(pin, Gpio::Dir::Input);
    
    TimerCounter::SetClock(TimerCounter::TC::TC0D, TimerCounter::ClockValue::Div1);
    TimerCounter::SetWaveformGenMode(TimerCounter::TC::TC0D, TimerCounter::WaveformGenMode::SingleSlope);
    TimerCounter::EnableOnPin(TimerCounter::TC::TC0D, Gpio::PinNo::Pin3);
    TimerCounter::SetPeriod(TimerCounter::TC::TC0D, 532);
    
    // Infinite Loop
    while (1)
    {
        Gpio::SetPinValue(Pin::E7, Value::Low);
        Gpio::SetPinValue(Pin::E6, Value::Low);
        Gpio::SetPinValue(Pin::E5, Value::Low);
        _delay_ms(1000);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 50, Gpio::PinNo::Pin3);
        _delay_ms(1000);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 0, Gpio::PinNo::Pin3);
        
        Gpio::SetPinValue(Pin::E7, Value::Low);
        Gpio::SetPinValue(Pin::E6, Value::Low);
        Gpio::SetPinValue(Pin::E5, Value::High);
        _delay_ms(1000);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 50, Gpio::PinNo::Pin3);
        _delay_ms(1000);
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0D, 0, Gpio::PinNo::Pin3);
    }
}
