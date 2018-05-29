/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

// REMINDER: The distance sensor class needs to be re-tested, the default clock has been replaced by the generic clock

#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/Gpio.h"
#include "Hardware/GenericTC.h"
#include "Master/RaspberryPi.h"
#include "Controllers/CoolingController.h"
#include "Controllers/MotorController.h"

using namespace Hardware;
using namespace Communication;
using namespace Master;
using namespace Controllers;

RaspberryPi* raspberryPi;
CoolingController* coolingController;
MotorController* motorController;

// Initialize all the hardware
void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    
    raspberryPi = new RaspberryPi(Usart::RxTx::C2_C3);
    coolingController = new CoolingController();
    motorController = new MotorController();
}

void routine(void) 
{
    while (1) 
    {
        uint8_t operationStatus = raspberryPi->waitForNextCommand();        // 0 = success, 1 = command does not exist, 2 = timeout
        uint8_t response[6] = { 0 };
        
        switch (operationStatus) 
        {
            case 0:
                // Everything went fine
            break;
            
            case 1:
                // Command does not exist
            break;
            
            case 2:
                // Timeout
            break;
        }
        
        
        
    }
}

int main()
{
    initialize();
    
    uint8_t success = 7;                                    // Variable to store the error code in, 0 = success, 7 = unchanged, 1 = command does not exist, 2 = timeout
    uint8_t myCommand[] = {                                 // The command:
        (uint8_t) RaspberryPi::CommandResponse::Sense,      // Sense response
        0x01                                                // With 0 parameters
    };
    
    while (1)
    {
        success = raspberryPi->waitForNextCommand();
        raspberryPi->returnResponse(myCommand);
    }
}

