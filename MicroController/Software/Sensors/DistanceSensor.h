/* 
* DistanceSensor.h
*
* Created: 19-Mar-18 14:38:47
* Author: Robin C. Pel
*/

#ifndef __DISTANCESENSOR_H__
#define __DISTANCESENSOR_H__

#include "Sensor.h"

namespace Sensors 
{
	class DistanceSensor : public Sensor
	{
		// Variables
		public:
		protected:
		private:
		
		// Methods
		public:
		~DistanceSensor();
		virtual void* GetData();

		protected:
		private:
		
	}; //DistanceSensor
}

#endif //__DISTANCESENSOR_H__
