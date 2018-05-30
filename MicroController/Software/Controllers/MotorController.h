/* 
* MotorController.h
*
* Created: 19-Mar-18 15:24:44
* Author: Robin C. Pel
*/

#ifndef __MOTORCONTROLLER_H__
#define __MOTORCONTROLLER_H__

#include "../Sensors/RotationSensor.h"

namespace Controllers 
{
	class MotorController
	{
		// Variables
		public:
		protected:
		private:
        Sensors::RotationSensor* rotationSensor;
        
		// Methods
		public:
		MotorController(Sensors::RotationSensor* rotSensor);
		~MotorController() {};
        
		protected:
		private:

	}; //MotorController
}

#endif //__MOTORCONTROLLER_H__
