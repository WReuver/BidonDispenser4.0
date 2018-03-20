/* 
* Gpio.cpp
*
* Created: 20-Mar-18 11:07:31
* Author: Robin C. Pel
*/

#include "Gpio.h"

void Hardware::SetPinDirection(GpioPin pin, GpioDir dir)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	uint8_t temp = port->DIR;						// Copy the current configuration
	
	temp &= ~(1 << pinNo);							// Clear the selected pin's direction
	temp |= (uint8_t) dir << pinNo;					// Configure the selected pin's direction
	
	port->DIR = temp;								// Apply the configuration
}

void Hardware::TogglePinDirection(GpioPin pin)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	port->DIRTGL |= 1 << pinNo;						// Toggle the pin direction
}

void Hardware::SetPinValue(GpioPin pin, GpioValue val)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	uint8_t temp = port->OUT;						// Copy the current configuration
	
	temp &= ~(1 << pinNo);							// Clear the selected pin's direction
	temp |= (uint8_t) val << pinNo;					// Configure the selected pin's direction
	
	port->OUT = temp;								// Apply the configuration
}

void Hardware::TogglePinValue(GpioPin pin)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	port->OUTTGL |= 1 << pinNo;						// Toggle the pin value
}

void Hardware::SetPinMode(GpioPin pin, GpioMode mode)
{
	register8_t* CtrlReg = GetPinConfigReg(pin);	// Get a pointer to the selected pin configuration register
	uint8_t temp = *CtrlReg;						// Copy the current configuration
	
	temp &= 0b11000111;								// Clear the current mode
	temp |= (uint8_t) mode;							// Configure the selected mode
	
	*CtrlReg = temp;								// Apply the configuration
}

PORT_t* Hardware::GetPort(GpioPin pin)
{
	switch ( (Port) ((uint8_t) pin >> 3) )			// Bit shift the pin part off
	{												// And return a pointer to the corresponding port
		case Port::PortA:	return &PORTA;
		case Port::PortB:	return &PORTB;
		case Port::PortC:	return &PORTC;
		case Port::PortD:	return &PORTD;
		case Port::PortE:	return &PORTE;
		case Port::PortF:	return &PORTF;
		default:			return nullptr;
	}
}

Hardware::Pin Hardware::GetPinNumber(GpioPin pin)
{
	return (Pin) ((uint8_t) pin & 0b111);			// Remove the port part
}

register8_t* Hardware::GetPinConfigReg(GpioPin pin)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	
	switch ( (Pin) ((uint8_t) pin & 0b111) )		// Remove the port part
	{												// And return a pointer to the corresponding pin configuration register
		case Pin::Pin0:		return &port->PIN0CTRL;
		case Pin::Pin1:		return &port->PIN1CTRL;
		case Pin::Pin2:		return &port->PIN2CTRL;
		case Pin::Pin3:		return &port->PIN3CTRL;
		case Pin::Pin4:		return &port->PIN4CTRL;
		case Pin::Pin5:		return &port->PIN5CTRL;
		case Pin::Pin6:		return &port->PIN6CTRL;
		case Pin::Pin7:		return &port->PIN7CTRL;
		default:			return nullptr;
	}
}
