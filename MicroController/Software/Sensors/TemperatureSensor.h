/* 
* TemperatureSensor.h
*
* Created: 19-Mar-18 14:38:58
* Author: Robin C. Pel
*/

#ifndef __TEMPERATURESENSOR_H__
#define __TEMPERATURESENSOR_H__

#include "Sensor.h"

namespace Sensors
{
	class TemperatureSensor : public Sensor 
	{
		// Variables
		public:
		protected:
		private:

		// Methods
		public:
		TemperatureSensor();
		~TemperatureSensor();
		virtual bool initialize();
		virtual uint8_t* getData();
		
		protected:
		private:
		
	}; //TemperatureSensor
}

#endif //__TEMPERATURESENSOR_H__
