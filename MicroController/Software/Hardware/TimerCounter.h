/* 
* TimerCounter.h
*
* Created: 09-Apr-18 11:14:18
* Author: Robin C. Pel
*/

#ifndef __TIMERCOUNTER_H__
#define __TIMERCOUNTER_H__

#include "../includes.h"
#include "Gpio.h"

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
        uint8_t SetClock(TC tc, ClockValue prescval);                               // Turn on the clock for the chosen timer/counter and select a prescaler
        uint8_t SetPeriod(TC tc, uint16_t period);                                  // Set the period for the chosen timer/counter
        uint8_t EnableOnPin(TC tc, Gpio::PinNo pinNo);                              // Select the pin which will be taken over by the chosen timer/counter
        uint8_t DisbaleOnPin(TC tc, Gpio::PinNo pinNo);                             // Select the pin which will be released by the chosen timer/counter
        uint8_t SetWaveformGenMode(TC tc, WaveformGenMode wgm);                     // Select which waveform generation mode will be used on the chosen timer/counter
        uint8_t SetDutyCycleOnPin(TC tc, uint8_t dutyCycle, Gpio::PinNo pinNo);     // Select what duty cycle the timer/counter has to be on a certain pin, the duty cycle is defined in percentages (10, 50, etc.)
        uint16_t GetCount(TC tc);                                                   // Returns the amount of ticks passed on the given timer/counter
        uint8_t ClearCount(TC tc);                                                  // Clear the count of the chosen timer/counter
        uint8_t GetTcNumber(TC tc);                                                 // Returns whether the timer counter is of type 0 or 1
        uint8_t DidTcOverflow(TC tc);                                               // Check whether the overflow interrupt flag is set or not, calling this function also resets said flag
        void* GetTcAddress(TC tc);                                                  // Returns a void pointer to the address of the given timer/counter
        Gpio::Port GetPort(TC tc);                                                  // Returns the port which is connected to the given timer/counter
        float TicksToMicoSeconds(uint16_t prescval, uint16_t ticks);                // Converts ticks to microseconds
        float TicksToMilliSeconds(uint16_t prescval, uint16_t ticks);               // Converts ticks to milliseconds
        float TicksToSeconds(uint16_t prescval, uint16_t ticks);                    // Converts ticks to seconds
        float MicroSecondsToTicks(uint16_t prescval, uint32_t microSeconds);        // Converts microseconds to ticks
        float MilliSecondsToTicks(uint16_t prescval, uint32_t milliSeconds);        // Converts milliseconds to ticks
        float SecondsToTicks(uint16_t prescval, uint16_t seconds);                  // Converts seconds to ticks
        
        
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

/****************************************************************************************\
 * EXAMPLE: Counter of 0.5Hz
 *  Counts up for one second.
 *  Toggles the LED on pin B0.
 *  ==> Repeat
|****************************************************************************************|
    
    // Set the LED pin to Output mode
    Gpio::SetPinDirection(Gpio::Pin::B0, Gpio::Dir::Output);
    
    // Start the clock for TC0D and set the prescaler to 1.024
    TimerCounter::SetClock(TimerCounter::TC::TC0D, TimerCounter::ClockValue::Div1024);
    
    // Set the period to 31.250   (Source Clock / Prescaler = 31.250)
    TimerCounter::SetPeriod(TimerCounter::TC::TC0D, 31250);
    
    // Infinite Loop
    while (1)
    {
        // Toggle The LED on pin B0
        Gpio::TogglePinValue(Gpio::Pin::B0);
        // Wait until one second has passed
        while (TCD0.CNT < 0x7A12);
    }

\***************************************************************************************/



/****************************************************************************************\
 * EXAMPLE: PWM of ?60.037Hz with a duty cycle of 50%
 *  Let's a stepper driver spin a motor 180 degrees.
 *  Waits for five seconds.
 *  ==> Repeat
|****************************************************************************************|

    // Set the IR sensor pin to Input mode
    Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Input);
    
    // Start the clock for TC0C and set the prescaler to 1
    TimerCounter::SetClock(TimerCounter::TC::TC0C, TimerCounter::ClockValue::Div1);
    
    // Set the waveform generation mode to Single slope PWM
    TimerCounter::SetWaveformGenMode(TimerCounter::TC::TC0C, TimerCounter::WaveformGenMode::SingleSlope);
    
    // Enable the TC signal on Pin C0
    TimerCounter::EnableOnPin(TimerCounter::TC::TC0C, Gpio::PinNo::Pin0);
    
    // Set the period to 532   (Source Clock / (Prescaler * (Period + 1)) = 60.037)
    TimerCounter::SetPeriod(TimerCounter::TC::TC0C, 532);
    
    // Infinite Loop
    while (1)
    {
        // Set the duty cycle on pin C0 to 50%
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0C, 50, Gpio::PinNo::Pin0);
        // Wait a little bit so the IR sensor is passed by the gap
        _delay_ms(500);
        // Wait until the next gap is seen by the IR sensor
        while (Gpio::GetPinValue(Gpio::Pin::A0) == Gpio::Value::Low);
        // Set the duty cycle on pin C0 to 00%
        TimerCounter::SetDutyCycleOnPin(TimerCounter::TC::TC0C, 0, Gpio::PinNo::Pin0);
        // Wait a bit
        _delay_ms(5000);
    }

\***************************************************************************************/

#endif //__TIMERCOUNTER_H__
