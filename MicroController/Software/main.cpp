/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#include "includes.h"
#include "Hardware/SystemClock.h"

using namespace Hardware;

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
}

int main() 
{
    initialize();
	Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Input);
	Gpio::SetPinDirection(Gpio::Pin::B0, Gpio::Dir::Output);
    Gpio::SetPinDirection(Gpio::Pin::C0, Gpio::Dir::Output);
    
    TCC0.CTRLA = 0b00000001;
    TCC0.CTRLB = 0b00010011;
    //TCC0.CCA   = 0x7FFF;              // Size of the pulses
    TCC0.PER   = 532;              // Max amount of ticks
    
    TCD0.CTRLA = 0b00000111;        // Prescaler val = 1024
    TCD0.PER = 0x7A12;
    //TCD0.CTRLB = 0b00010011;
    //TCD0.CCA = 0xFF;
    
    //TCE0.CTRLA = 0b00000011;
    //TCE0.CTRLB = 0b00010011;
    //TCE0.CCA = 0x7F;
    //TCE0.PER = 0xFE;
    //
    //TCF0.CTRLA = 0b00000011;
    //TCF0.CTRLB = 0b00010011;
    //TCF0.CCA = 0x7F;
    //TCF0.PER = 0xFE;
    
    // PWM = PER / CCx
    
    while (1)
    {
        Gpio::TogglePinValue(Gpio::Pin::B0);
        while (TCD0.CNT < 0x7A12);
        Gpio::TogglePinValue(Gpio::Pin::B0);
        while (TCD0.CNT < 0x7A12);
        //_delay_ms(1000);
    }
    
    while (1)
    {
        TCC0.CCA = TCC0.PER / 2;
        _delay_ms(500);
        while (Gpio::GetPinValue(Gpio::Pin::A0) == Gpio::Value::Low);
        TCC0.CCA = 0;
        _delay_ms(300000);
    }
    
    
}
