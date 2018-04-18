/* 
* GenericTC.cpp
*
* Created: 18-Apr-18 09:21:51
* Author: Robin C. Pel
*/

#include "GenericTC.h"

void Hardware::TimerCounter::InitializeGenericTC()
{
    SetClock(GTC::genericTC, ClockValue::Div1024);
}

void Hardware::TimerCounter::TerminateGenericTC()
{
    SetClock(GTC::genericTC, ClockValue::ClockOff);
}

Hardware::TimerCounter::TC Hardware::TimerCounter::GetGenericTC()
{
    return GTC::genericTC;
}

void Hardware::TimerCounter::SetGenericTC(TC tc)
{
    GTC::genericTC = tc;
}


