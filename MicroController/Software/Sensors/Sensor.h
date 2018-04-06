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
#include "../Hardware/Communication/OneWire.h"

namespace Sensors
{
	class Sensor
	{
        // Variables
        public:
        protected:
        Hardware::Gpio::Pin* pins;
        private:
        
        
		// Methods
		public:
        Sensor(Hardware::Gpio::Pin* pins) 
        {
            this->pins = pins;
        }
        
        /* Returns a pointer to the data */
		virtual void* GetData() = 0;
        
        /* Basic Destructor */
		virtual ~Sensor() {};
        
        protected:
        private:
		
	}; //Sensor
}

#endif //__SENSOR_H__
