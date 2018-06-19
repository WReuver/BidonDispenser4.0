/**
 * main.cpp
 *
 * Created: 06-Mar-18 11:56:46
 * Author: Robin C. Pel
 */


//// Includes
#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/Gpio.h"
#include "Hardware/GenericTC.h"
#include "Master/RaspberryPi.h"
#include "Controllers/CoolingController.h"
#include "Controllers/MotorController.h"
#include "Sensors/DistanceSensor.h"


//// Namespaces
using namespace Hardware;
using namespace Communication;
using namespace Master;
using namespace Controllers;
using namespace Gpio;
using namespace Sensors;
using namespace TimerCounter;


//// LEDs
Pin greenLed = Pin::F3;                                                                                     // The green led
Pin yellowLed = Pin::F4;                                                                                    // The yellow led
Pin redLed = Pin::F6;                                                                                       // The red led
void runningLed( uint8_t ledVal ) { SetPinValue(greenLed, (Value) ledVal); }                                // This function controls the "running" LED, it will be on when the device is turned on
void busyLed( uint8_t ledVal ) { SetPinValue(yellowLed, (Value) ledVal); }                                  // This function controls the "busy" LED, it will be on when the device is processing a command
void errorLed( uint8_t ledVal ) { SetPinValue(redLed, (Value) ledVal); }                                    // This function controls the "error" LED, it will be on when an error has occurred


//// Raspberry Pi
RaspberryPi* raspberryPi;                                                                                   // The Raspberry Pi object
Usart::RxTx raspberrySerialPort = Usart::RxTx::D2_D3;                                                       // The Raspberry Pi's serial port pins


//// Cooling Controller Variables
CoolingController* coolingController;                                                                       // The cooling controller object
Pin temperatureSensorPins[3] = { Pin::D6, Pin::D5, Pin::D4 };                                               // The pins the temperature sensors are connected to
Pin fanGroupPins[2] = { Pin::C0, Pin::C1 };                                                                 // The pins the fan groups are connected to
TC coolingTc = TC::TC0C;                                                                                    // The timercounter the cooling controller uses


//// Motor Controller Variables
MotorController* motorController;                                                                           // The motor controller object
Pin rotationSensorPins[8] = { Pin::A0, Pin::A1, Pin::B2, Pin::B3, Pin::B4, Pin::B5, Pin::B6, Pin::B7 };     // The pins the rotation sensor are connected to
Pin motorMultiplexPins[3] = { Pin::F2, Pin::F1, Pin::F0 };                                                  // The pins the motor multiplexer is connected to
TC motorTimerCounter = TC::TC0D;                                                                            // The timercounter the motor controller uses
Pin motorTcPin = Pin::D1;                                                                                   // The pin the PWM signal has to be transmitted on for the motor controller


//// Distance Sensor
DistanceSensor* distanceSensor;                                                                             // The distance sensor object
Pin triggerPins[2] = { Pin::C3, Pin::C2 };                                                                  // The pins the distance sensors' triggers are connected to
Pin echoPin = Pin::D0;                                                                                      // The pin the distance sensors' echoes are connected to
Pin distanceMultiplexPins[4] = { Pin::C7, Pin::C6, Pin::C5, Pin::C4 };                                      // The pins the distance sensors' multiplexer is connected to
float emptyDistance = 100.0;                                                                                // After which distance a column is seen as "empty"


//// Miscellaneous Variables
uint8_t const IDENTIFIER[] = { 0xAB, 0xBC, 0xCD, 0xDA };                                                    // The identifier bytes of the micro controller, these are used in the "sense" command
bool locked = false;                                                                                        // Boolean indicating whether the micro controller is locked or not
bool infiniteTest = true;                                                                                   // Boolean indicating whether to execute one test function endlessly or to go past more of them


