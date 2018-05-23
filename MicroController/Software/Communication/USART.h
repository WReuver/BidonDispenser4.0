/* 
* Usart.h
*
* Created: 13-Apr-18 12:52:41
* Author: Robin C. Pel
*/

#ifndef __USART_H__
#define __USART_H__

#include "../Hardware/Gpio.h"
#include "../Hardware/GenericTC.h"

using namespace Hardware;

namespace Communication
{
    namespace Usart
    {
        // USART Pins
        enum class RxTx: uint16_t
        {
            C2_C3 = ((uint16_t) Gpio::Pin::C2 << 8) + ((uint16_t) Gpio::Pin::C3 & 0b00111111),
            C6_C7 = ((uint16_t) Gpio::Pin::C6 << 8) + ((uint16_t) Gpio::Pin::C7 & 0b00111111),
            D2_D3 = ((uint16_t) Gpio::Pin::D2 << 8) + ((uint16_t) Gpio::Pin::D3 & 0b00111111),
            D6_D7 = ((uint16_t) Gpio::Pin::D6 << 8) + ((uint16_t) Gpio::Pin::D7 & 0b00111111),
            E2_E3 = ((uint16_t) Gpio::Pin::E2 << 8) + ((uint16_t) Gpio::Pin::E3 & 0b00111111),
            F2_F3 = ((uint16_t) Gpio::Pin::F2 << 8) + ((uint16_t) Gpio::Pin::F3 & 0b00111111)
        };
            
        // USART Baud rates
        enum class Baudrate: uint16_t
        {
            // 0        = BSCALE
            // 1        = BSEL
            // BR       = 0b0000111111111111
            b2400       = 0b0110000000001100,
            b4800       = 0b0101000000001100,
            b9600       = 0b0100000000001100,
            b14400      = 0b0000000010001010,
            b19200      = 0b0011000000001100,
            b28800      = 0b1111000010001001,
            b38400      = 0b0010000000001100,
            b2000000    = 0b0000000000000000
        };
            
        // Communication Modes
        enum class CMode
        {
            Asynchronous,
            Synchronous,
            IrCom,
            MasterSpi
        };
        
        // Parity Modes
        enum class PMode
        {
            Disabled    = 0b00,
            Even        = 0b10,
            Odd         = 0b11
        };
    
        // Stop Bit Modes
        enum class SbMode
        {
            One,
            Two
        };
            
        // Character Size
        enum class CharSize
        {
            Bit5    = 0b000,
            Bit6    = 0b001,
            Bit7    = 0b010,
            Bit8    = 0b011,
            Bit9    = 0b111
        };
            
        void Initialize(RxTx pins);                                             // Sets the correct pin directions
            
        // Data Register
        void TransmitData(RxTx pins, uint8_t data);                             // Transmit one byte of data
        uint8_t ReadData(RxTx pins);                                            // Read one byte of data
        void WaitForData(RxTx pins);                                            // Wait for new data without a timeout
        uint8_t WaitForDataWithTimeout(RxTx pins);                              // Wait for new data with a max timeout of about 2 seconds, 0 = new data, 1 = timeout
            
        // Status Register
        bool IsNewDataAvailable(RxTx pins);                                     // New data available flag
        bool IsTransmitComplete(RxTx pins);                                     // Transmission completed flag
        bool IsTransmitRegisterEmpty(RxTx pins);                                // Transmission buffer is empty flag
        bool DidFrameErrorOccur(RxTx pins);                                     // Frame error occurred flag
        bool DidReceiveBufferOverflow(RxTx pins);                               // Buffer overflow occurred flag
        bool DidParityErrorOccur(RxTx pins);                                    // Parity error occurred flag
            
        // Control Register A
        void SetReceiveCompleteInterruptLevel(RxTx pins, uint8_t level);        // Set the level of the receive complete interrupt
        void SetTransmitCompleteInterruptLevel(RxTx pins, uint8_t level);       // Set the level of the transmit complete interrupt
        void SetDataRegisterEmptyInterruptLevel(RxTx pins, uint8_t level);      // Set the level of the data register empty interrupt
            
        // Control Register B
        void EnableReceiver(RxTx pins);                                         // Enable the receiver of the given USART pins
        void DisableReceiver(RxTx pins);                                        // Disable the receiver of the given USART pins
        void EnableTransmitter(RxTx pins);                                      // Enable the transmitter of the given USART pins
        void DisableTransmitter(RxTx pins);                                     // Disable the transmitter of the given USART pins
        void EnableDoubleTransmissionSpeed(RxTx pins);                          // Enable double transmission speed on the given USART pins
        void DisableDoubleTransmissionSpeed(RxTx pins);                         // Disable double transmission speed on the given USART pins
        void EnableMultiprocessorCommunicationMode(RxTx pins);                  // Enable multiprocessor communication mode on the given USART pins
        void DisableMultiprocessorCommunicationMode(RxTx pins);                 // Disable multiprocessor communication mode on the given USART pins
            
        // Control Register C
        void SetCommunicationMode(RxTx pins, CMode cmode);                      // Set the communication mode of the given USART pins
        void SetParityMode(RxTx pins, PMode pmode);                             // Set the parity mode of the given USART pins
        void SetStopBitMode(RxTx pins, SbMode sbmode);                          // Set the stop bit mode of the given USART pins
        void SetCharacterSize(RxTx pins, CharSize csize);                       // Set the character size on the given USART pins
            
        // Baudrate Registers
        void SetBaudrate(RxTx pins, Baudrate br);                               // Set the preferred baud rate on the given USART pins
            
        // Other
        USART_t* UsartPinsToAddress(RxTx pins);                                 // Get a pointer to the corresponding USART
        Gpio::Pin GetRx(RxTx pins);                                             // Get the Rx pin out of a RxTx couple
        Gpio::Pin GetTx(RxTx pins);                                             // Get the Tx pin out of a RxTx couple
            
        /* 
		* Functionalities not included:
		* - - - - - - - - - - - - - - - - - - - - - -
		* 9-bit character size mode
		* SPI mode
		*
		*/
            
    }
}

/****************************************************************************************\
 * EXAMPLE: Basic Usart
 *  Transmits some data from pin C3
 *  Waits for new data from pin C2
 *  Reads the new data from pin C2
 *  ==> Repeat
|****************************************************************************************|
    
    // Initialize the Usart
    Usart::Initialize(Usart::RxTx::C2_C3);
    // Set the baud rate to 9600
    Usart::SetBaudrate(Usart::RxTx::C2_C3, Usart::Baudrate::b9600);
    // Enable Rx
    Usart::EnableReceiver(Usart::RxTx::C2_C3);
    // Enable Tx
    Usart::EnableTransmitter(Usart::RxTx::C2_C3);
    
    volatile uint8_t response = 0;
    
    // Infinite loop
    while (1)
    {
        // Transmit some data
        Usart::TransmitData(Usart::RxTx::C2_C3, 0x88);
        // Wait for new data to be available
        while (!Usart::IsNewDataAvailable(Usart::RxTx::C2_C3));
        // Read the available data
        response = Usart::ReadData(Usart::RxTx::C2_C3);
        // Wait a bit
        _delay_ms(500);
    }

\***************************************************************************************/

#endif //__USART_H__
