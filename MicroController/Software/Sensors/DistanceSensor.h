/* 
* DistanceSensor.h
*
* Created: 19-Mar-18 14:38:47
* Author: Robin C. Pel
*/

#ifndef __DISTANCESENSOR_H__
#define __DISTANCESENSOR_H__

#include "Sensor.h"
#include "../includes.h"
#include "../Hardware/TimerCounter.h"

namespace Sensors 
{
	class DistanceSensor : public Sensor
	{
		// Variables
		public:
		protected:
		private:
		float buffer = 0;
		
		// Methods
		public:
        DistanceSensor(Hardware::Gpio::Pin* pins);
		~DistanceSensor();
		virtual void* GetData();

		protected:
		private:
        void sendTtl(Hardware::Gpio::Pin pin);
        uint16_t getPulseWidth(Hardware::Gpio::Pin pin);
        float ticksToCentimeters(uint16_t prescval, uint16_t ticks);
		
	}; //DistanceSensor
}

#endif //__DISTANCESENSOR_H__

/** EXAMPLE
    
    #define TRIGGER     Gpio::Pin::D2
    #define ECHO        Gpio::Pin::D0
    
    Gpio::Pin pins[2] = { TRIGGER, ECHO };
    Sensors::DistanceSensor* ds = new Sensors::DistanceSensor(pins);
    volatile float dist = 0;
    
    float* pointer = (float*) ds->GetData();
    dist = *pointer;
*/