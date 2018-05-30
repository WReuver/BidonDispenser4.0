/* 
* CoolingController.cpp
*
* Created: 19-Mar-18 15:27:07
* Author: Robin C. Pel
*/

#include "CoolingController.h"


Controllers::CoolingController::CoolingController(Gpio::Pin* temperatureSensorPins, Gpio::Pin* fanGroupPins)
{
    Gpio::Pin ts0[1] = { temperatureSensorPins[0] };
    Gpio::Pin ts1[1] = { temperatureSensorPins[1] };
    Gpio::Pin ts2[1] = { temperatureSensorPins[2] };
    
    temperatureSensor[0] = new TemperatureSensor(ts0);
    temperatureSensor[1] = new TemperatureSensor(ts1);
    temperatureSensor[2] = new TemperatureSensor(ts2);
    fanGroup[0] = fanGroupPins[0];
    fanGroup[1] = fanGroupPins[1];
}
