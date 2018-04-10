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
        enum class TcNo
        {
            TC0, 
            TC1
        };
        
        // Timer/Counter Mapping
        enum class TC
        {
            TC0C = ((uint8_t) TcNo::TC0 << 3) + (uint8_t) Gpio::Port::PortC,
            TC0D = ((uint8_t) TcNo::TC0 << 3) + (uint8_t) Gpio::Port::PortD,
            TC0E = ((uint8_t) TcNo::TC0 << 3) + (uint8_t) Gpio::Port::PortE,
            TC0F = ((uint8_t) TcNo::TC0 << 3) + (uint8_t) Gpio::Port::PortF,
            TC1C = ((uint8_t) TcNo::TC1 << 3) + (uint8_t) Gpio::Port::PortC,
            TC1D = ((uint8_t) TcNo::TC1 << 3) + (uint8_t) Gpio::Port::PortD,
            TC1E = ((uint8_t) TcNo::TC1 << 3) + (uint8_t) Gpio::Port::PortE
        };
        
        // Prescaler Values
        enum class ClockValue
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
        uint8_t SetClock(TC tc, ClockValue prescval);
        uint8_t SetPeriod(TC tc, uint16_t period);
        uint8_t EnableOnPin(TC tc, Gpio::PinNo pinNo);
        uint8_t DisbaleOnPin(TC tc, Gpio::PinNo pinNo);
        uint8_t SetWaveformGenMode(TC tc, WaveformGenMode wgm);
        uint8_t SetDutyCycleOnPin(TC tc, uint8_t dutyCycle, Gpio::PinNo pinNo);
        uint16_t GetCount(TC tc);
        
        uint8_t GetTcNumber(TC tc);
        void* GetTcAddress(TC tc);
        Gpio::Port GetPort(TC tc);
        
        
        
        /* 
		* Functionalities not included:
		* - - - - - - - - - - - - - - - - - - - - - -
        * Timer/Counter 2
        * 
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
