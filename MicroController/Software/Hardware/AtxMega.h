/* 
* AtxMega.h
*
* Created: 21-Mar-18 14:48:54
* Author: Robin C. Pel
*/

#ifndef __ATXMEGA_H__
#define __ATXMEGA_H__

#include <stdint.h>
#include <avr/io.h>

namespace Hardware
{
	namespace AtxMega
	{
        // The Change Protection Modes
		enum class ChangeProtectionMode
		{
			SPM		= 0x9D,
			IOREG	= 0xD8
		};
		
		void SetChangeProtectionMode(ChangeProtectionMode mode);        // Set the change protection mode to the given mode
		
		
	}
}

#endif //__ATXMEGA_H__
