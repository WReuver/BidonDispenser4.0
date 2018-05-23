/* 
* DistanceSensor.h
*
* Created: 19-Mar-18 14:38:47
* Author: Robin C. Pel
*/

#ifndef __DISTANCESENSOR_H__
#define __DISTANCESENSOR_H__

#include "Sensor.h"
#include "../Hardware/GenericTC.h"

namespace Sensors 
{
	class DistanceSensor : public Sensor
	{
		// Variables
		public:
		protected:
		private:
		float buffer = 0;                                               // Buffer for the data
		
		// Methods
		public:
        DistanceSensor(Hardware::Gpio::Pin* pins);                      // Default constructor
		~DistanceSensor();                                              // Default destructor
		virtual void* GetData();                                        // Get the data from the sensor

		protected:
		private:
        void sendTtl(Hardware::Gpio::Pin pin);                          // Send a TTL pulse on the given pin
        uint16_t getPulseWidth(Hardware::Gpio::Pin pin);                // Get the width of a pulse on the given pin
        float ticksToCentimeters(uint16_t prescval, uint16_t ticks);    // Convert the amount of ticks to centimeters
		
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