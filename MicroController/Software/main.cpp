/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#define F_CPU 2000000UL

#include <util/delay.h>
#include <stdint.h>

#include "Hardware/Gpio.h"

int main() 
{
	Hardware::SetPinDirection(Hardware::GpioPin::PinA0, Hardware::GpioDir::Output);
	
	while (1) 
	{
		Hardware::TogglePinValue(Hardware::GpioPin::PinA0);
		_delay_ms(250);
	}
}
