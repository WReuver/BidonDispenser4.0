/* 
* SystemClock.h
*
* Created: 21-Mar-18 11:09:51
* Author: Robin C. Pel
*/

#ifndef __SYSTEMCLOCK_H__
#define __SYSTEMCLOCK_H__

#include <stdint.h>
#include <avr/io.h>
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
			None		= 0b0000000,
			DivideBy2	= 0b0000100,
			DivideBy4	= 0b0001100,
			DivideBy8	= 0b0010100,
			DivideBy16	= 0b0011100,
			DivideBy32	= 0b0100100,
			DivideBy64	= 0b0101100,
			DivideBy128	= 0b0110100,
			DivideBy256	= 0b0111100,
			DivideBy512	= 0b1000100
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
		void SetClockSource(Source source);
		void SetClockPrescaler(PrescalerA pa, PrescalerBC pbc);
		void SetClockPrescaler(PrescalerA pa);
		void SetClockPrescaler(PrescalerBC pbc);
		void LockClockSourceAndPrescaler();
		void SetClockSourceRtc(SourceRtc sourceRtc);
		void EnableClockSourceRtc();
		void DisableClockSourceRtc();
		void EnableClockSource(Source source);
		bool IsClockSourceReady(Source source);
		
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
