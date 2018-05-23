/* 
* RotationSensor.h
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel
*/

#ifndef __ROTATIONSENSOR_H__
#define __ROTATIONSENSOR_H__

#include "Sensor.h"

namespace Sensors
{
	class RotationSensor : public Sensor
	{
		// Variables
		public:
		protected:
		private:
		bool buffer = 0;                                    // Buffer for the data
        
		// Methods
		public:
        RotationSensor(Hardware::Gpio::Pin* pins);          // Default constructor
		~RotationSensor();                                  // Default destructor
		virtual void* GetData();                            // Get the data from the sensor
		
		protected:
		private:
		
	}; //RotationSensor
}

#endif //__ROTATIONSENSOR_H__
