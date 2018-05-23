/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

// REMINDER: The distance sensor class needs to be re-tested, the default clock has been replaced by the generic clock

#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/Gpio.h"
#include "Hardware/GenericTC.h"

using namespace Hardware;

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Output);
}

int main()
{
    initialize();
    
    // Infinite loop
    while (1)
    {
        Gpio::TogglePinValue(Gpio::Pin::A0);
        _delay_ms(500);
    }
}
