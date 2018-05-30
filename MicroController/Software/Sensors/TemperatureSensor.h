/* 
* TemperatureSensor.h
*
* Created: 19-Mar-18 14:38:58
* Author: Robin C. Pel
*/

#ifndef __TEMPERATURESENSOR_H__
#define __TEMPERATURESENSOR_H__

#include "Sensor.h"
#include "../Communication/OneWire.h"

namespace Sensors
{
	class TemperatureSensor : public Sensor 
	{
        // Enumerations
        public:
        protected:
        private:
        // ROM Commands
        enum class RomCommand
        {
            Search          = 0xF0,
            Read            = 0x33,
            Match           = 0x55,
            Skip            = 0xCC,
            AlarmSearch     = 0xEC
        };
        
        // Function Commands
        enum class FunctionCommand
        {
            Convert         = 0x44,
            WriteScratchPad = 0x4E,
            ReadScratchPad  = 0xBE,
            CopyScratchPad  = 0x48,
            RecallE         = 0xB8,
            ReadPowerSupply = 0xB4
        };
        
        // Resolutions
        enum class Resolution
        {
            NineBit,
            TenBit,
            ElevenBit,
            TwelveBit
        };
        
		// Variables
		public:
		protected:
		private:
        Resolution resolution = Resolution::TwelveBit;      // The resolution of the temperature sensor
		bool buffer = 0;                                    // Buffer for the data

		// Methods
		public:
		TemperatureSensor(Hardware::Gpio::Pin* pins);       // Default constructor
		~TemperatureSensor() {};                            // Default destructor
		void* GetData();                                    // Get the data from the sensor
		
		protected:
		private:
        uint8_t initializationSequence();                   // Send the initialization sequence to the sensor
        void convertData();                                 // Send the convert command to the sensor
        int16_t readRawTemperature();                       // Get the temperature data out of the sensor
        float rawToCelsius(int16_t raw);                    // Convert the temperature data from a value to Celsius
        uint8_t resolutionDivider();                        // Get the resolution divider, the resolution divider is calculated like this: 2^(1+resolution)
        
        
	}; //TemperatureSensor
}

#endif //__TEMPERATURESENSOR_H__

/** EXAMPLE

    volatile float realTemp = -100;
    
    Gpio::Pin pins[1] = {Gpio::Pin::B0};
    TemperatureSensor* ts = new TemperatureSensor(pins);
    
    void* dataLocation = ts->GetData();
    realTemp = *((float*) dataLocation);
    
    delete ts;
*/