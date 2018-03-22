/* 
* AtxMega.cpp
*
* Created: 21-Mar-18 14:48:54
* Author: Robin C. Pel
*/

#include "AtxMega.h"

void Hardware::AtxMega::SetChangeProtectionMode(ChangeProtectionMode mode)
{
	CCP = (uint8_t) mode;
}
