/* 
* OneWire.h
*
* Created: 04-Apr-18 09:20:35
* Author: Robin C. Pel
*/

#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include "../../includes.h"

namespace Hardware
{
    namespace Communication
    {
        namespace OneWire
        {
            // Functions
            uint8_t Initialize(Gpio::Pin pin);
            void WriteBit(Gpio::Pin pin, uint8_t bit);
            uint8_t ReadBit(Gpio::Pin pin);
            void Write(Gpio::Pin pin, uint8_t byte);
            uint8_t Read(Gpio::Pin pin);
            
            void pinLow(Gpio::Pin pin);
            void pinFloat(Gpio::Pin pin);
        }
    }
}

#endif //__ONEWIRE_H__
