/* 
* includes.h
*
* Created: 06-Apr-18 11:13:29
* Author: Robin C. Pel
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

/* Standard stuff */
#include <stdlib.h>
#include <stdint.h>

void* operator new(size_t objsize);
void operator delete(void* obj);


/* Delay stuff */
#define F_CPU 32000000UL
#include <util/delay.h>


/* I/O */
#include "Hardware/Gpio.h"


/* AtxMega */
#include "Hardware/AtxMega.h"


/* Conversions */
uint32_t FloatToUint32(float data);
float Uint32ToFloat(uint32_t data);

#endif //__INCLUDES_H__
