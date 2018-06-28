using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using Windows.Devices.Enumeration;
using Windows.Devices.Gpio;
using Windows.Devices.Spi;

namespace BidonDispenser {

    [Obsolete("This class never worked, ONLY use it for reference!")]
    class Pn532 {
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
        
        private byte[] bitOrderTable = {
            0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
            0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
            0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
            0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
            0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
            0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
            0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
            0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
            0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
            0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
            0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
            0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
            0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
            0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
            0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
            0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
        };

        private SpiDevice spiPort;
        private bool spiIsInitialized = false;

        public Pn532(int spiBusNo) {
            initSpi(spiBusNo);
        }

        private async void initSpi(int spiBusNo) {
            try {
                System.Diagnostics.Debug.WriteLine("Initializing the SPI");
                var settings = new SpiConnectionSettings(spiBusNo);
                settings.ClockFrequency = 1_000_000;                      // Okay
                settings.Mode = SpiMode.Mode0;                          // Okay


                var controller = await SpiController.GetDefaultAsync();
                spiPort = controller.GetDevice(settings);

                System.Diagnostics.Debug.WriteLine("SPI Initialized");
                spiIsInitialized = true;


                //string spiAqs = SpiDevice.GetDeviceSelector();
                //var devicesInfo = await DeviceInformation.FindAllAsync(spiAqs);
                //System.Diagnostics.Debug.WriteLine("Device id: "+ devicesInfo[0].Id);
                //spiPort = await SpiDevice.FromIdAsync(devicesInfo[0].Id, settings);
            } catch (Exception e) {
                System.Diagnostics.Debug.WriteLine("SPI Initialisation Error: " + e.Message);
            }
        }

        public bool sendCommandAck(byte[] command, int timeout = 0) {
            writeCommand(command);

            if (!waitReady(timeout))
                return false;

            if (!readAck())
                return false;

            if (!waitReady(timeout))
                return false;

            return true;
        }

        private void writeCommand(byte[] command) {
            byte[] writeBuffer = new byte[9+command.Length];
            uint checkSum = (uint) Protocol.PreAmble + (uint) Protocol.StartCode1 + (uint) Protocol.StartCode2;
            byte cmdLen = (byte) (command.Length + 1);

            writeBuffer[0] = (byte) Operation.DataWrite;        // ok
            writeBuffer[1] = (byte) Protocol.PreAmble;          // ok
            writeBuffer[2] = (byte) Protocol.StartCode1;        // ok
            writeBuffer[3] = (byte) Protocol.StartCode2;        // ok
            writeBuffer[4] = (byte) (cmdLen);                   // ok
            writeBuffer[5] = (byte) (~cmdLen + 1);              // ok
            writeBuffer[6] = (byte) Mode.Slave;                 // ok
            checkSum += (uint) Mode.Slave;

            for (int i = 0; i < command.Length; i++) {
                writeBuffer[7 + i] = command[i];
                checkSum += (uint) command[i];
            }
            
            writeBuffer[7 + command.Length + 0] = (byte) (~checkSum & 0xFF);
            writeBuffer[7 + command.Length + 1] = (byte) Protocol.PostAmble;

            writeData(writeBuffer);
        }

        // Converts data to "least significant bit first"
        private byte[] swapBitOrder(byte[] bytes) {
            //return bytes;

            byte[] newBytes = new byte[bytes.Length];

            for (int i = 0; i < bytes.Length; i++)
                newBytes[i] = bitOrderTable[bytes[i]];

            return newBytes;
        }

        private bool readAck() {
            byte[] pn532Ack = { 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
            byte[] ackBuffer = readData(6);

            return ackBuffer.SequenceEqual(pn532Ack);
        }

        public bool isReady() {
            byte[] command = { (byte) Operation.StatusRead };
            byte[] response = new byte[1];
            
            writeData(command);
            response = readRaw(1);

            return ((response[0] & 0b1) == 1);
        }

        public bool waitReady(int timeout = 0) {
            int timer = 0;

            while (! isReady()) {
                if (timeout != 0) {
                    timer += 10;
                    if (timer > timeout)
                        return false;
                }
                Thread.Sleep(10);
            }

            return true;
        }

        // Read data from the SPI port
        public byte[] readData(int amount) {
            byte[] command = { (byte) Operation.DataRead };
            byte[] readBuffer = new byte[amount];

            writeData(command);                                     // Send the "read data" command
            spiPort.Read(readBuffer);
            readBuffer = swapBitOrder(readBuffer);

            // Print what has been read
            System.Diagnostics.Debug.Write("Reading: \t");
            for (int i = 0; i < readBuffer.Length; i++)
                System.Diagnostics.Debug.Write(readBuffer[i] + " ");
            System.Diagnostics.Debug.Write("\n");

            // Return the read data
            return readBuffer;
        }

        // Read data from the SPI port without the "read data" command
        public byte[] readRaw(int amount) {
            byte[] readBuffer = new byte[amount];
            
            spiPort.Read(readBuffer);
            readBuffer = swapBitOrder(readBuffer);

            // Print what has been read
            System.Diagnostics.Debug.Write("Reading: \t");
            for (int i = 0; i < readBuffer.Length; i++)
                System.Diagnostics.Debug.Write(readBuffer[i] + " ");
            System.Diagnostics.Debug.Write("\n");

            // Return the read data
            return readBuffer;
        }

        // Write data to the SPI port
        public void writeData(byte[] data) {

            System.Diagnostics.Debug.Write("Writing: \t");
            for (int i = 0; i < data.Length; i++)
                System.Diagnostics.Debug.Write(data[i]+" ");
            System.Diagnostics.Debug.Write("\n");

            data = swapBitOrder(data);
            spiPort.Write(data);
        }

        public void setup() {
            while (!spiIsInitialized);

            if (!sense()) {
                System.Diagnostics.Debug.WriteLine("Could not find the PN532");
                return;
            }

            if (!initialize()) {
                System.Diagnostics.Debug.WriteLine("Could not initialize the PN532");
                return;
            }
        }

        private bool sense() {
            byte[] pn532Response = { 0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03 };
            byte[] command = { (byte) Command.FirmwareVersion };
            byte[] response = new byte[12];

            if (!sendCommandAck(command, 1_000))
                return false;

            response = readData(12);

            return response.SequenceEqual(pn532Response);
        }

        private bool initialize() {
            byte[] command = { (byte) Command.SamConf, 0x01, 0x14, 0x01 };
            byte[] response = new byte[8];

            if (!sendCommandAck(command))
                return false;

            response = readData(8);

            return (response[5] == 0x15);
        }



        public void dispose() {
            spiPort.Dispose();
        }

    }
}
