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
using namespace TimerCounter;

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
        TC timerCounter;
        float lowerTargetTemperature = 3.0;
        float lowerTemperature = 50.0;
        float middleTemperature = 50.0;
        float upperTemperature = 50.0;
        

		// Methods
		public:
		CoolingController(Gpio::Pin* temperatureSensors, Gpio::Pin* fanGroupPins, TC coolingTimerCounter);
        ~CoolingController() {};
        void updateFanSpeed();                                                                                  // Read the temperature from the temperature sensors and adjust the fan speed accordingly
        void setLowerTargetTemperature(float targetTemp);                                                       // Set the target temperature of the lower temperature sensor
        void setFangroupSpeed(uint8_t groupNo, uint8_t fanSpeed);
        
        void gatherTemperatures();                                                                              // Reads the temperatures from the lower, middle and upper temperature sensors and stores the data in the designated variable
        float getLowerTemperature();                                                                            // Basic getter for the lower temperature
        float getMiddleTemperature();                                                                           // Basic getter for the middle temperature
        float getUpperTemperature();                                                                            // Basic getter for the upper temperature
        
		protected:
		private:
        
        
	}; //CoolingController
}

#endif //__COOLINGCONTROLLER_H__
