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
#include "Master/RaspberryPi.h"

using namespace Hardware;
using namespace Communication;
using namespace Master;

static void initialize(void)
{
    SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
    TimerCounter::InitializeGenericTC();
    Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Output);
}

int main()
{
    initialize();
    
    
    //// Initialize the Usart
    //Usart::Initialize(Usart::RxTx::C2_C3);
    //// Set the baud rate to 9600
    //Usart::SetBaudrate(Usart::RxTx::C2_C3, Usart::Baudrate::b9600);
    //// Enable Rx
    //Usart::EnableReceiver(Usart::RxTx::C2_C3);
    //// Enable Tx
    //Usart::EnableTransmitter(Usart::RxTx::C2_C3);
    //
    //volatile uint8_t response = 0;
    
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
        
        //// Wait for new data to be available
        //while (!Usart::IsNewDataAvailable(Usart::RxTx::C2_C3));
        //// Read the available data
        //response = Usart::ReadData(Usart::RxTx::C2_C3);
        //// Transmit some data
        //Usart::TransmitData(Usart::RxTx::C2_C3, response);
        //// Wait a bit
        ////_delay_ms(500);
    }
}
