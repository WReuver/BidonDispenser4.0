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
	namespace Gpio 
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
		
		// Pin numbers
		enum class PinNo
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
		
		// Pin - consist out of a port and pin number
		enum class Pin
		{
			A0 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin0,
			A1 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin1,
			A2 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin2,
			A3 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin3,
			A4 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin4,
			A5 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin5,
			A6 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin6,
			A7 = ((uint8_t) Port::PortA << 3) + (uint8_t) PinNo::Pin7,
			B0 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin0,
			B1 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin1,
			B2 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin2,
			B3 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin3,
			B4 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin4,
			B5 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin5,
			B6 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin6,
			B7 = ((uint8_t) Port::PortB << 3) + (uint8_t) PinNo::Pin7,
			C0 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin0,
			C1 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin1,
			C2 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin2,
			C3 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin3,
			C4 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin4,
			C5 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin5,
			C6 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin6,
			C7 = ((uint8_t) Port::PortC << 3) + (uint8_t) PinNo::Pin7,
			D0 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin0,
			D1 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin1,
			D2 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin2,
			D3 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin3,
			D4 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin4,
			D5 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin5,
			D6 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin6,
			D7 = ((uint8_t) Port::PortD << 3) + (uint8_t) PinNo::Pin7,
			E0 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin0,
			E1 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin1,
			E2 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin2,
			E3 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin3,
			E4 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin4,
			E5 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin5,
			E6 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin6,
			E7 = ((uint8_t) Port::PortE << 3) + (uint8_t) PinNo::Pin7,
			F0 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin0,
			F1 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin1,
			F2 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin2,
			F3 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin3,
			F4 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin4,
			F5 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin5,
			F6 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin6,
			F7 = ((uint8_t) Port::PortF << 3) + (uint8_t) PinNo::Pin7,
			R0 = ((uint8_t) Port::PortR << 3) + (uint8_t) PinNo::Pin0,
			R1 = ((uint8_t) Port::PortR << 3) + (uint8_t) PinNo::Pin1
		};
		
		// Pin Directions
		enum class Dir
		{
			Input,
			Output
		};
	
		// Pin Values
		enum class Value
		{
			Low,
			High
		};
	
		// Pin Modes
		enum class Mode
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
		void SetPinDirection(Pin pin, Dir dir);                 // Configure the given direction on the given pin
		void TogglePinDirection(Pin pin);                       // Toggle the direction on the given pin
		void SetPinValue(Pin pin, Value val);                   // Set the given value on the given pin
        Value GetPinValue(Pin pin);                             // Get the value of the given pin
		void TogglePinValue(Pin pin);                           // Toggle the value of the given pin
		void SetPinMode(Pin pin, Mode mode);                    // Configure the given mode on the given pin
		PORT_t* GetPort(Pin pin);                               // Get a pointer to the port of the given pin
		PinNo GetPinNumber(Pin pin);                            // Get the pin number of the given pin
		register8_t* GetPinConfigReg(Pin pin);                  // Get the pin configuration register of the given pin
        Pin GetPinFromPortAndPinNo(Port port, PinNo pinNo);     // Get a pin from the given port and pin number
		
		
	}
}

#endif //__GPIO_H__
