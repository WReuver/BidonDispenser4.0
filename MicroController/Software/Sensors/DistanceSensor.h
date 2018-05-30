/* 
* DistanceSensor.h
*
* Created: 19-Mar-18 14:38:47
* Author: Robin C. Pel
*/

#ifndef __DISTANCESENSOR_H__
#define __DISTANCESENSOR_H__

#include "../includes.h"
#include "../Hardware/Gpio.h"
#include "../Hardware/GenericTC.h"

using namespace Hardware;

namespace Sensors 
{
	class DistanceSensor
	{
		// Variables
		public:
		protected:
		private:
		float buffer[16] = { 0.0 };                                                                                 // Buffer for the data
        Gpio::Pin* triggerPin;                                                                                      // Two trigger pins
        Gpio::Pin  echoPin;                                                                                         // One echo pin
		Gpio::Pin* multiplexPin;                                                                                    // Four multiplexer pins
        float emptyDistance;                                                                                        // If the distance is larger than "emptyDistance" the column will be seen as empty
        
		// Methods
		public:
        DistanceSensor(Gpio::Pin* triggerPin, Gpio::Pin echoPin, Gpio::Pin* multiplexPin, float emptyDistance);     // Default constructor
		~DistanceSensor() {};                                                                                       // Default destructor
		float* getData();                                                                                           // Get the data from the sensor
        uint8_t getSimpleData();                                                                                    // Get the data in a simpler form
        
		protected:
		private:
        void setMuxChannel(uint8_t channel);                                                                        // Set the specified multiplex channel
        void sendTtl(Gpio::Pin pin);                                                                                // Send a TTL pulse on the given pin
        uint16_t getPulseWidth(Gpio::Pin pin);                                                                      // Get the width of a pulse on the given pin
        float ticksToCentimeters(uint16_t prescval, uint16_t ticks);                                                // Convert the amount of ticks to centimeters
        float getDistance(uint8_t ColumnSectionNo);                                                                 // Get the distance for one specific column section
		
	}; //DistanceSensor
}

#endif //__DISTANCESENSOR_H__
