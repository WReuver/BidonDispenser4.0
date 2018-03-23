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
	
	TCC0.CTRLA = 0b00000110;    // Use the regular clock as the source clock and divide it by 2
	TCC0.CTRLB = 0b11110011;    // Single Slope PWM mode
	//TCC0.CTRLC = 
	//TCC0.CTRLD = 
	//TCC0.CTRLE = 
	
	while (1) 
	{
		Gpio::TogglePinValue(Gpio::Pin::A0);
		_delay_ms(250);
	}
	
}
