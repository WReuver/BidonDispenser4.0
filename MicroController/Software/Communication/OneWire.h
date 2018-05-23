/* 
* OneWire.h
*
* Created: 04-Apr-18 09:20:35
* Author: Robin C. Pel
*/

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include "../includes.h"
#include "../Hardware/Gpio.h"

using namespace Hardware;

namespace Communication
{
    namespace OneWire
    {
        // Functions
        uint8_t Initialize(Gpio::Pin pin);              // Initialize the one wire device on the selected pin
        void WriteBit(Gpio::Pin pin, uint8_t bit);      // Write a bit to the one wire device on the selected pin
        uint8_t ReadBit(Gpio::Pin pin);                 // Read a bit from the one wire device on the selected pin
        void Write(Gpio::Pin pin, uint8_t byte);        // Write a byte to the one wire device on the selected pin
        uint8_t Read(Gpio::Pin pin);                    // Read a byte from the one wire device on the selected pin
            
        void pinLow(Gpio::Pin pin);                     // Set the pin low and configure the pin as output
        void pinFloat(Gpio::Pin pin);                   // Configure the pin as input and set the pin high
    }
}

#endif //__ONEWIRE_H__
