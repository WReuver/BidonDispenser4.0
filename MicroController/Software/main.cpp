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

int main() 
{
    //OSC.CTRL	= 0b00000011;				    // Enable the internal 32 and 2 MHz oscillators
    //while (! (OSC.STATUS & 0b00000010) );	    // Wait for the 32MHz internal oscillator to be enabled
    //CCP			= 0xD8;
    //CLK.CTRL	= 0b00000001;
    
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);              // TODO: Test Whether this works or not
	
	Gpio::SetPinDirection(Gpio::Pin::A0, Hardware::Gpio::Dir::Output);
	
	while (1) 
	{
		Gpio::TogglePinValue(Gpio::Pin::A0);
		_delay_ms(250);
	}
}