//// Functions
// Lock all functionalities
void executeLockCommand(uint8_t* response)
{
    if (locked)
    {
        errorLed(1);                                                                                                    // Turn on the "error" LED
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;                                                      // Add the "Locked" exception
        response[1] = 0x00;                                                                                             // Add the amount of parameters
    }
    else
    {
        locked = true;                                                                                                  // Lock the micro controller from doing anything but the "sense" and "unlock" command
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Lock);                  // Add the equivalent command response
        response[1] = 0x00;                                                                                             // Add the amount of parameters
    }
}

// Unlock all functionalities
void executeUnlockCommand(uint8_t* response)
{
    locked = false;                                                                                                     // Unlock the micro controller so it will be able to execute all commands again
    response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Unlock);                    // Add the equivalent command response
    response[1] = 0x00;                                                                                                 // Add the amount of parameters
}

// Return the constant identifier
void executeSenseCommand(uint8_t* response)
{
    response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Sense);                     // Add the equivalent command response
    response[1] = 0x04;                                                                                                 // Add the amount of parameters
    
    response[2] = IDENTIFIER[0];                                                                                        // Add the first identifier byte
    response[3] = IDENTIFIER[1];                                                                                        // Add the second identifier byte
    response[4] = IDENTIFIER[2];                                                                                        // Add the third identifier byte
    response[5] = IDENTIFIER[3];                                                                                        // Add the fourth identifier byte
}

// Measure the temperatures and return them to the master
void executeTemperatureCommand(uint8_t* response, uint8_t* receivedCommand)
{
    if (locked)
    {
        errorLed(1);                                                                                                    // Turn on the "error" LED
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;                                                      // Add the "Locked" exception
        response[1] = 0x00;                                                                                             // Add the amount of parameters
    }
    else
    {
        response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Temperature);           // Add the equivalent command response
        response[1] = 0x03;                                                                                             // Add the amount of parameters
        
        coolingController->gatherTemperatures();                                                                        // Refresh all t he temperature variables
        
        response[2] = (uint8_t) ( coolingController->getLowerTemperature() * 5.0 );                                     // Add the lower temperature to the response
        response[3] = (uint8_t) ( coolingController->getMiddleTemperature() * 5.0 );                                    // Add the middle temperature to the response
        response[4] = (uint8_t) ( coolingController->getUpperTemperature() * 5.0 );                                     // Add the upper temperature to the response
    }
}

