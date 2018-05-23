/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

// REMINDER: The distance sensor class needs to be re-tested, the default clock has been replaced by the generic clock

#include "includes.h"
#include "Hardware/SystemClock.h"
#include "Hardware/Gpio.h"
#include "Hardware/GenericTC.h"
#include "Communication/USART.h"

using namespace Hardware;
using namespace Communication;

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Output);
}

int main()
{
    initialize();
    
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
        // Wait for new data to be available
        while (!Usart::IsNewDataAvailable(Usart::RxTx::C2_C3));
        // Read the available data
        response = Usart::ReadData(Usart::RxTx::C2_C3);
        // Transmit some data
        Usart::TransmitData(Usart::RxTx::C2_C3, response);
        // Wait a bit
        //_delay_ms(500);
    }
}
