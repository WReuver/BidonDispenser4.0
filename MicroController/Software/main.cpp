/*
* main.cpp
*
* Created: 06-Mar-18 11:56:46
* Author: Robin C. Pel
*/

#define F_CPU 32000000UL

#include <util/delay.h>
#include <stdint.h>

#include "Hardware/Gpio.h"
#include "Hardware/SystemClock.h"

using namespace Hardware;

static void initialize(void) 
{
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
}

#define TxMode      Gpio::SetPinDirection(Gpio::Pin::B0, Gpio::Dir::Output)
#define RxMode      Gpio::SetPinDirection(Gpio::Pin::B0, Gpio::Dir::Input)
#define SetLow      Gpio::SetPinValue(Gpio::Pin::B0, Gpio::Value::Low)
#define SetHigh     Gpio::SetPinValue(Gpio::Pin::B0, Gpio::Value::High)
#define GetValue    Gpio::GetPinValue(Gpio::Pin::B0)

/**
 * Returns 0 when the reset has succeeded
 * Returns 1 when the reset has failed
 */
static uint8_t reset() 
{
    RxMode;                                 // Let the pin float
    while (GetValue == Gpio::Value::Low);   // Wait until the pin is high
    
    SetLow;                                 // Pull the pin low
    TxMode;                                 // Set the pin to output again
    _delay_us(500);                         // Wait a bit
    
    RxMode;                                 // Let the pin float
    _delay_us(65);                          // Wait a bit
    
    uint8_t response = (uint8_t) GetValue;  // Read the reset response
    _delay_us(490);
    
    return response;                        // Return the response
}

static void write_bit(uint8_t bit) 
{
    if (bit)
    {
        SetLow;
        TxMode;
        _delay_us(10);
        SetHigh;
        _delay_us(55);
    }
    else
    {
        SetLow;
        TxMode;
        _delay_us(65);
        SetHigh;
        _delay_us(5);
    }
}

static uint8_t read_bit()
{
    uint8_t val = 0;
    
    TxMode;
    SetLow;
    _delay_us(3);
    RxMode;
    _delay_us(10);
    val = (uint8_t) GetValue;
    _delay_us(53);
    
    return val;
}

static void write(uint8_t data) 
{
    for (uint8_t mask = 1; mask; mask <<= 1)
    {
        write_bit(mask & data);
    }
    
    RxMode;
    SetLow;
}

static uint8_t read() 
{
    uint8_t val = 0;
    
    for (uint8_t mask = 1; mask; mask <<= 1)
    {
        if (read_bit())
        {
            val |= mask;
        }
    }
    
    return val;
}

static void skip() {
    write(0xCC);
}

static void readTemperature() {
    write(0x44);
}

int main() 
{
    initialize();
	Gpio::SetPinDirection(Gpio::Pin::A0, Gpio::Dir::Output);
	
    volatile uint8_t resetResult = 2;
    volatile uint8_t rom[8] = {0};
    volatile uint8_t temperature[2] = {0};
    volatile int16_t tempTemp = 0;
    volatile float realTemp = 0.0;
        
    //resetResult = reset();
    //_delay_ms(1);
    
    //write(0x33);                                        // Get ROM Command
    //for (int i = 0; i < 10; i++) rom[i] = read();
    //_delay_ms(1);
    
	while (1) 
	{
		Gpio::TogglePinValue(Gpio::Pin::A0);
        _delay_ms(500);
        
        resetResult = reset();
        skip();
        readTemperature();
        while (!read_bit());
        //_delay_ms(1000);
        
        resetResult = reset();
        skip();
        write(0xBE);                                            // Read scratchpad command
        //for (int i = 0; i < 2; i++) temperature[i] = read();    // Read the scratchpad
        tempTemp = read();
        tempTemp += (int16_t) read() * 256;
        _delay_ms(1000);
        
        //tempTemp = (uint16_t) temperature[1] << 8 | (uint16_t) temperature[0];
        realTemp = (float) tempTemp / 16;
        
        Gpio::TogglePinValue(Gpio::Pin::A0);
        _delay_ms(500);
	}
	
}
