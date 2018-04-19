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
            // A variable where the current Timer Counter - which is being used as the Generic TC - is stored
            volatile static TimerCounter::TC genericTC = Hardware::TimerCounter::TC::TC0F;
        }
        
        void InitializeGenericTC();     // Initialize the Generic TC with a prescaler of 1024
        void TerminateGenericTC();      // Stops the current Generic TC and resets it's configuration
        TC GetGenericTC();              // Get the Generic TC
        void SetGenericTC(TC tc);       // Set the Generic TC, if the previous Generic TC was initialized it should be terminated first
    }
}

#endif //__GENERICTC_H__
