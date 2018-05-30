/* 
* CoolingController.h
*
* Created: 19-Mar-18 15:27:08
* Author: Robin C. Pel
*/

#ifndef __COOLINGCONTROLLER_H__
#define __COOLINGCONTROLLER_H__

#include "../Sensors/TemperatureSensor.h"
#include "../Hardware/TimerCounter.h"

using namespace Hardware;
using namespace Sensors;

namespace Controllers
{
	class CoolingController
	{
		// Variables
		public:
		protected:
		private:
        TemperatureSensor* temperatureSensor[3];
        Gpio::Pin fanGroup[2];

		// Methods
		public:
		CoolingController(Gpio::Pin* temperatureSensors, Gpio::Pin* fanGroupPins);
        ~CoolingController() {};

		protected:
		private:

	}; //CoolingController
}

#endif //__COOLINGCONTROLLER_H__
