/*
 * ledController.h
 *
 * Created: 30-May-18 22:07:39
 *  Author: Robin C. Pel
 */ 

#ifndef LEDCONTROLLER_H_
#define LEDCONTROLLER_H_

#include "../Hardware/Gpio.h"

class ledController
{
    public:
    
    #ifdef GREENLED
    
    static void runningStart() {
        Hardware::Gpio::SetPinValue(GREENLED, Value::High);
    }
    
    static void runningStop() {
        Hardware::Gpio::SetPinValue(GREENLED, Value::Low);
    }
    
    #endif
    
    
    #ifdef YELLOWLED
    
    static void busyStart() {
        Hardware::Gpio::SetPinValue(YELLOWLED, Value::High);
    }
    
    static void busyStop() {
        Hardware::Gpio::SetPinValue(YELLOWLED, Value::Low);
    }
    
    #endif
    
    
    #ifdef REDLED
    
    static void errorStart() {
        Hardware::Gpio::SetPinValue(REDLED, Value::High);
    }
    
    static void errorStop() {
        Hardware::Gpio::SetPinValue(REDLED, Value::Low);
    }
    
    #endif
};

#endif /* LEDCONTROLLER_H_ */