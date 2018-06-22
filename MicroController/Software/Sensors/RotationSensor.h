/* 
* RotationSensor.h
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel
*/

#ifndef __ROTATIONSENSOR_H__
#define __ROTATIONSENSOR_H__

#include "../includes.h"
#include "../Hardware/Gpio.h"

using namespace Hardware;

namespace Sensors
{
	class RotationSensor
	{
		// Variables
		public:
		protected:
		private:
		Gpio::Pin pins[8];                      // The pins the sensor is connected to
        
        
		// Methods
		public:
        RotationSensor(Gpio::Pin* sensorPins);  // Default constructor
		~RotationSensor() {};                   // Default destructor
		uint8_t getData();                      // Get the data from the sensor
		
		protected:
		private:
		
	}; //RotationSensor
}

#endif //__ROTATIONSENSOR_H__
