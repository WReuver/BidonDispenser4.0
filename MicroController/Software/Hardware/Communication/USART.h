/* 
* Usart.h
*
* Created: 13-Apr-18 12:52:41
* Author: Robin C. Pel
*/

#ifndef __USART_H__
#define __USART_H__

#include "../../includes.h"

namespace Hardware
{
    namespace Communication
    {
        namespace Usart
        {
            bool Initialize(USART_t* usart);
            
            // Data Register
            void TransmitData(USART_t* usart, uint8_t data);
            uint8_t ReadData(USART_t* usart);
            
            // Status Register
            bool IsNewDataAvailable(USART_t* usart);
            bool IsTransmitComplete(USART_t* usart);
            bool IsTransmitRegisterEmpty(USART_t* usart);
            bool DidFrameErrorOccur(USART_t* usart);
            bool DidReceiveBufferOverflow(USART_t* usart);
            bool DidParityErrorOccur(USART_t* usart);
            
            // Control Register A
            void SetReceiveCompleteInterruptLevel(USART_t* usart, uint8_t level);
            void SetTransmitCompleteInterruptLevel(USART_t* usart, uint8_t level);
            void SetDataRegisterEmptyInterruptLevel(USART_t* usart, uint8_t level);
            
            // Control Register B
            void EnableReceiver(USART_t* usart);
            void DisableReceiver(USART_t* usart);
            void EnableTransmitter(USART_t* usart);
            void DisableTransmitter(USART_t* usart);
            void EnableDoubleTransmissionSpeed(USART_t* usart);
            void DisableDoubleTransmissionSpeed(USART_t* usart);
            void EnableMultiprocessorCommunicationMode(USART_t* usart);
            void DisableMultiprocessorCommunicationMode(USART_t* usart);
            
            
            
            
        }
    }
}

#endif //__USART_H__
