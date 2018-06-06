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
#include "Sensors/DistanceSensor.h"


// Name-spaces
using namespace Hardware;
using namespace Communication;
using namespace Master;
using namespace Controllers;
using namespace Gpio;
using namespace Sensors;
using namespace TimerCounter;


// LEDs
Pin greenLed = Pin::F3;
Pin yellowLed = Pin::F4;
Pin redLed = Pin::F6;
void runningLed( uint8_t ledVal ) { SetPinValue(greenLed, (Value) ledVal); }        // This function controls the "running" LED, it will be on when the device is turned on
void busyLed( uint8_t ledVal ) { SetPinValue(yellowLed, (Value) ledVal); }          // This function controls the "busy" LED, it will be on when the device is processing a command
void errorLed( uint8_t ledVal ) { SetPinValue(redLed, (Value) ledVal); }            // This function controls the "error" LED, it will be on when an error has occurred


// Raspberry Pi
RaspberryPi* raspberryPi;
Usart::RxTx raspberrySerialPort = Usart::RxTx::D2_D3;


// Cooling Controller Variables
CoolingController* coolingController;
Pin temperatureSensorPins[3] = { Pin::D6, Pin::D5, Pin::D4 };
Pin fanGroupPins[2] = { Pin::C0, Pin::C1 };
TC coolingTc = TC::TC0C;


// Motor Controller Variables
MotorController* motorController;
Pin rotationSensorPins[8] = { Pin::A0, Pin::A1, Pin::B2, Pin::B3, Pin::B4, Pin::B5, Pin::B6, Pin::B7 };
Pin motorMultiplexPins[3] = { Pin::F2, Pin::F1, Pin::F0 };
TC motorTimerCounter = TC::TC0D;
Pin motorTcPin = Pin::D1;


// Distance Sensor
DistanceSensor* distanceSensor;
Pin triggerPins[2] = { Pin::C3, Pin::C2 };
Pin echoPin = Pin::D0;
Pin distanceMultiplexPins[4] = { Pin::C7, Pin::C6, Pin::C5, Pin::C4 };
float emptyDistance = 100.0;


// Miscellaneous Variables
uint8_t const IDENTIFIER[] = { 0xAB, 0xBC, 0xCD, 0xDA };
bool locked = false;


// Functions
void executeLockCommand(uint8_t* response)
{
    if (locked)
    {
        errorLed(1);
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
        errorLed(1);
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
        errorLed(1);
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;                                                      // Add the "Locked
        response[1] = 0x00;                                                                                             // Zero parametersresponse[1] = 0x00;
    }
    else
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::TemperatureCheck);      // Add the equivalent command response
        response[1] = 0x00;                                                                                             // Add the amount of parameters
        
        if (receivedCommand[1] == 1) coolingController->setLowerTargetTemperature(receivedCommand[2]);                  // Target temperature has been supplied
        coolingController->updateFanSpeed();                                                                            // Update the fan speed according to the current temperatures
    }
}

void executeDispenseCommand(uint8_t* response, uint8_t* receivedCommand)
{
    if (locked)
    {
        errorLed(1);
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;                                                      // Add the "Locked
        response[1] = 0x00;                                                                                             // Zero parametersresponse[1] = 0x00;
    }
    else
    {
        if (receivedCommand[2] > 7) 
        {
            errorLed(1);
            response[0] = (uint8_t) RaspberryPi::ComException::Parameter;                                               // Add the "Not enough or wrong parameters" exception
            response[1] = 0x00;                                                                                         // Add the amount of parameters
        }
        else
        {
            response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Dispense);          // Add the equivalent command response
            response[1] = 0x01;                                                                                         // Add the amount of parameters
            
            motorController->rotateMotor(receivedCommand[2]);                                                           // Rotate the requested motor
            
            if ( distanceSensor->getSimpleData() & (1 << receivedCommand[2]) ) response[2] = 0x01;                      // The column just became empty
            else response[2] = 0x00;                                                                                    // The column still contains bottles
        }
    }
}

void executeDistanceCommand(uint8_t* response)
{
    if (locked)
    {
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;                                                      // Add the "Locked" exception
        response[1] = 0x00;                                                                                             // Zero parameters
    }
    else 
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Distance);              // Add the equivalent command response
        response[1] = 0x01;                                                                                             // Add the amount of parameters
        response[2] = distanceSensor->getSimpleData();                                                                  // Add the empty state of all eight columns
    }
}

void executecommand(uint8_t* response, uint8_t* receivedCommand)
{
    switch ((RaspberryPi::Command) receivedCommand[0])
    {
        case RaspberryPi::Command::Lock:                executeLockCommand(response);                                   break;      // Received a lock command
        case RaspberryPi::Command::Unlock:              executeUnlockCommand(response);                                 break;      // Received an unlock command
        case RaspberryPi::Command::Sense:               executeSenseCommand(response);                                  break;      // Received a sense command
        case RaspberryPi::Command::TemperatureCheck:    executeTemperatureCheckCommand(response, receivedCommand);      break;      // Received a temperature check command
        case RaspberryPi::Command::Dispense:            executeDispenseCommand(response, receivedCommand);              break;      // Received a dispense command
        case RaspberryPi::Command::Distance:            executeDistanceCommand(response);                               break;      // Received a distance command
        default:                                                                                                        break;      // Impossible
    }
}

