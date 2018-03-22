/* 
* Gpio.cpp
*
* Created: 20-Mar-18 11:07:31
* Author: Robin C. Pel
*/

#include "Gpio.h"

using namespace Hardware::Gpio;

void Hardware::Gpio::SetPinDirection(Pin pin, Dir dir)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	uint8_t temp = port->DIR;						// Copy the current configuration
	
	temp &= ~(1 << pinNo);							// Clear the selected pin's direction
	temp |= (uint8_t) dir << pinNo;					// Configure the selected pin's direction
	
	port->DIR = temp;								// Apply the configuration
}

void Hardware::Gpio::TogglePinDirection(Pin pin)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	port->DIRTGL |= 1 << pinNo;						// Toggle the pin direction
}

void Hardware::Gpio::SetPinValue(Pin pin, Value val)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	uint8_t temp = port->OUT;						// Copy the current configuration
	
	temp &= ~(1 << pinNo);							// Clear the selected pin's direction
	temp |= (uint8_t) val << pinNo;					// Configure the selected pin's direction
	
	port->OUT = temp;								// Apply the configuration
}

void Hardware::Gpio::TogglePinValue(Pin pin)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	uint8_t pinNo = (uint8_t) GetPinNumber(pin);	// Get the selected pin number
	port->OUTTGL |= 1 << pinNo;						// Toggle the pin value
}

void Hardware::Gpio::SetPinMode(Pin pin, Mode mode)
{
	register8_t* CtrlReg = GetPinConfigReg(pin);	// Get a pointer to the selected pin configuration register
	uint8_t temp = *CtrlReg;						// Copy the current configuration
	
	temp &= 0b11000111;								// Clear the current mode
	temp |= (uint8_t) mode;							// Configure the selected mode
	
	*CtrlReg = temp;								// Apply the configuration
}

PORT_t* Hardware::Gpio::GetPort(Pin pin)
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

PinNo Hardware::Gpio::GetPinNumber(Pin pin)
{
	return (PinNo) ((uint8_t) pin & 0b111);			// Remove the port part
}

register8_t* Hardware::Gpio::GetPinConfigReg(Pin pin)
{
	PORT_t* port = GetPort(pin);					// Get a pointer to the selected port
	
	switch ( (PinNo) ((uint8_t) pin & 0b111) )		// Remove the port part
	{												// And return a pointer to the corresponding pin configuration register
		case PinNo::Pin0:		return &port->PIN0CTRL;
		case PinNo::Pin1:		return &port->PIN1CTRL;
		case PinNo::Pin2:		return &port->PIN2CTRL;
		case PinNo::Pin3:		return &port->PIN3CTRL;
		case PinNo::Pin4:		return &port->PIN4CTRL;
		case PinNo::Pin5:		return &port->PIN5CTRL;
		case PinNo::Pin6:		return &port->PIN6CTRL;
		case PinNo::Pin7:		return &port->PIN7CTRL;
		default:				return nullptr;
	}
}
