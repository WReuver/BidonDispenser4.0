/* 
* Sensor.h
*
* Created: 19-Mar-18 14:18:05
* Author: Robin C. Pel
*/

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <stdint.h>

namespace Sensors
{
	class Sensor
	{
		// Methods
		public:
		virtual bool initialize() = 0;		// Initialize the sensor (if needed)
		virtual uint8_t* getData() = 0;		// Returns a pointer to the data, the first byte contains the size of the data (in bytes), the bytes after that are the data
		virtual ~Sensor();					// Basic destructor
		
	}; //Sensor
}

#endif //__SENSOR_H__
