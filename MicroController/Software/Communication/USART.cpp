/* 
* Usart.cpp
*
* Created: 13-Apr-18 12:52:40
* Author: Robin C. Pel
*/

#include "Usart.h"

void Communication::Usart::Initialize(RxTx pins)
{
    Gpio::Pin txPin = GetTx(pins);
    Gpio::Pin rxPin = GetRx(pins);
    
    Gpio::SetPinValue(txPin, Gpio::Value::High);
    Gpio::SetPinDirection(txPin, Gpio::Dir::Output);
    Gpio::SetPinDirection(rxPin, Gpio::Dir::Input);
}

void Communication::Usart::TransmitData(RxTx pins, uint8_t data)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->DATA = data;
}

uint8_t Communication::Usart::ReadData(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return usart->DATA;
}

void Communication::Usart::WaitForData(RxTx pins)
{
    while (!IsNewDataAvailable(pins));
}

uint8_t Communication::Usart::WaitForDataWithTimeout(RxTx pins)
{
    TimerCounter::ClearCount(TimerCounter::GetGenericTC());                         // Clear the TC count
    TimerCounter::DidTcOverflow(TimerCounter::GetGenericTC());                      // Reset the overflow flag
    
    while (!IsNewDataAvailable(pins))                                               // Wait for new data
    {
        if (TimerCounter::DidTcOverflow(TimerCounter::GetGenericTC())) return 1;    // If there is no new data before (around) two seconds have passed => exit
    }
    
    return 0;
}

bool Communication::Usart::IsNewDataAvailable(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return (bool) (usart->STATUS & 0b10000000);
}

bool Communication::Usart::IsTransmitComplete(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return (bool) (usart->STATUS & 0b01000000);
}

bool Communication::Usart::IsTransmitRegisterEmpty(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return (bool) (usart->STATUS & 0b00100000);
}

bool Communication::Usart::DidFrameErrorOccur(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return (bool) (usart->STATUS & 0b00010000);
}

bool Communication::Usart::DidReceiveBufferOverflow(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return (bool) (usart->STATUS & 0b00001000);
}

bool Communication::Usart::DidParityErrorOccur(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    return (bool) (usart->STATUS & 0b00000100);
}

void Communication::Usart::SetReceiveCompleteInterruptLevel(RxTx pins, uint8_t level)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLA & 0b00001111;
    usart->CTRLA = (reg | ((level & 0b11) << 4));
}

void Communication::Usart::SetTransmitCompleteInterruptLevel(RxTx pins, uint8_t level)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLA & 0b00110011;
    usart->CTRLA = (reg | ((level & 0b11) << 2));
}

void Communication::Usart::SetDataRegisterEmptyInterruptLevel(RxTx pins, uint8_t level)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLA & 0b00111100;
    usart->CTRLA = (reg | ((level & 0b11) << 0));
}

void Communication::Usart::EnableReceiver(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB |= 0b00010000;
}

void Communication::Usart::DisableReceiver(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB &= ~(0b00010000);
}

void Communication::Usart::EnableTransmitter(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB |= 0b00001000;
}

void Communication::Usart::DisableTransmitter(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB &= ~(0b00001000);
}

void Communication::Usart::EnableDoubleTransmissionSpeed(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB |= 0b00000100;
}

void Communication::Usart::DisableDoubleTransmissionSpeed(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB &= ~(0b00000100);
}

void Communication::Usart::EnableMultiprocessorCommunicationMode(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB |= 0b00000010;
}

void Communication::Usart::DisableMultiprocessorCommunicationMode(RxTx pins)
{
    USART_t* usart = UsartPinsToAddress(pins);
    usart->CTRLB &= ~(0b00000010);
}

void Communication::Usart::SetCommunicationMode(RxTx pins, CMode cmode)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLC & 0b00111111;
    usart->CTRLC = (reg | ((uint8_t) cmode << 6));
}

void Communication::Usart::SetParityMode(RxTx pins, PMode pmode)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLC & 0b11001111;
    usart->CTRLC = (reg | ((uint8_t) pmode << 4));
}

void Communication::Usart::SetStopBitMode(RxTx pins, SbMode sbmode)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLC & 0b11110111;
    usart->CTRLC = (reg | ((uint8_t) sbmode << 3));
}

void Communication::Usart::SetCharacterSize(RxTx pins, CharSize csize)
{
    USART_t* usart = UsartPinsToAddress(pins);
    uint8_t reg = usart->CTRLC & 0b11111000;
    usart->CTRLC = (reg | ((uint8_t) csize << 0));
}

void Communication::Usart::SetBaudrate(RxTx pins, Baudrate br)
{
    USART_t* usart = UsartPinsToAddress(pins);
    // 0        = BSCALE
    // 1        = BSEL
    // BR       = 0b0000111111111111
    usart->BAUDCTRLA = (uint8_t) ((uint16_t) br >> 0) & 0xff;
    usart->BAUDCTRLB = (uint8_t) ((uint16_t) br >> 8) & 0xff;
}

USART_t* Communication::Usart::UsartPinsToAddress(RxTx pins)
{
    switch (pins)
    {
        case RxTx::C2_C3: return &USARTC0;
        case RxTx::C6_C7: return &USARTC1;
        case RxTx::D2_D3: return &USARTD0;
        case RxTx::D6_D7: return &USARTD1;
        case RxTx::E2_E3: return &USARTE0;
        case RxTx::F2_F3: return &USARTF0;
        default: return nullptr;
    }
}

Hardware::Gpio::Pin Communication::Usart::GetRx(RxTx pins)
{
    uint8_t rawPin = ((uint16_t) pins >> 8) & 0xff;
    return (Gpio::Pin) rawPin;
}

Hardware::Gpio::Pin Communication::Usart::GetTx(RxTx pins)
{
    uint8_t rawPin = (uint16_t) pins & 0xff;
    return (Gpio::Pin) rawPin;
}
