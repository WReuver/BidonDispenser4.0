using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace BidonDispenser {

    [Obsolete("This class was never finsihed, do NOT use it!")]
    class Pn532_I2C {

        public enum Operation {
            DataWrite = 0x01,
            StatusRead = 0x02,
            DataRead = 0x03
        }

        public enum Command {
            FirmwareVersion = 0x02,
            Status = 0x04,
            SamConf = 0x14,
            DataExchange = 0x40,
            Communicate = 0x42,
            PassiveTarget = 0x4A
        }

        public enum Protocol {
            PreAmble = 0x00,
            StartCode1 = 0x00,
            StartCode2 = 0xFF,
            PostAmble = 0x00
        }

        public enum Mode {
            Slave = 0xD4,
            Master = 0xD5
        }


        private Boolean i2cIsInitialized = false;
        private I2cDevice i2cPort;
        private int i2cDevAddress = (0x48 >> 1);

        

        public async Task initialize() {
            try {
                string aqs = I2cDevice.GetDeviceSelector();
                DeviceInformation device = (await DeviceInformation.FindAllAsync(aqs))[0];
                I2cConnectionSettings settings = new I2cConnectionSettings(i2cDevAddress);
                settings.BusSpeed = I2cBusSpeed.StandardMode;
                i2cPort = await I2cDevice.FromIdAsync(device.Id, settings);
                
                Debug.WriteLine("The I2C port has been initialized");
                i2cIsInitialized = true;
                
            } catch (Exception e) {
                Debug.WriteLine("I2C port initialization Error: " + e.Message + "\n" + e.StackTrace);
            }
        }
        
        public async Task<Boolean> sense() {
            
            if (!i2cIsInitialized) 
                return false;
            
            try {
                byte[] i2cOut = new byte[1] { 0x00 };
                byte[] i2cIn  = new byte[1] { 0x00 };

                i2cPort.WriteRead(i2cOut, i2cIn);
                return true;

            } catch (Exception ex) {
                Debug.WriteLine("EXCEPTION: " + ex.Message + "\n" + ex.StackTrace);
                return false;
            }
        }



        public void dispose() {
            i2cPort?.Dispose();
        }

    }
}