void runRoutine(void)
{
    while (1)
    {
        uint8_t operationStatus = raspberryPi->waitForNextCommand();        // 0 = success, 1 = command does not exist, 2 = timeout
        uint8_t* receivedCommand = raspberryPi->getCommand();               // Get the location to the received command
        uint8_t response[6] = { 0 };                                        // The response will never be larger than six bytes
        
        busyLed(1);
        
        switch (operationStatus)
        {
            case 0:     // Everything went fine, the command is recognized and there was no timeout
            executecommand(response, receivedCommand);
            break;
            
            
            case 1:     // Command does not exist
            errorLed(1);
            response[0] = (uint8_t) RaspberryPi::ComException::Unknown;     // Add the "Command Unknown" Exception as command response
            response[1] = 0x00;                                             // Zero parameters
            break;
            
            
            case 2:     // Timeout
            errorLed(1);
            response[0] = (uint8_t) RaspberryPi::ComException::TimeOut;     // Add the "Serial timeout" Exception as command response
            response[1] = 0x00;                                             // Zero parameters
            break;
        }
        
        raspberryPi->returnResponse(response);                              // Return the response
        
        busyLed(0);
        errorLed(0);
    }
}

void initialize(void)
{
    // Initialize the system clock and the generic timer-counter
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    
    // Initialize the status LEDs
    SetPinDirection(greenLed, Dir::Output);
    SetPinDirection(yellowLed, Dir::Output);
    SetPinDirection(redLed, Dir::Output);
}

void initializeHardware(void) 
{
    // Initialize all the other hardware
    raspberryPi = new RaspberryPi(raspberrySerialPort);
    coolingController = new CoolingController(temperatureSensorPins, fanGroupPins, coolingTc);
    motorController = new MotorController(motorMultiplexPins, motorTimerCounter, motorTcPin);
    distanceSensor = new DistanceSensor(triggerPins, echoPin, distanceMultiplexPins, emptyDistance);
}

uint8_t dispenseStatus = 0;

void raspiTestCommand(uint8_t* response, uint8_t* receivedCommand)
{
    response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse((RaspberryPi::Command) receivedCommand[0]);
    
    switch ((RaspberryPi::Command) receivedCommand[0])
    {
        case RaspberryPi::Command::Sense:       response[1] = 0x04; response[2] = IDENTIFIER[0]; response[3] = IDENTIFIER[1]; response[4] = IDENTIFIER[2]; response[5] = IDENTIFIER[3]; break;
        case RaspberryPi::Command::Dispense:    response[1] = 0x01; response[2] = ( dispenseStatus++ % 2 ); break;
        case RaspberryPi::Command::Distance:    response[1] = 0x01; response[2] = 0x03; break;
        default:                                response[1] = 0x00;
    }
}

void testRaspi(void) 
{
    // Initialize the system clock and the generic timer-counter
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    
    // Initialize all the other hardware
    raspberryPi = new RaspberryPi(raspberrySerialPort);
    
    while (1)
    {
        uint8_t operationStatus = raspberryPi->waitForNextCommand();        // 0 = success, 1 = command does not exist, 2 = timeout
        uint8_t* receivedCommand = raspberryPi->getCommand();               // Get the location to the received command
        uint8_t response[6] = { 0 };                                        // The response will never be larger than six bytes
        
        switch (operationStatus)
        {
            case 0:     // Everything went fine, the command is recognized and there was no timeout
            raspiTestCommand(response, receivedCommand);
            break;
            
            
            case 1:     // Command does not exist
            errorLed(1);
            response[0] = (uint8_t) RaspberryPi::ComException::Unknown;     // Add the "Command Unknown" Exception as command response
            response[1] = 0x00;                                             // Zero parameters
            break;
            
            
            case 2:     // Timeout
            errorLed(1);
            response[0] = (uint8_t) RaspberryPi::ComException::TimeOut;     // Add the "Serial timeout" Exception as command response
            response[1] = 0x00;                                             // Zero parameters
            break;
        }
        
        raspberryPi->returnResponse(response);                              // Return the response
    }
}

void testMotors(void) 
{
    busyLed(1);
    
    while (1)
    {
        for (int i = 2; i < 8; i++)
        {
            motorController->rotateMotor(i);
            _delay_ms(2500);
        }
    }
}

void testFans(void) 
{
    coolingController->setFangroupSpeed(0, 100);        // Lower fans
    coolingController->setFangroupSpeed(1, 100);        // Upper fans
    
    while (1) 
    {
        //coolingController->setFangroupSpeed(0, 80);
        //coolingController->setFangroupSpeed(1, 80);
        //_delay_ms(5000);
        //coolingController->setFangroupSpeed(0, 0);
        //coolingController->setFangroupSpeed(1, 0);
        //_delay_ms(5000);
    }
    
}

void temperatureCheckTest(void) 
{
    while (1) 
    {
        busyLed(1);
        
        volatile float temperatures[3] = { 50.0 };
        
        coolingController->gatherTemperatures();
        
        temperatures[0] = coolingController->getLowerTemperature();
        temperatures[1] = coolingController->getMiddleTemperature();
        temperatures[2] = coolingController->getUpperTemperature();
        
        busyLed(0);
    }
}

void testDistSensor(void) 
{
    while (1) 
    {
        busyLed(1);
        
        volatile float distances[16] = { 0.0 };
        float* resultLocation = distanceSensor->getData();
        
        for (int i = 0; i < 16; i++)
            distances[i] = resultLocation[i];
        
        //const int sensorNo = 13;
        
        //for (int i = sensorNo; i < (sensorNo+1); i++)
            //distances[i] = distanceSensor->getOneData(i);
        
        
        busyLed(0);
    }
}

int main()
{
    initialize();
    initializeHardware();
    //runningLed(1);
    //runRoutine();
    //runningLed(0);
    
    //testMotors();
    //testFans();
    //temperatureCheckTest();
    //testDistSensor();
    
}
