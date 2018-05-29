/* 
* RaspberryPi.cpp
*
* Created: 19-Mar-18 15:30:34
* Author: Robin C. Pel
*/

#include "RaspberryPi.h"

/**
 * Constructor, initializes the USART for the communication between the host and the Raspberry Pi
 */
 Master::RaspberryPi::RaspberryPi(Usart::RxTx pins)
{
    usartPins = pins;
    Usart::Initialize(usartPins);                               // Initialize the Usart
    Usart::SetBaudrate(usartPins, Usart::Baudrate::b9600);      // Set the baud rate to 9600
    Usart::EnableReceiver(usartPins);                           // Enable Rx
    Usart::EnableTransmitter(usartPins);                        // Enable Tx
}

/**
 * Waits for a new command to arrive and puts it into the command buffer
 * 
 * \return uint8_t
 *      0 => Succes!
 *      1 => Command does not exist!
 *      2 => Timeout error
 */
uint8_t Master::RaspberryPi::waitForNextCommand()
{
    clearCommandData();
    
    bool foundPreAmble = false;
    
    while ( !foundPreAmble ) 
    {
        Usart::WaitForData(usartPins);                                          // Wait for preamble part 1
        if (Usart::ReadData(usartPins) == (uint8_t) PreAmble::P0) 
        {
            if ( Usart::WaitForDataWithTimeout(usartPins) == 0 )                // Wait for preamble part 2 with a timeout
            {
                if (Usart::ReadData(usartPins) == (uint8_t) PreAmble::P1) 
                {
                    foundPreAmble = true;                                       // Preamble found!
                }
            }
            // else => retry
        }
    }
    
    if (Usart::WaitForDataWithTimeout(usartPins)) return 2;         // Wait for new data for about 2 seconds
    command[0] = Usart::ReadData(usartPins);                        // Read the Command
    
    if (Usart::WaitForDataWithTimeout(usartPins)) return 2;         // Wait for new data for about 2 seconds
    command[1] = Usart::ReadData(usartPins);                        // Read the Parameter Length
    
    for (int i = 0; i < command[1]; i++) 
    {
        if (Usart::WaitForDataWithTimeout(usartPins)) return 2;     // Wait for new data for about 2 seconds
        command[2+i] = Usart::ReadData(usartPins);                  // Read a parameter
    }        
    
    if (!commandExists(command[0])) return 1;                       // Failure
    
    return 0;                                                       // Success
}

/**
 * Returns the response to the Raspberry Pi using the USART
 * 
 * \param uint8_t* response
 *      Array containing the command, parameter length and the parameters.
 */
void Master::RaspberryPi::returnResponse(uint8_t* response)
{
    uint8_t fullResponse[4+response[1]] = {0};                      // Create an array for the full response
    
    fullResponse[0] = (uint8_t) PreAmble::P0;                       // Add the first preamble
    fullResponse[1] = (uint8_t) PreAmble::P1;                       // Add the second preamble
    
    for (int i = 0; i < (response[1]+2); i++)
        fullResponse[2+i] = response[i];                            // Add the command, parameter length and parameters
    
    for (int i = 0; i < (4+response[1]); i++) 
    {
        Usart::TransmitData(usartPins, fullResponse[i]);            // Transmit the full response over the USART
        _delay_ms(1);
    }        
}

/**
 * Resets the array containing the buffered command
 */
void Master::RaspberryPi::clearCommandData()
{
    for (int i = 0; i < commandMaxSize; i++)
    {
        command[i] = 0;
    }
}

/**
 * Returns true or false dependent on whether the command exists or not
 */
bool Master::RaspberryPi::commandExists(uint8_t comm)
{
    switch (comm) 
    {
        case (uint8_t) Command::Lock:               return true;
        case (uint8_t) Command::Unlock:             return true;
        case (uint8_t) Command::Sense:              return true;
        case (uint8_t) Command::TemperatureCheck:   return true;
        case (uint8_t) Command::Dispense:           return true;
        default:                                    return false;
    }
}
