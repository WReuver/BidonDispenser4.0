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
			None		= 0b00000,
			DivideBy2	= 0b00001,
			DivideBy4	= 0b00011,
			DivideBy8	= 0b00101,
			DivideBy16	= 0b00111,
			DivideBy32	= 0b01001,
			DivideBy64	= 0b01011,
			DivideBy128	= 0b01101,
			DivideBy256	= 0b01111,
			DivideBy512	= 0b10001
		};
		
		// System Clock Prescaler B & C
		enum class PrescalerBC
		{
			None_None,
			None_Two,
			Four_None,
			Two_Two
		};
		
		// RTC Sources
		enum class SourceRtc
		{
			ULP		= 0b000,
			TOSC	= 0b001,
			RCOSC	= 0b010,
			TOSC32	= 0b101,
			RCOSC32	= 0b110,
			EXTCLK	= 0b111
		};
		
		
		// Functions
		void SetClockSource(Source source);
		void SetClockPrescaler(PrescalerA pa, PrescalerBC pbc);
		void LockClockSourceAndPrescaler();
		void SetClockSourceRtc(SourceRtc sourceRtc);
		void EnableClockSourceRtc();
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