// Dispense one bottle from the selected column
void executeDispenseCommand(uint8_t* response, uint8_t* receivedCommand)
{
    if (locked)
    {
        errorLed(1);                                                                                                    // Turn on the "error" LED
        response[0] = (uint8_t) RaspberryPi::ComException::Locked;                                                      // Add the "Locked" exception
        response[1] = 0x00;                                                                                             // Add the amount of parameters
    }
    else
    {
        if (receivedCommand[2] > 7) 
        {
            errorLed(1);                                                                                                // Turn on the "error" LED
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

// Measure the distances in all columns and return whether the the columns are empty or not
void executeDistanceCommand(uint8_t* response)
{
    if (locked)
    {
        errorLed(1);                                                                                                    // Turn on the "error" LED
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

// Execute a command
void executecommand(uint8_t* response, uint8_t* receivedCommand)
{
    switch ((RaspberryPi::Command) receivedCommand[0])
    {
        case RaspberryPi::Command::Lock:                executeLockCommand(response);                                   break;      // Received a lock command
        case RaspberryPi::Command::Unlock:              executeUnlockCommand(response);                                 break;      // Received an unlock command
        case RaspberryPi::Command::Sense:               executeSenseCommand(response);                                  break;      // Received a sense command
        case RaspberryPi::Command::Temperature:         executeTemperatureCommand(response, receivedCommand);           break;      // Received a temperature command
        case RaspberryPi::Command::Dispense:            executeDispenseCommand(response, receivedCommand);              break;      // Received a dispense command
        case RaspberryPi::Command::Distance:            executeDistanceCommand(response);                               break;      // Received a distance command
        default:                                                                                                        break;      // Impossible
    }
}

// Wait for a command, execute the command, return the response, do it all again
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

// Set the correct clock source, initialize the generic timer-counter, initialize the LEDs
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

// Initialize the raspberry pi, cooling controller, motor controller and distance sensor objects
void initializeHardware(void) 
{
    // Initialize all the other hardware
    raspberryPi = new RaspberryPi(raspberrySerialPort);
    coolingController = new CoolingController(temperatureSensorPins, fanGroupPins, coolingTc);
    motorController = new MotorController(motorMultiplexPins, motorTimerCounter, motorTcPin);
    distanceSensor = new DistanceSensor(triggerPins, echoPin, distanceMultiplexPins, emptyDistance);
}

// Raspberry Pi test command
void raspiTestCommand(uint8_t* response, uint8_t* receivedCommand)
{
    static uint8_t dispenseStatus = 0;
    response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse((RaspberryPi::Command) receivedCommand[0]);
    
    switch ((RaspberryPi::Command) receivedCommand[0])
    {
        case RaspberryPi::Command::Sense:       response[1] = 0x04; response[2] = IDENTIFIER[0]; response[3] = IDENTIFIER[1]; response[4] = IDENTIFIER[2]; response[5] = IDENTIFIER[3]; break;
        case RaspberryPi::Command::Dispense:    response[1] = 0x01; response[2] = ( dispenseStatus++ % 2 ); break;
        case RaspberryPi::Command::Distance:    response[1] = 0x01; response[2] = 0x03; break;
        case RaspberryPi::Command::Temperature:
        default:                                response[1] = 0x00;
    }
}

// Raspberry Pi test mode
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

// Rotate all the motors
void testMotors(void) 
{
    do
    {
        for (int i = 0; i < 8; i++)
        {
            busyLed(1);
            motorController->rotateMotor(i);
            _delay_ms(1000);
            busyLed(0);
            _delay_ms(1000);
        }
    } while (infiniteTest);
}

// Test both fan groups
void testFans(void) 
{
    //coolingController->setFangroupSpeed(0, 50);        // Lower fans
    //coolingController->setFangroupSpeed(1, 50);        // Upper fans
    
    do 
    {
        busyLed(1);
        
        coolingController->setFangroupSpeed(0, 100);
        coolingController->setFangroupSpeed(1, 100);
        _delay_ms(5000);
        coolingController->setFangroupSpeed(0, 0);
        coolingController->setFangroupSpeed(1, 0);
        _delay_ms(5000);
        
        busyLed(0);
    } while (infiniteTest);
    
}

// Test the temperature sensors
void temperatureCheckTest(void) 
{
    do
    {
        busyLed(1);
        
        volatile float temperatures[3] = { 50.0 };
        
        coolingController->gatherTemperatures();
        
        temperatures[0] = coolingController->getLowerTemperature();
        temperatures[1] = coolingController->getMiddleTemperature();
        temperatures[2] = coolingController->getUpperTemperature();
        
        busyLed(0);
    } while (infiniteTest);
}

// Test the distance sensors
void testDistSensor(void) 
{
    do 
    {
        busyLed(1);
        
        volatile float distances[16] = { 0.0 };
        float* resultLocation = distanceSensor->getData();
        
        for (int i = 0; i < 16; i++)
            distances[i] = resultLocation[i];
        
        const int sensorNo = 0;
        
        for (int i = sensorNo; i < (sensorNo+1); i++)
            distances[i] = distanceSensor->getOneData(i);
        
        
        busyLed(0);
    } while (infiniteTest);
}

// The general main
int main()
{
    initialize();               // Initialize the base system
    initializeHardware();       // Initialize the other hardware
    runningLed(1);              // Turn on the "running" LED
    runRoutine();               // Start the routine
    runningLed(0);              // The "running" LED should never be stopped since the "runRoutine" function will run endlessly
    
    
    //testRaspi();
    //testMotors();
    //testFans();
    //temperatureCheckTest();
    //testDistSensor();
    
}
