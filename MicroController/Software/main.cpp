/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#define F_CPU 32000000UL

#include <util/delay.h>
#include <stdint.h>

#include "Hardware/Gpio.h"
#include "Hardware/SystemClock.h"

using namespace Hardware;

static void initialize(void) 
{
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
}

int main() 
{
    initialize();
	
	Gpio::SetPinDirection(Gpio::Pin::A0, Hardware::Gpio::Dir::Output);
	Gpio::SetPinDirection(Gpio::Pin::C0, Hardware::Gpio::Dir::Output);
	
    PR.PRGEN = 0;
    
	TCC0.CTRLA  = 0b00000010;
    TCC0.CTRLB  = 0b00010011;
    
	TCC1.CTRLA = 0b00000010;
	TCC1.CTRLB = 0b00010011;
	//TCC2.CTRLA = 0b00000010;
	//TCC2.CTRLB = 0b01000100;
    
    
	
	while (1) 
	{
		Gpio::TogglePinValue(Gpio::Pin::A0);
		_delay_ms(250);
	}
	
}
