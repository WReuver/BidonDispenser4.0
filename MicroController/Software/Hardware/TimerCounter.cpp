/* 
* TimerCounter.cpp
*
* Created: 09-Apr-18 11:14:17
* Author: Robin C. Pel
*/

#include "TimerCounter.h"

uint8_t Hardware::TimerCounter::SetPrescaler(TimerCounter tc, PrescalerValue prescval)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 1:     ((TC1_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 2:     ((TC2_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        default:    return 0;
    }
}

uint8_t Hardware::TimerCounter::SetPeriod(TimerCounter tc, uint16_t period)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->PER = period;
                    return 1;
        case 1:     ((TC1_t*) tcPointer)->PER = period;
                    return 1;
        case 2:     ((TC2_t*) tcPointer)->HPER = period >> 8;
                    ((TC2_t*) tcPointer)->LPER = period & 0xFF;
                    return 1;
        default:    return 0;
    }
}

uint8_t Hardware::TimerCounter::EnableOnPin(TimerCounter tc, Gpio::Pin pin)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 1:     ((TC1_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 2:     ((TC2_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        default:    return 0;
    }
}

uint8_t Hardware::TimerCounter::DisbaleOnPin(TimerCounter tc, Gpio::Pin pin)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 1:     ((TC1_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 2:     ((TC2_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        default:    return 0;
    }
}

uint8_t Hardware::TimerCounter::SetWaveformGenMode(TimerCounter tc, WaveformGenMode wgm)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 1:     ((TC1_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 2:     ((TC2_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        default:    return 0;
    }
}

uint8_t Hardware::TimerCounter::SetDutyCycle(TimerCounter tc, uint8_t dutyCycle)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 1:     ((TC1_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        case 2:     ((TC2_t*) tcPointer)->CTRLA = (uint8_t) prescval;   return 1;
        default:    return 0;
    }
}

uint16_t Hardware::TimerCounter::GetCount(TimerCounter tc)
{
    void* tcPointer = getTcAddress(tc);
    
    switch ( getTcNumber(tc) )
    {
        case 0:     return ((TC0_t*) tcPointer)->CNT;
        case 1:     return ((TC1_t*) tcPointer)->CNT;
        case 2:     return ( (((TC2_t*) tcPointer)->HCNT << 8) | ((TC2_t*) tcPointer)->LCNT );
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::getTcNumber(TimerCounter tc)
{
    switch ( (TC) ( ((uint8_t) tc >> 3) & 0b111) )
    {
        case TC::TC0: return 0;
        case TC::TC1: return 1;
        case TC::TC2: return 2;
        default:      return 255;
    }
}

void* Hardware::TimerCounter::getTcAddress(TimerCounter tc)
{
    switch ( tc )
    {
        case TimerCounter::TC0C:    return (void*) &TCC0;
        case TimerCounter::TC0D:    return (void*) &TCD0;
        case TimerCounter::TC0E:    return (void*) &TCE0;
        case TimerCounter::TC0F:    return (void*) &TCF0;
        case TimerCounter::TC1C:    return (void*) &TCC1;
        case TimerCounter::TC1D:    return (void*) &TCD1;
        case TimerCounter::TC1E:    return (void*) &TCE1;
        case TimerCounter::TC2C:    return (void*) &TCC2;
        case TimerCounter::TC2D:    return (void*) &TCD2;
        case TimerCounter::TC2E:    return (void*) &TCE2;
        case TimerCounter::TC2F:    return (void*) &TCF2;
        default:                    return nullptr;
    }
}
