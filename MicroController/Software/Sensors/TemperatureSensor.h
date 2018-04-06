/* 
* TemperatureSensor.h
*
* Created: 19-Mar-18 14:38:58
* Author: Robin C. Pel
*/

#ifndef __TEMPERATURESENSOR_H__
#define __TEMPERATURESENSOR_H__

#include "Sensor.h"
#include "../Hardware/Communication/OneWire.h"
#include <stdlib.h>

using namespace Hardware::Communication;

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
        Resolution resolution = Resolution::TwelveBit;
        float buffer = 0;

		// Methods
		public:
        TemperatureSensor(Hardware::Gpio::Pin* pins): Sensor(pins) {  };
		~TemperatureSensor();
		void* GetData();
        bool SetResolution(Resolution resolution);
		
		protected:
		private:
        uint8_t initializationSequence();
        void convertData();
        int16_t readRawTemperature();
        float rawToCelsius(int16_t raw);
        uint8_t resolutionDivider();        // 2^(1+resolution)
        
        
		
	}; //TemperatureSensor
}

#endif //__TEMPERATURESENSOR_H__
