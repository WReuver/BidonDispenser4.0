/* 
* Gpio.h
*
* Created: 20-Mar-18 11:07:31
* Author: Robin C. Pel
*/

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <avr/io.h>

namespace Hardware 
{
	// Ports
	enum class Port
	{
		PortA,
		PortB,
		PortC,
		PortD,
		PortE,
		PortF,
		PortR
	};
	
	// Pins
	enum class Pin
	{
		Pin0,
		Pin1,
		Pin2,
		Pin3,
		Pin4,
		Pin5,
		Pin6,
		Pin7
	};
	
	// GpioPin - consist out of the port and the pin combined
	enum class GpioPin
	{
		PinA0 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin0,
		PinA1 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin1,
		PinA2 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin2,
		PinA3 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin3,
		PinA4 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin4,
		PinA5 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin5,
		PinA6 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin6,
		PinA7 = ((uint8_t) Port::PortA << 3) + (uint8_t) Pin::Pin7,
		PinB0 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin0,
		PinB1 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin1,
		PinB2 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin2,
		PinB3 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin3,
		PinB4 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin4,
		PinB5 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin5,
		PinB6 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin6,
		PinB7 = ((uint8_t) Port::PortB << 3) + (uint8_t) Pin::Pin7,
		PinC0 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin0,
		PinC1 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin1,
		PinC2 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin2,
		PinC3 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin3,
		PinC4 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin4,
		PinC5 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin5,
		PinC6 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin6,
		PinC7 = ((uint8_t) Port::PortC << 3) + (uint8_t) Pin::Pin7,
		PinD0 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin0,
		PinD1 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin1,
		PinD2 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin2,
		PinD3 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin3,
		PinD4 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin4,
		PinD5 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin5,
		PinD6 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin6,
		PinD7 = ((uint8_t) Port::PortD << 3) + (uint8_t) Pin::Pin7,
		PinE0 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin0,
		PinE1 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin1,
		PinE2 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin2,
		PinE3 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin3,
		PinE4 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin4,
		PinE5 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin5,
		PinE6 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin6,
		PinE7 = ((uint8_t) Port::PortE << 3) + (uint8_t) Pin::Pin7,
		PinF0 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin0,
		PinF1 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin1,
		PinF2 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin2,
		PinF3 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin3,
		PinF4 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin4,
		PinF5 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin5,
		PinF6 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin6,
		PinF7 = ((uint8_t) Port::PortF << 3) + (uint8_t) Pin::Pin7,
		PinR0 = ((uint8_t) Port::PortR << 3) + (uint8_t) Pin::Pin0,
		PinR1 = ((uint8_t) Port::PortR << 3) + (uint8_t) Pin::Pin1
	};
	
	// Pin Directions
	enum class GpioDir
	{
		Input,
		Output
	};
	
	// Pin Values
	enum class GpioValue
	{
		Low,
		High
	};
	
	// Pin Modes
	enum class GpioMode
	{
		Totem			= 0b00000000,
		BusKeeper		= 0b00001000,
		PullDown		= 0b00010000,
		PullUp			= 0b00011000,
		WiredOr			= 0b00100000,
		WiredAnd		= 0b00101000,
		WiredOrPullDown	= 0b00110000,
		WiredAndPullUp	= 0b00111000
	};
	
	
	// Functions
	void SetPinDirection(GpioPin pin, GpioDir dir);
	void TogglePinDirection(GpioPin pin);
	void SetPinValue(GpioPin pin, GpioValue val);
	void TogglePinValue(GpioPin pin);
	void SetPinMode(GpioPin pin, GpioMode mode);
	
	PORT_t* GetPort(GpioPin pin);
	Pin GetPinNumber(GpioPin pin);
	register8_t* GetPinConfigReg(GpioPin pin);
}

#endif //__GPIO_H__
