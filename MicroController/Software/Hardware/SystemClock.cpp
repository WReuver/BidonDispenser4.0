/* 
* SystemClock.cpp
*
* Created: 21-Mar-18 11:09:51
* Author: Robin C. Pel
*/

#include "SystemClock.h"

void Hardware::SystemClock::SetClockSource(Source source)
{
	EnableClockSource(source);
	while (!IsClockSourceReady(source));
	AtxMega::SetChangeProtectionMode(AtxMega::ChangeProtectionMode::IOREG);
	CLK.CTRL = (uint8_t) source;
}

void Hardware::SystemClock::SetClockPrescaler(PrescalerA pa, PrescalerBC pbc)
{
	
}

void Hardware::SystemClock::LockClockSourceAndPrescaler()
{
	CLK.LOCK = 1;
}

void Hardware::SystemClock::SetClockSourceRtc(SourceRtc sourceRtc)
{
	
}

void Hardware::SystemClock::EnableClockSourceRtc()
{
	
}

void Hardware::SystemClock::EnableClockSource(Source source)
{
	OSC.CTRL |= 1 << (uint8_t) source;					// Enable the selected clock source
}

bool Hardware::SystemClock::IsClockSourceReady(Source source)
{
	return OSC.STATUS & (1 << (uint8_t) source);		// Get the ready bit of the selected clock source
}
