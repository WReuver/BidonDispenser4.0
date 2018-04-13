/* 
* RotationSensor.h
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel
*/

#ifndef __ROTATIONSENSOR_H__
#define __ROTATIONSENSOR_H__

#include "Sensor.h"
#include "../includes.h"

namespace Sensors
{
	class RotationSensor : public Sensor
	{
		// Variables
		public:
		protected:
		private:
		bool buffer = 0;
        
		// Methods
		public:
        RotationSensor(Hardware::Gpio::Pin* pins);
		~RotationSensor();
		virtual void* GetData();
		
		protected:
		private:
		
	}; //RotationSensor
}

#endif //__ROTATIONSENSOR_H__
