using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading;
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


        public Boolean serialInitialized { get; private set; } = false;
        private SerialDevice serialPort;
        private DataWriter serialPortTx;
        private DataReader serialPortRx;
        private CancellationTokenSource readCancellationTokenSource;
        public List<byte> _response = new List<byte>();
        public ReadOnlyCollection<byte> response => _response.AsReadOnly();


        public MicroController(int receiveTimeout = 1000) {
            initializeSerialPort(receiveTimeout);
        }

        public Boolean transmitCommand(byte[] bytes) {
            byte[] command = new byte[bytes.Length + 2];    // Bytes + preamble0 + preamble1

            command[0] = (byte) PreAmble.P0;                // Add the preamble part 1
            command[1] = (byte) PreAmble.P1;                // Add the preamble part 2

            for (int i = 0; i < bytes.Length; i++)          // Add the command, the paramater length and the parameters
                command[2 + i] = bytes[i];
            
            transmitBytes(command);

            return false;
        }

        public byte waitForResponse() {
            readCancellationTokenSource = new CancellationTokenSource();        // Create a cancellation token to stop the reading
            _response.Clear();                                                  // Clear the current response list
            receiveBytes(readCancellationTokenSource.Token);                    // Read the data and store it in a list
            return 0;
        }

        private async void initializeSerialPort(int receiveTimeout) {
            try {
                string aqs = SerialDevice.GetDeviceSelector();                                  // Get the device selector
                DeviceInformation device = (await DeviceInformation.FindAllAsync(aqs))[0];      // Get the first (and only) serial device information
                serialPort = await SerialDevice.FromIdAsync(device.Id);                         // Use the aquired device information to get the Serial Port

                // If retrieving the Serial Port has failed => print an error message and return
                if (serialPort == null) {
                    System.Diagnostics.Debug.WriteLine("Could not find the serial port");
                    return;
                }

                // Configure serial settings
                serialPort.WriteTimeout = TimeSpan.FromMilliseconds(1000);
                serialPort.ReadTimeout = TimeSpan.FromMilliseconds(receiveTimeout);
                serialPort.BaudRate = 9600;
                serialPort.Parity = SerialParity.None;
                serialPort.StopBits = SerialStopBitCount.One;
                serialPort.DataBits = 8;
                serialPort.Handshake = SerialHandshake.None;

                serialPortTx = new DataWriter(serialPort.OutputStream);
                serialPortRx = new DataReader(serialPort.InputStream);

                System.Diagnostics.Debug.WriteLine("The serial port has been initialized");
                serialInitialized = true;

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private async void transmitBytes(byte[] bytes) {
            try {

                if (!serialInitialized) {
                    System.Diagnostics.Debug.WriteLine("The serial port is not initialized!");
                    return;
                }

                serialPortTx.WriteBytes(bytes);

                Task<uint> storeAsyncTask = serialPortTx.StoreAsync().AsTask();
                uint bytesWritten = await storeAsyncTask;

                // Print what bytes have been written
                System.Diagnostics.Debug.Write("Wrote " + bytesWritten + " bytes: [");
                foreach (byte aByte in bytes) System.Diagnostics.Debug.Write(" "+aByte);
                System.Diagnostics.Debug.WriteLine(" ]");

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private async void receiveBytes(CancellationToken cancellationToken) {
            try {

                if (!serialInitialized) {
                    System.Diagnostics.Debug.WriteLine("The serial port is not initialized!");
                    return;
                }
                
                cancellationToken.ThrowIfCancellationRequested();                                                                   // If task cancellation was requested, comply
                serialPortRx.InputStreamOptions = InputStreamOptions.Partial;                                                       
                
                using (var childCancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken)) {
                    Task<UInt32> loadAsyncTask = serialPortRx.LoadAsync( 8 ).AsTask(childCancellationTokenSource.Token);            // Create a task object to wait for data on the serialPort.InputStream

                    UInt32 bytesRead = await loadAsyncTask;                                                                         // Launch the task and wait

                    while (serialPortRx.UnconsumedBufferLength > 0) _response.Add(serialPortRx.ReadByte());                         // Store the read bytes

                    // Print what bytes have been read
                    System.Diagnostics.Debug.Write("Read " + bytesRead + " bytes: [");
                    foreach (byte aByte in _response) System.Diagnostics.Debug.Write(" " + aByte);
                    System.Diagnostics.Debug.WriteLine(" ]");
                }

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private void CancelReadTask() {
            if (readCancellationTokenSource != null) {
                if (!readCancellationTokenSource.IsCancellationRequested) {
                    readCancellationTokenSource.Cancel();
                }
            }
        }

    }
}
