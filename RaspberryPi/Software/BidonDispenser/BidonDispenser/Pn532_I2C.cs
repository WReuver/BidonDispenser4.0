using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace BidonDispenser {
    class Pn532_I2C {

        private Boolean i2cIsInitialized = false;
        private I2cDevice i2cPort;
        private int i2cDevAddress = 0x48;

        public Pn532_I2C() {
            initI2C();
        }

        private async void initI2C() {
            try {
                string aqs = I2cDevice.GetDeviceSelector();
                DeviceInformation device = (await DeviceInformation.FindAllAsync(aqs))[0];
                I2cConnectionSettings settings = new I2cConnectionSettings(i2cDevAddress);
                i2cPort = await I2cDevice.FromIdAsync(device.Id, settings);
                
                System.Diagnostics.Debug.WriteLine("I2C Initialized");
                i2cIsInitialized = true;
                
            } catch (Exception e) {
                System.Diagnostics.Debug.WriteLine("I2C Initialisation Error: " + e.Message + "\n" + e.StackTrace);
            }
        }

        
        public void setup() {

        }

        
        public void dispose() {
            i2cPort?.Dispose();
        }

    }
}
