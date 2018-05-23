/* 
* TimerCounter.cpp
*
* Created: 09-Apr-18 11:14:17
* Author: Robin C. Pel
*/

#include "TimerCounter.h"

uint8_t Hardware::TimerCounter::SetClock(TC tc, ClockValue clockValue)
{
    void* tcPointer = GetTcAddress(tc);
    
    switch ( GetTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CTRLA = (uint8_t) clockValue;   return 0;
        case 1:     ((TC1_t*) tcPointer)->CTRLA = (uint8_t) clockValue;   return 0;
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::SetPeriod(TC tc, uint16_t period)
{
    void* tcPointer = GetTcAddress(tc);
    uint8_t tcNumber = GetTcNumber(tc);
    
    switch ( tcNumber )
    {
        // Assign the new period
        case 0:     ((TC0_t*) tcPointer)->PER = period; break;
        case 1:     ((TC1_t*) tcPointer)->PER = period; break;
        default:    return 1;
    }
    
    return 0;
}

uint8_t Hardware::TimerCounter::EnableOnPin(TC tc, Gpio::PinNo pinNo)
{
    if ((uint8_t) pinNo > 3) return 1;      // Quick check whether the pin has TC capabilities or not
    void* tcPointer = GetTcAddress(tc);     // Get a pointer to the correct TC
    
    Gpio::Port port = GetPort(tc);
    Gpio::Pin pin = Gpio::GetPinFromPortAndPinNo(port, pinNo);
    Gpio::SetPinDirection(pin, Gpio::Dir::Output);
    
    switch ( GetTcNumber(tc) )
    {
        // Set the specified bit
        case 0:     ((TC0_t*) tcPointer)->CTRLB |= 1 << (((uint8_t) pinNo) + 4);   return 0;
        case 1:     ((TC1_t*) tcPointer)->CTRLB |= 1 << (((uint8_t) pinNo) + 4);   return 0;
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::DisbaleOnPin(TC tc, Gpio::PinNo pinNo)
{
    if ((uint8_t) pinNo > 3) return 1;      // Quick check whether the pin has TC capabilities or not
    void* tcPointer = GetTcAddress(tc);     // Get a pointer to the correct TC
    
    switch ( GetTcNumber(tc) )
    {
        // Reset the specified bit
        case 0:     ((TC0_t*) tcPointer)->CTRLB &= ~(1 << (((uint8_t) pinNo) + 4));   return 0;
        case 1:     ((TC1_t*) tcPointer)->CTRLB &= ~(1 << (((uint8_t) pinNo) + 4));   return 0;
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::SetWaveformGenMode(TC tc, WaveformGenMode wgm)
{
    void* tcPointer = GetTcAddress(tc);
    
    switch ( GetTcNumber(tc) )
    {
        //           Clear the Waveform Generation Mode bits     Configure the chosen Waveform Generation Mode
        case 0:     ((TC0_t*) tcPointer)->CTRLB &= 0b11110000;  ((TC0_t*) tcPointer)->CTRLB = (uint8_t) wgm;    return 0;
        case 1:     ((TC1_t*) tcPointer)->CTRLB &= 0b11110000;  ((TC1_t*) tcPointer)->CTRLB = (uint8_t) wgm;    return 0;
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::SetDutyCycleOnPin(TC tc, uint8_t dutyCycle, Gpio::PinNo pinNo)
{
    void* tcPointer = GetTcAddress(tc);
    uint8_t tcNumber = GetTcNumber(tc);
    uint16_t period = 0;
    uint16_t compareCapture = 0;
    
    switch ( tcNumber )
    {
        case 0:     period = ((TC0_t*) tcPointer)->PER; break;
        case 1:     period = ((TC1_t*) tcPointer)->PER; break;
        default:    return 1;
    }
    
    compareCapture = ( period * dutyCycle ) / 100;
    
    if (tcNumber == 0)
    {
        switch (pinNo)
        {
            case Gpio::PinNo::Pin0: ((TC0_t*) tcPointer)->CCA = compareCapture; return 0;
            case Gpio::PinNo::Pin1: ((TC0_t*) tcPointer)->CCB = compareCapture; return 0;
            case Gpio::PinNo::Pin2: ((TC0_t*) tcPointer)->CCC = compareCapture; return 0;
            case Gpio::PinNo::Pin3: ((TC0_t*) tcPointer)->CCD = compareCapture; return 0;
            default: return 1;
        }
    }
    else if (tcNumber == 1)
    {
        switch (pinNo)
        {
            case Gpio::PinNo::Pin0: ((TC1_t*) tcPointer)->CCA = compareCapture; return 0;
            case Gpio::PinNo::Pin1: ((TC1_t*) tcPointer)->CCB = compareCapture; return 0;
            default: return 1;
        }
    }
    else 
        return 1;
}

uint16_t Hardware::TimerCounter::GetCount(TC tc)
{
    void* tcPointer = GetTcAddress(tc);
    
    switch ( GetTcNumber(tc) )
    {
        case 0:     return ((TC0_t*) tcPointer)->CNT;
        case 1:     return ((TC1_t*) tcPointer)->CNT;
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::ClearCount(TC tc)
{
    void* tcPointer = GetTcAddress(tc);
    
    switch ( GetTcNumber(tc) )
    {
        case 0:     ((TC0_t*) tcPointer)->CNT = 0; return 0;
        case 1:     ((TC1_t*) tcPointer)->CNT = 0; return 0;
        default:    return 1;
    }
}

uint8_t Hardware::TimerCounter::GetTcNumber(TC tc)
{
    switch ( (TcNo) ( ((uint8_t) tc >> 3) & 0b111) )
    {
        case TcNo::TC0: return 0;
        case TcNo::TC1: return 1;
        default:        return 255;
    }
}

uint8_t Hardware::TimerCounter::DidTcOverflow(TC tc)
{
    void* tcPointer = GetTcAddress(tc);
    uint8_t tcNumber = GetTcNumber(tc);
    uint8_t didOVerflow = 0;
    
    switch ( tcNumber )
    {
        // Assign the new period
        case 0:
            didOVerflow = ((TC0_t*) tcPointer)->INTFLAGS & 0b00000001;
            if (didOVerflow == 1) ((TC0_t*) tcPointer)->INTFLAGS |= 0b00000001;
            break;
            
        case 1:
            didOVerflow = ((TC1_t*) tcPointer)->INTFLAGS & 0b00000001;
            if (didOVerflow == 1) ((TC1_t*) tcPointer)->INTFLAGS |= 0b00000001;
            break;
            
        default:
            return 0;
    }
    
    return didOVerflow;
}

void* Hardware::TimerCounter::GetTcAddress(TC tc)
{
    switch ( tc )
    {
        case TC::TC0C:    return (void*) &TCC0;
        case TC::TC0D:    return (void*) &TCD0;
        case TC::TC0E:    return (void*) &TCE0;
        case TC::TC0F:    return (void*) &TCF0;
        case TC::TC1C:    return (void*) &TCC1;
        case TC::TC1D:    return (void*) &TCD1;
        case TC::TC1E:    return (void*) &TCE1;
        default:          return nullptr;
    }
}

Hardware::Gpio::Port Hardware::TimerCounter::GetPort(TC tc)
{
    switch ( tc )
    {
        case TC::TC0C:    return Gpio::Port::PortC;
        case TC::TC0D:    return Gpio::Port::PortD;
        case TC::TC0E:    return Gpio::Port::PortE;
        case TC::TC0F:    return Gpio::Port::PortF;
        case TC::TC1C:    return Gpio::Port::PortC;
        case TC::TC1D:    return Gpio::Port::PortD;
        case TC::TC1E:    return Gpio::Port::PortE;
        default:          return Gpio::Port::PortC;
    }
}

float Hardware::TimerCounter::TicksToMicoSeconds(uint16_t prescval, uint16_t ticks)
{
    float secondsPerTick = 1.0 / (F_CPU / prescval);
    float microsecondsPerTick = secondsPerTick * 1000000;
    return microsecondsPerTick * ticks;
}

float Hardware::TimerCounter::TicksToMilliSeconds(uint16_t prescval, uint16_t ticks)
{
    return TicksToMicoSeconds(prescval, ticks) / 1000;
}

float Hardware::TimerCounter::TicksToSeconds(uint16_t prescval, uint16_t ticks)
{
    return TicksToMilliSeconds(prescval, ticks) / 1000;
}

float Hardware::TimerCounter::MicroSecondsToTicks(uint16_t prescval, uint32_t microSeconds)
{
    volatile float ticksPerSecond = F_CPU / prescval;
    volatile float ticksPerMicroSeconds = ticksPerSecond / 1000000;
    return ticksPerMicroSeconds * microSeconds;
}

float Hardware::TimerCounter::MilliSecondsToTicks(uint16_t prescval, uint32_t milliSeconds)
{
    return MicroSecondsToTicks(prescval, milliSeconds*1000);
}

float Hardware::TimerCounter::SecondsToTicks(uint16_t prescval, uint16_t seconds)
{
    return MilliSecondsToTicks(prescval, seconds*1000);
}
