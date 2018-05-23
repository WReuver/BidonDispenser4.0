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
            TemperatureCheck    = 0x08,
            Dispense            = 0x0A,
            ERROR               = 0xFF
        };
        
        // The possible command responses
        enum class CommandResponse {
            Lock                = 0x03,
            Unlock              = 0x05,
            Sense               = 0x07,
            TemperatureCheck    = 0x09,
            Dispense            = 0x0B,
            ERROR               = 0xFF
        };
        
		// Variables
		private:
        uint8_t command[32] = {0};
        Usart::RxTx usartPins;
        
        
		// Methods
		public:
        RaspberryPi(Usart::RxTx pins);
        ~RaspberryPi() {};
        bool waitForNextCommand();
        uint8_t* getCommand();
        void returnResponse(uint8_t* response);
        bool validateChecksum();
        
		private:
        uint8_t calculateChecksum(uint8_t* bytes);
        void clearCommandData();
        
	}; //RaspberryPi
}

#endif //__RASPBERRYPI_H__
