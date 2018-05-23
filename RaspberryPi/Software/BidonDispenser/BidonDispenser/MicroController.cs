using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.Storage.Streams;

namespace BidonDispenser {
    class MicroController {

        // The protocol's preambles
        private enum PreAmble {
            P0 = 0x00,
            P1 = 0xFF
        };

        // The possible commands
        public enum Command {
            Lock = 0x02,
            Unlock = 0x04,
            Sense = 0x06,
            TemperatureCheck = 0x08,
            Dispense = 0x0A,
            ERROR = 0xFF
        };

        // The possible command responses
        public enum CommandResponse {
            Lock = 0x03,
            Unlock = 0x05,
            Sense = 0x07,
            TemperatureCheck = 0x09,
            Dispense = 0x0B,
            ERROR = 0xFF
        };


        private Boolean serialInitialized = false;
        private SerialDevice serialPort;
        private DataWriter serialPortTx;
        private DataReader serialPortRx;
        private ArrayList response = null;



        public MicroController() {
            initializeSerialPort();
        }

        public Boolean transmitCommand(byte[] bytes) {
            // TODO: Add the preamble and the checksum
            // TODO: Transmit the command
            transmitBytes(bytes);

            return false;
        }

        public Boolean waitForResponse(int withTimeOut = 0) {
            // TODO: Clear the current response arraylist
            // TODO: Read in the bytes
            // TODO: Remove the preamble and the checksum
            // TODO: Return a boolean indicating whether the checksum is correct or not

            return false;
        }

        public ArrayList getResponse() {
            return response;
        }

        public Boolean isReady() {
            return serialInitialized;
        }

        private Boolean validateChecksum() {
            // TODO: Calculate what the checksum should be
            // TODO: Return whether the checksum of the response is equal to the calculated checksum

            return false;
        }

        private byte calculateChecksum(ArrayList bytes) {
            // TODO: Calculate the checksum with the given bytes

            return 0x00;
        }

        private async void initializeSerialPort() {
            try {
                string aqs = SerialDevice.GetDeviceSelector();                                  // Get the device selector
                DeviceInformation device = (await DeviceInformation.FindAllAsync(aqs))[0];      // Get the first (and only) serial device information
                serialPort = await SerialDevice.FromIdAsync(device.Id);                         // Use the aquired device information to get the Serial Port

                // If retrieving the Serial Port has failed => print an error message and return
                if (serialPort == null) {
                    System.Diagnostics.Debug.WriteLine("Could not find the Serial Port");
                    return;
                }

                // Configure serial settings
                serialPort.WriteTimeout = TimeSpan.FromMilliseconds(1000);
                serialPort.ReadTimeout = TimeSpan.FromMilliseconds(1000);
                serialPort.BaudRate = 9600;
                serialPort.Parity = SerialParity.None;
                serialPort.StopBits = SerialStopBitCount.One;
                serialPort.DataBits = 8;
                serialPort.Handshake = SerialHandshake.None;

                serialPortTx = new DataWriter(serialPort.OutputStream);
                serialPortRx = new DataReader(serialPort.InputStream);

                System.Diagnostics.Debug.WriteLine("Serial Port initialized");
                serialInitialized = true;

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private async void transmitBytes(byte[] bytes) {
            try {

                if (!serialInitialized) {
                    System.Diagnostics.Debug.WriteLine("Serial Port is not initialized!");
                    return;
                }

                serialPortTx.WriteBytes(bytes);

                Task<uint> storeAsyncTask = serialPortTx.StoreAsync().AsTask();
                uint bytesWritten = await storeAsyncTask;
                System.Diagnostics.Debug.WriteLine("Wrote " + bytesWritten + " bytes");
                
                

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }


    }
}
