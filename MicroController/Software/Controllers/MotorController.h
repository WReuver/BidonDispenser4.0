/* 
* MotorController.h
*
* Created: 19-Mar-18 15:24:44
* Author: Robin C. Pel
*/

#ifndef __MOTORCONTROLLER_H__
#define __MOTORCONTROLLER_H__

#include "../Sensors/RotationSensor.h"
#include "../Hardware/TimerCounter.h"

using namespace Hardware;
using namespace Sensors;
using namespace TimerCounter;

namespace Controllers 
{
	class MotorController
	{
		// Variables
		public:
		protected:
		private:
        RotationSensor* rotationSensor;
        TC motorTimerCounter;
        Gpio::Pin* multiplexPin;
        
		// Methods
		public:
		MotorController(Gpio::Pin* rotationSensorPin, TC motorTimerCounter, Gpio::Pin* motorMultiplexPin);
		~MotorController() {};
        
		protected:
		private:

	}; //MotorController
}

#endif //__MOTORCONTROLLER_H__
