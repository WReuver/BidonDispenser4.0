/* 
* includes.cpp
*
* Created: 06-Apr-18 11:13:29
* Author: Robin C. Pel
*/

#include "includes.h"

void* operator new(size_t objsize)
{
    return malloc(objsize);
}

void operator delete(void* obj)
{
    free(obj);
}

uint32_t FloatToUint32(float data)
{
    return (uint32_t)(*(uint32_t*)&data);
}

float Uint32ToFloat(uint32_t data)
{
    return (float)(*(float*)&data);
}