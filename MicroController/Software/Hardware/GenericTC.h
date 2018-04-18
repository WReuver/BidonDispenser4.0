/* 
* GenericTC.h
*
* Created: 18-Apr-18 09:21:51
* Author: Robin C. Pel
*/

#ifndef __GENERICTC_H__
#define __GENERICTC_H__

#include "TimerCounter.h"

namespace Hardware
{
    namespace TimerCounter
    {
        namespace GTC
        {
            static TimerCounter::TC genericTC = Hardware::TimerCounter::TC::TC0F;
        }
        
        // Initialize the Generic TC with a prescaler of 1024
        void InitializeGenericTC();
        
        // Stops the current Generic TC and resets it's configuration
        void TerminateGenericTC();
        
        // Get the Generic TC
        TC GetGenericTC();
        
        // Set the Generic TC, if the previous Generic TC was initialized it should be terminated first
        void SetGenericTC(TC tc);
    }
}

#endif //__GENERICTC_H__
