/* 
* MotorController.cpp
*
* Created: 19-Mar-18 15:24:44
* Author: Robin C. Pel
*/

#include "MotorController.h"

 Controllers::MotorController::MotorController(Gpio::Pin* rotationSensorPin, TC motorTimerCounter, Gpio::Pin* multiplexPin):
    motorTimerCounter(motorTimerCounter),
    multiplexPin(multiplexPin)
{
    rotationSensor = new RotationSensor(rotationSensorPin);
}
