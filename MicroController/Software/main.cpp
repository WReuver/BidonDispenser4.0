/*
 * main.cpp
 *
 * Created: 06-Mar-18 11:56:46
 * Author: Robin C. Pel
 */


// Includes
#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/Gpio.h"
#include "Hardware/GenericTC.h"
#include "Master/RaspberryPi.h"
#include "Controllers/CoolingController.h"
#include "Controllers/MotorController.h"


// Name-spaces
using namespace Hardware;
using namespace Communication;
using namespace Master;
using namespace Controllers;
using namespace Gpio;


// Raspberry Pi
RaspberryPi* raspberryPi;
Usart::RxTx raspberrySerialPort = Usart::RxTx::C2_C3;


// Cooling Controller Variables
MotorController* motorController;
Pin temperatureSensorPins[3] = { Pin::D6, Pin::D5, Pin::D4 };
Pin fanGroupPins[2] = { Pin::C0, Pin::C1 };
TimerCounter::TC coolingTimerCounter = TimerCounter::TC::TC0C;


// Motor Controller Variables
CoolingController* coolingController;
Pin rotationSensorPins[8] = { Pin::A0, Pin::A1, Pin::B2, Pin::B3, Pin::B4, Pin::B5, Pin::B6, Pin::B7 };
Sensors::RotationSensor* rotationSensor = new Sensors::RotationSensor(rotationSensorPins);
TimerCounter::TC motorTimerCounter = TimerCounter::TC::TC0D;





// Miscellaneous Variables
uint8_t const IDENTIFIER[] = { 0xAB, 0xBC, 0xCD, 0xDA };
bool locked = false;


// Functions
void executeLockCommand(uint8_t* response)
{
    if (locked)
    {
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;
        response[1] = 0x00;
    }
    else
    {
        locked = true;
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Lock);
        response[1] = 0x00;
    }
}

void executeUnlockCommand(uint8_t* response)
{
    locked = false;
    response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Unlock);
    response[1] = 0x00;
}

void executeSenseCommand(uint8_t* response)
{
    if (locked)
    {
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;
        response[1] = 0x00;
    }
    else
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Sense);
        response[1] = 0x04;
        
        response[2] = IDENTIFIER[0];
        response[3] = IDENTIFIER[1];
        response[4] = IDENTIFIER[2];
        response[5] = IDENTIFIER[3];
    }
}

void executeTemperatureCheckCommand(uint8_t* response, uint8_t* receivedCommand)
{
    if (locked)
    {
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;
        response[1] = 0x00;
    }
    else
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::TemperatureCheck);
        response[1] = 0x00;
        
        if (receivedCommand[1] == 1)                // Target temperature has been supplied
        {
            // TODO: Execute a cooling configuration update
        } 
        else                                        // Target temperature has not been supplied
        {
            // TODO: Execute a cooling configuration update
        }
    }
}

void executeDispenseCommand(uint8_t* response, uint8_t* receivedCommand)
{
    if (locked)
    {
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;
        response[1] = 0x00;
    }
    else
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Dispense);
        response[1] = 0x01;
        
        // TODO: Add whether the dispensing went fine or not
        response[2] = 0x00;
        
    }
}

void executeDistanceCommand(uint8_t* response)
{
    if (locked)
    {
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;
        response[1] = 0x00;
    }
    else 
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Distance);
        response[1] = 0x01;
        
        // TODO: Add the empty status of all columns
        response[2] = 0x00;
        
    }
}

void executecommand(uint8_t* response, uint8_t* receivedCommand)
{
    switch ((RaspberryPi::Command) receivedCommand[0])
    {
        case RaspberryPi::Command::Lock:                executeLockCommand(response);                                   break;
        case RaspberryPi::Command::Unlock:              executeUnlockCommand(response);                                 break;
        case RaspberryPi::Command::Sense:               executeSenseCommand(response);                                  break;
        case RaspberryPi::Command::TemperatureCheck:    executeTemperatureCheckCommand(response, receivedCommand);      break;
        case RaspberryPi::Command::Dispense:            executeDispenseCommand(response, receivedCommand);              break;
        case RaspberryPi::Command::Distance:            executeDistanceCommand(response);                               break;
    }
}

void routine(void)
{
    while (1)
    {
        uint8_t operationStatus = raspberryPi->waitForNextCommand();        // 0 = success, 1 = command does not exist, 2 = timeout
        uint8_t* receivedCommand = raspberryPi->getCommand();               // Get the location to the received command
        uint8_t response[6] = { 0 };
        
        switch (operationStatus)
        {
            case 0:     // Everything went fine
            executecommand(response, receivedCommand);
            break;
            
            
            case 1:     // Command does not exist
            response[0] = (uint8_t) RaspberryPi::ComException::Unknown;     // Add the "Command Unknown" Exception as command response
            response[1] = 0x00;                                             // Zero parameters
            break;
            
            
            case 2:     // Timeout
            response[0] = (uint8_t) RaspberryPi::ComException::TimeOut;     // Add the "Serial timeout" Exception as command response
            response[1] = 0x00;                                             // Zero parameters
            break;
        }
        
        raspberryPi->returnResponse(response);                              // Return the response
    }
}

void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    
    raspberryPi = new RaspberryPi(raspberrySerialPort);
    coolingController = new CoolingController(temperatureSensorPins, fanGroupPins);
    //motorController = new MotorController();
}

int main()
{
    initialize();
    routine();
}
