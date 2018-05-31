/* 
* SystemClock.h
*
* Created: 21-Mar-18 11:09:51
* Author: Robin C. Pel
*/

#ifndef __SYSTEMCLOCK_H__
#define __SYSTEMCLOCK_H__

// PLL_CLK [FREQUENCY]      // Uncomment this and set the correct frequency when using the PLL
// EXT_CLK [FREQUENCY]      // Uncomment this and set the correct frequency when using an external oscillator or clock
//#define F_CPU 2000000UL     // The default CPU frequency

#include "../includes.h"
#include "AtxMega.h"

namespace Hardware 
{
	namespace SystemClock
	{
		// System Clock Sources
		enum class Source 
		{
			RC2MHz,
			RC32MHz,
			RC32kHz,
			XOSC,
			PLL
		};
		
		// System Clock Prescaler A
		enum class PrescalerA
		{
			None		= 0b00000000,
			DivideBy2	= 0b00000100,
			DivideBy4	= 0b00001100,
			DivideBy8	= 0b00010100,
			DivideBy16	= 0b00011100,
			DivideBy32	= 0b00100100,
			DivideBy64	= 0b00101100,
			DivideBy128	= 0b00110100,
			DivideBy256	= 0b00111100,
			DivideBy512	= 0b01000100
		};
		
		// System Clock Prescaler B & C
		enum class PrescalerBC
		{
			None_None   = 0b00,
			None_Two    = 0b01,
			Four_None   = 0b10,
			Two_Two     = 0b11
		};
		
		// RTC Sources
		enum class SourceRtc
		{
			ULP		= 0b0000,
			TOSC	= 0b0010,
			RCOSC	= 0b0100,
			TOSC32	= 0b1010,
			RCOSC32	= 0b1100,
			EXTCLK	= 0b1110
		};
		
		
		// Functions
		void SetClockSource(Source source);                         // Set the preferred clock source as the CPU source
		void SetClockPrescaler(PrescalerA pa, PrescalerBC pbc);     // Set the preferred prescalers
		void SetClockPrescaler(PrescalerA pa);                      // Set the preferred A prescaler
		void SetClockPrescaler(PrescalerBC pbc);                    // Set the preferred B and C prescaler
		void LockClockSourceAndPrescaler();                         // Disable the possibility to change the CPU source and prescalers until a reset occurs
		void SetClockSourceRtc(SourceRtc sourceRtc);                // Set the preferred RTC source
		void EnableClockSourceRtc(SourceRtc sourceRtc);             // Enable the preferred RTC source
		void DisableClockSourceRtc(SourceRtc sourceRtc);            // Disable the preferred RTC source
		void EnableClockSource(Source source);                      // Enable the preferred CPU source
		bool IsClockSourceReady(Source source);                     // Check whether the CPU source is ready or not
		
        
		/* 
		* Functionalities not included:
		* - - - - - - - - - - - - - - - - - - - - - -
		* External Oscillator configuration
		* USB Clock configuration
		* PLL configuration
		* 32kHz Calibration configuration
		* DFLL configuration
		*/
		
		
	}
}

#endif //__SYSTEMCLOCK_H__
