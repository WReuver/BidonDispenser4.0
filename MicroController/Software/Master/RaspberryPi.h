/* 
* RaspberryPi.h
*
* Created: 19-Mar-18 15:30:34
* Author: Robin C. Pel
*/

#ifndef __RASPBERRYPI_H__
#define __RASPBERRYPI_H__

#include "../Communication/USART.h"

using namespace Hardware;
using namespace Communication;

namespace Master 
{
	class RaspberryPi
	{
        public:
        // The protocol's preambles
        enum class PreAmble {
            P0 = 0x00,
            P1 = 0xFF
        };
    
        // The possible commands
        enum class Command {
            Lock                = 0x02,
            Unlock              = 0x04,
            Sense               = 0x06,
            Temperature         = 0x08,
            Dispense            = 0x0A,
            Distance            = 0x0C,
            ERROR               = 0xFF
        };
        
        // The possible command responses
        enum class CommandResponse {
            Lock                = 0x03,
            Unlock              = 0x05,
            Sense               = 0x07,
            Temperature         = 0x09,
            Dispense            = 0x0B,
            Distance            = 0x0D,
            ERROR               = 0xFF
        };
        
        // Communication exceptions
        enum class ComException {
            TimeOut             = 0xE0, 
            Unknown             = 0xE1, 
            Parameter           = 0xE2,
            Locked              = 0xE3
        };
        
        
		// Variables
		private:
        static const uint8_t commandMaxSize = 3;
        uint8_t command[commandMaxSize] = {0};
        Usart::RxTx usartPins;
        
        
		// Methods
		public:
        RaspberryPi(Usart::RxTx pins);
        ~RaspberryPi() {};
          
        /**
         * Waits for a new command to arrive and puts it into the command buffer
         * 
         * \return uint8_t
         *      0 => Succes!
         *      1 => Command does not exist!
         *      2 => Timeout error
         */
        uint8_t waitForNextCommand();
        
        /**
         * Returns the response to the Raspberry Pi using the USART
         * 
         * \param uint8_t* response
         *      Array containing the command, parameter length and the parameters.
         */
        void returnResponse(uint8_t* response);
        
        /**
         * Returns the command response code which is equivalent to the given command
         */
        CommandResponse getEquivalentCommandResponse(Command cmd);
        
        /**
         * Returns the location of the last received command
         */
        uint8_t* getCommand() { return command; };
        
        
		private:
        /**
         * Resets the array containing the buffered command
         */
        void clearCommandData();
        
        /**
         * Returns true or false dependent on whether the command exists or not
         */
        bool commandExists(uint8_t cmd);
        
	}; //RaspberryPi
}

/**
    ////////////////////////////////////////
    //  Example: 
    /////////////////////

    RaspberryPi* raspi = new RaspberryPi(Usart::RxTx::C2_C3);       // Initialize the Raspberry Pi
    uint8_t success = 7;                                            // Variable to store the error code in, 0 = success, 7 = unchanged, 1 = command does not exist, 2 = timeout
    uint8_t myCommand[] = {                                         // The command:
        (uint8_t) RaspberryPi::CommandResponse::Sense,              // Sense response
        0x00                                                        // With 0 parameters
    };
    
    // Infinite loop
    while (1)
    {
        success = raspi->waitForNextCommand();
        raspi->returnResponse(myCommand);
    }
    
    ////////////////////
    /////////////////////////////////////////
    
 */

#endif //__RASPBERRYPI_H__
