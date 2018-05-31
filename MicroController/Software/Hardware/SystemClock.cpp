/*
* SystemClock.cpp
*
* Created: 21-Mar-18 11:09:51
* Author: Robin C. Pel
*/

#include "SystemClock.h"

void Hardware::SystemClock::SetClockSource(Source source)
{
    EnableClockSource(source);														// Enable the clock source
    while (!IsClockSourceReady(source));											// Wait until the clock source is ready
    AtxMega::SetChangeProtectionMode(AtxMega::ChangeProtectionMode::IOREG);			// Set the Configuration Change Protection mode to "IOREG"
    CLK.CTRL = (uint8_t) source;													// Set the clock source
    //RedefineF_Cpu();
}

void Hardware::SystemClock::SetClockPrescaler(PrescalerA pa, PrescalerBC pbc)
{
    uint8_t prescReg = (uint8_t) pa | (uint8_t) pbc;                                // Combine the two prescaler enumerations into one
    AtxMega::SetChangeProtectionMode(AtxMega::ChangeProtectionMode::IOREG);			// Set the Configuration Change Protection mode to "IOREG"
    CLK.PSCTRL = prescReg;                                                          // Set the clock prescaler
    //RedefineF_Cpu();
}

void Hardware::SystemClock::SetClockPrescaler(PrescalerA pa)
{
    uint8_t prescReg = (CLK.PSCTRL & 0b10000011) | (uint8_t) pa;                    // Combine the new prescaler value with the not changing one
    AtxMega::SetChangeProtectionMode(AtxMega::ChangeProtectionMode::IOREG);			// Set the Configuration Change Protection mode to "IOREG"
    CLK.PSCTRL = prescReg;                                                          // Set the clock prescaler
    //RedefineF_Cpu();
}

void Hardware::SystemClock::SetClockPrescaler(PrescalerBC pbc)
{
    uint8_t prescReg = (CLK.PSCTRL & 0b11111100) | (uint8_t) pbc;                   // Combine the new prescaler value with the not changing one
    AtxMega::SetChangeProtectionMode(AtxMega::ChangeProtectionMode::IOREG);			// Set the Configuration Change Protection mode to "IOREG"
    CLK.PSCTRL = prescReg;                                                          // Set the clock prescaler
    //RedefineF_Cpu();
}

void Hardware::SystemClock::LockClockSourceAndPrescaler()
{
    CLK.LOCK = 1;			                                                        // Lock the clock and prescaler configuration until the micro controller is reset
}

void Hardware::SystemClock::SetClockSourceRtc(SourceRtc sourceRtc)
{
    CLK.RTCCTRL = (CLK.RTCCTRL & 0b0001) | (uint8_t) sourceRtc;                     // Configure the selected RTC as source
}

void Hardware::SystemClock::EnableClockSourceRtc(SourceRtc sourceRtc)
{
    CLK.RTCCTRL |= (uint8_t) sourceRtc;                                             // Enable the selected RTC
}

void Hardware::SystemClock::DisableClockSourceRtc(SourceRtc sourceRtc)
{
    CLK.RTCCTRL &= ~((uint8_t) sourceRtc);                                          // Disable the selected RTC
}

void Hardware::SystemClock::EnableClockSource(Source source)
{
    OSC.CTRL |= 1 << (uint8_t) source;					                            // Enable the selected clock source
}

bool Hardware::SystemClock::IsClockSourceReady(Source source)
{
    return OSC.STATUS & (1 << (uint8_t) source);		                            // Get the ready bit of the selected clock source
}
