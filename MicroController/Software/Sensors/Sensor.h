/* 
* Sensor.h
*
* Created: 19-Mar-18 14:18:05
* Author: Robin C. Pel
*/

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "../includes.h"
#include "../Hardware/Gpio.h"

namespace Sensors
{
	class Sensor
	{
        // Variables
        public:
        protected:
        Hardware::Gpio::Pin* pins;                  // The pins the sensor is connected to
        private:
        
        
		// Methods
		public:
        Sensor(Hardware::Gpio::Pin* pins)           // Default constructor
        {
            this->pins = pins;
        }
        
        /* Returns a pointer to the data */
		virtual void* GetData() = 0;                // Get the data from the sensor
        
        /* Basic Destructor */
		virtual ~Sensor() {};                       // Default destructor
        
        protected:
        private:
		
	}; //Sensor
}

#endif //__SENSOR_H__
