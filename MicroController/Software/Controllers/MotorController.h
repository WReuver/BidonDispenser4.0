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
        Gpio::Pin multiplexPin[3];
        TC timerCounter;
        Gpio::Pin tcPin;
        RotationSensor* rotationSensor;
        uint8_t motorNumberToMuxChannel[8] = { 5, 7, 6, 4, 2, 1, 0, 3 };
        
		// Methods
		public:
		MotorController(Gpio::Pin* motorMultiplexPin, TC motorTimerCounter, Gpio::Pin motorTcPin, Gpio::Pin* rotationSensorPins);
		~MotorController() {};
        void rotateMotor(uint8_t motor);                                                                // Rotate the specified motor
        
		protected:
		private:
		void setMuxChannel(uint8_t channel);                                                            // Set the specified multiplex channel
        void waitForRotation(uint8_t channelNumber);                                                    // Wait for the cylinder to rotate 180 degrees

	}; //MotorController
}

#endif //__MOTORCONTROLLER_H__
