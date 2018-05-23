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
 * Waits for a new command to arrive and puts it in the command buffer
 * 
 * \return bool
 *      Whether the checksum of the received message is correct or not
 */
bool Master::RaspberryPi::waitForNextCommand()
{
    clearCommandData();
    while (!Usart::IsNewDataAvailable(usartPins));             // Wait for new data to be available
    
    // Read all the available data
    command[0] = Usart::ReadData(usartPins);                   // PreAmble0
    command[1] = Usart::ReadData(usartPins);                   // PreAmble1
    command[2] = Usart::ReadData(usartPins);                   // Command
    command[3] = Usart::ReadData(usartPins);                   // Parameter Length
    
    for (int i = 0; i < command[3]; i++)
        command[4+i] = Usart::ReadData(usartPins);             // Parameters
    
    command[4+command[3]] = Usart::ReadData(usartPins);        // Checksum
    
    return validateChecksum();                                 // Return whether the checksum is valid or not
}

/**
 * Returns the response to the Raspberry Pi using the USART
 * 
 * \param uint8_t* response
 *      Array containing the command, parameter length and the parameters.
 */
void Master::RaspberryPi::returnResponse(uint8_t* response)
{
    uint8_t fullResponse[5+response[1]] = {0};                      // Create an array for the full response
    
    fullResponse[0] = (uint8_t) PreAmble::P0;                       // Add the first preamble
    fullResponse[1] = (uint8_t) PreAmble::P1;                       // Add the second preamble
    
    for (int i = 0; i < (response[1]+2); i++)
        fullResponse[2+i] = response[i];                            // Add the command, parameter length and parameters
    
    fullResponse[4+response[1]] = calculateChecksum(response);      // Add the checksum
    
    for (int i = 0; i < (fullResponse[3]+5); i++)
        Usart::TransmitData(usartPins, fullResponse[i]);            // Transmit the full response over the USART
}

/**
 * Returns whether the checksum in the currently buffered command has a valid checksum or not
 */
bool Master::RaspberryPi::validateChecksum()
{
    // Add up the parameter length and all the parameters
    uint16_t paramTotal = 0x0000;
    for (int i = 0; i < (command[3]+1); i++) paramTotal += command[3+i];
    
    // Subtract the paramTotal from 0xFF to get the checksum
    uint8_t validSum = 0xFF - ((uint8_t) paramTotal);
    
    // Check whether the checksum is correct or not
    return validSum == command[4+command[3]];
}

/** 
 * Calculate the checksum
 * 
 * \param uint8_t* bytes
 *      Array of bytes containing the command, parameter length and parameters
 */
uint8_t Master::RaspberryPi::calculateChecksum(uint8_t* bytes)
{
    // Add up all the parameters
    uint16_t paramTotal = 0x0000;
    for (int i = 0; i < (bytes[1]+1); i++) paramTotal += bytes[1+i];
    
    // Add up the parameter length and the parameter sum, subtract this number from 0xFF to get the checksum
    uint8_t checkSum = 0xFF - ((uint8_t) paramTotal);
    
    // Return the calculated checksum
    return checkSum;
}

/**
 * Resets the array containing the buffered command
 */
void Master::RaspberryPi::clearCommandData()
{
    for (int i = 0; i < 32; i++)
    {
        command[i] = 0;
    }
}
