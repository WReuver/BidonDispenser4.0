/* 
* TimerCounter.h
*
* Created: 09-Apr-18 11:14:18
* Author: Robin C. Pel
*/

#ifndef __TIMERCOUNTER_H__
#define __TIMERCOUNTER_H__

#include "../includes.h"

namespace Hardware
{
    namespace TimerCounter
    {
        // Timer/Counters
        enum class TC
        {
            TC0, 
            TC1, 
            TC2
        };
        
        // Timer/Counter Mapping
        enum class TimerCounter
        {
            TC0C = ((uint8_t) TC::TC0 << 3) + (uint8_t) Gpio::Port::PortC,
            TC0D = ((uint8_t) TC::TC0 << 3) + (uint8_t) Gpio::Port::PortD,
            TC0E = ((uint8_t) TC::TC0 << 3) + (uint8_t) Gpio::Port::PortE,
            TC0F = ((uint8_t) TC::TC0 << 3) + (uint8_t) Gpio::Port::PortF,
            TC1C = ((uint8_t) TC::TC1 << 3) + (uint8_t) Gpio::Port::PortC,
            TC1D = ((uint8_t) TC::TC1 << 3) + (uint8_t) Gpio::Port::PortD,
            TC1E = ((uint8_t) TC::TC1 << 3) + (uint8_t) Gpio::Port::PortE,
            TC2C = ((uint8_t) TC::TC2 << 3) + (uint8_t) Gpio::Port::PortC,
            TC2D = ((uint8_t) TC::TC2 << 3) + (uint8_t) Gpio::Port::PortD,
            TC2E = ((uint8_t) TC::TC2 << 3) + (uint8_t) Gpio::Port::PortE,
            TC2F = ((uint8_t) TC::TC2 << 3) + (uint8_t) Gpio::Port::PortF
        };
        
        // Prescaler Values
        enum class PrescalerValue
        {
            ClockOff,
            Div1,
            Div2,
            Div4,
            Div8,
            Div64,
            Div256,
            Div1024
        };
        
        // Waveform Generation Mode
        enum class WaveformGenMode
        {
            Normal              = 0b000,
            Frq                 = 0b001,
            SingleSlope         = 0b011,
            DualSlopeTop        = 0b101,
            DualSlopeBoth       = 0b110,
            DualSlopeBottom     = 0b111
        };
        
        
        
        // Functions
        // ClockFreq / TargetFreq = Presc * (Period + 1)    32M / 60k = 1 * (532 + 1)
        uint8_t SetPrescaler(TimerCounter tc, PrescalerValue prescval);
        uint8_t SetPeriod(TimerCounter tc, uint16_t period);
        uint8_t EnableOnPin(TimerCounter tc, Gpio::Pin pin);
        uint8_t DisbaleOnPin(TimerCounter tc, Gpio::Pin pin);
        uint8_t SetWaveformGenMode(TimerCounter tc, WaveformGenMode wgm);
        uint8_t SetDutyCycle(TimerCounter tc, uint8_t dutyCycle);
        uint16_t GetCount(TimerCounter tc);
        
        uint8_t getTcNumber(TimerCounter tc);
        void* getTcAddress(TimerCounter tc);
        
        
        
        /* 
		* Functionalities not included:
		* - - - - - - - - - - - - - - - - - - - - - -
		* Anything to do with "Control Register C"
		* Anything to do with "Control Register D"
		* Anything to do with "Control Register E"
		* Anything to do with "Interrupt Enable Register A"
		* Anything to do with "Interrupt Enable Register A"
		* Anything to do with "Control Register F Clear/Set"
		* Anything to do with "Control Register G Clear/Set"
		* Anything to do with "Interrupt Flag Register"
		*
		*/
        
        
        
    }
}

#endif //__TIMERCOUNTER_H__
