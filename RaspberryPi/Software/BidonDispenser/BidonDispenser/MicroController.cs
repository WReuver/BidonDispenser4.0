using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
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
            Temperature = 0x08,
            Dispense = 0x0A,
            Distance = 0x0C,
            ERROR = 0xFF
        };

        // The possible command responses
        public enum CommandResponse {
            Lock = 0x03,
            Unlock = 0x05,
            Sense = 0x07,
            Temperature = 0x09,
            Dispense = 0x0B,
            Distance = 0x0D,
            ERROR = 0xFF
        };

        // Communication exceptions
        public enum ComException {
            TimeOut = 0xE0,
            Unknown = 0xE1,
            Parameter = 0xE2,
            Locked = 0xE3
        };


        public Boolean serialInitialized { get; private set; } = false;
        private SerialDevice serialPort;
        private DataWriter serialPortTx;
        private DataReader serialPortRx;
        private CancellationTokenSource readCancellationTokenSource;

        private readonly List<byte> IDENTIFIER = new List<byte> { 0xAB, 0xBC, 0xCD, 0xDA };

        private Mutex commandRight = new Mutex();                               // The mutex which decides whether the current thread has command right or not
        private int mutexTimeout = 15_000;                                      // How many seconds the program should wait for the mutex


        public MicroController(int receiveTimeout = 1000) {
            initializeSerialPort(receiveTimeout);
        }

        public async Task<Boolean> sense() {
            for (int i = 0; i < 9; i++) {
                var result = await sendSenseCommand();          // Get the result

                if (
                    (result.Item1 == 0) && 
                    (result.Item2.Count > 1) && 
                    (result.Item2[1] == 4) && 
                    (result.Item2[2] == IDENTIFIER[0]) && 
                    (result.Item2[3] == IDENTIFIER[1]) && 
                    (result.Item2[4] == IDENTIFIER[2]) && 
                    (result.Item2[5] == IDENTIFIER[3]) && 
                    (result.Item2[6] == IDENTIFIER[4])
                    ) {
                    return true;
                }
            }

            return false;
        }


        // Send command responses:
        // 0 => Everything went fine
        // 1 => Serial port is not initialized
        // 2 => Exception was catched
        // 3 => Mutex timeout

        public async Task<Tuple<int, List<Byte>>> sendSenseCommand() {
            if (!serialInitialized)
                return new Tuple<int, List<Byte>>(1, null);

            try {
                commandRight.WaitOne(mutexTimeout);                     // Claim the mutex
            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);                            // Mutex (probably) timed out
                return new Tuple<int, List<Byte>>(3, null);
            }

            byte[] bytesToSend = new byte[] { (byte) Command.Sense, 0x00 };

            try {
                transmitCommand(bytesToSend);
                return await waitForResponse();

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);

            } finally {
                commandRight.ReleaseMutex();                            // Release the mutex
            }
        }

        public async Task<Tuple<int, List<Byte>>> sendLockCommand() {
            if (!serialInitialized)
                return new Tuple<int, List<Byte>>(1, null);

            try {
                commandRight.WaitOne(mutexTimeout);                     // Claim the mutex
            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);                            // Mutex (probably) timed out
                return new Tuple<int, List<Byte>>(3, null);
            }

            byte[] bytesToSend = new byte[] { (byte) Command.Lock, 0x00 };

            try {
                transmitCommand(bytesToSend);
                return await waitForResponse();

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);

            } finally {
                commandRight.ReleaseMutex();                            // Release the mutex
            }
        }

        public async Task<Tuple<int, List<Byte>>> sendUnlockCommand() {
            if (!serialInitialized)
                return new Tuple<int, List<Byte>>(1, null);

            try {
                commandRight.WaitOne(mutexTimeout);                     // Claim the mutex
            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);                            // Mutex (probably) timed out
                return new Tuple<int, List<Byte>>(3, null);
            }

            byte[] bytesToSend = new byte[] { (byte) Command.Unlock, 0x00 };

            try {
                transmitCommand(bytesToSend);
                return await waitForResponse();

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);

            } finally {
                commandRight.ReleaseMutex();                            // Release the mutex
            }
        }

        public async Task<Tuple<int, List<Byte>>> sendTemperatureCommand() {
            if (!serialInitialized)
                return new Tuple<int, List<Byte>>(1, null);

            try {
                commandRight.WaitOne(mutexTimeout);                     // Claim the mutex
            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);                            // Mutex (probably) timed out
                return new Tuple<int, List<Byte>>(3, null);
            }

            byte[] bytesToSend = new byte[] { (byte) Command.Temperature, 0x00 };

            try {
                transmitCommand(bytesToSend);
                return await waitForResponse();

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);

            } finally {
                commandRight.ReleaseMutex();                            // Release the mutex
            }
        }

        public async Task<Tuple<int, List<Byte>>> sendDispenseCommand(byte columnNumber) {
            if (!serialInitialized)
                return new Tuple<int, List<Byte>>(1, null);

            try {
                commandRight.WaitOne(mutexTimeout);                     // Claim the mutex
            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);                            // Mutex (probably) timed out
                return new Tuple<int, List<Byte>>(3, null);
            }

            byte[] bytesToSend = new byte[] { (byte) Command.Dispense, 0x01, columnNumber };

            try {
                transmitCommand(bytesToSend);
                return await waitForResponse();

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);

            } finally {
                commandRight.ReleaseMutex();                            // Release the mutex
            }
        }

        public async Task<Tuple<int, List<Byte>>> sendDistanceCommand() {
            if (!serialInitialized)
                return new Tuple<int, List<Byte>>(1, null);

            try {
                commandRight.WaitOne(mutexTimeout);                     // Claim the mutex
            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);                            // Mutex (probably) timed out
                return new Tuple<int, List<Byte>>(3, null);
            }

            byte[] bytesToSend = new byte[] { (byte) Command.Distance, 0x00 };

            try {
                transmitCommand(bytesToSend);
                return await waitForResponse();

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);

            } finally {
                commandRight.ReleaseMutex();                            // Release the mutex
            }
        }


        private void transmitCommand(byte[] bytes) {
            byte[] command = new byte[bytes.Length + 2];    // Bytes + preamble0 + preamble1

            command[0] = (byte) PreAmble.P0;                // Add the preamble part 1
            command[1] = (byte) PreAmble.P1;                // Add the preamble part 2

            for (int i = 0; i < bytes.Length; i++)          // Add the command, the paramater length and the parameters
                command[2 + i] = bytes[i];

            transmitBytes(command);
        }

        private async Task<Tuple<int, List<Byte>>> waitForResponse() {
            cancelReadTask();                                               // Stop the current read task
            readCancellationTokenSource = new CancellationTokenSource();    // Create a cancellation token to stop the reading
            return await receiveBytes(readCancellationTokenSource.Token);   // Read the data and store it in a list
        }

        private async void initializeSerialPort(int receiveTimeout) {
            try {
                string aqs = SerialDevice.GetDeviceSelector();                                  // Get the device selector
                DeviceInformation device = (await DeviceInformation.FindAllAsync(aqs))[0];      // Get the first (and only) serial device information
                serialPort = await SerialDevice.FromIdAsync(device.Id);                         // Use the aquired device information to get the Serial Port

                // If retrieving the Serial Port has failed => print an error message and return
                if (serialPort == null) {
                    Debug.WriteLine("Could not find the serial port");
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

                Debug.WriteLine("The serial port has been initialized");
                serialInitialized = true;

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
            }
        }

        private async void transmitBytes(byte[] bytes) {
            try {

                if (!serialInitialized) {
                    Debug.WriteLine("The serial port is not initialized!");
                    return;
                }

                serialPortTx.WriteBytes(bytes);

                Task<uint> storeAsyncTask = serialPortTx.StoreAsync().AsTask();
                uint bytesWritten = await storeAsyncTask;

                // Print what bytes have been written
                Debug.Write("Wrote " + bytesWritten + " bytes: [");
                foreach (byte aByte in bytes)
                    Debug.Write(" " + aByte);
                Debug.WriteLine(" ]");

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
            }
        }

        private async Task<Tuple<int, List<Byte>>> receiveBytes(CancellationToken cancellationToken) {
            try {

                if (!serialInitialized) {
                    Debug.WriteLine("The serial port is not initialized!");
                    return new Tuple<int, List<Byte>>(1, null);
                }
                
                cancellationToken.ThrowIfCancellationRequested();                                                                   // If task cancellation was requested, comply
                serialPortRx.InputStreamOptions = InputStreamOptions.Partial;
                List<Byte> response = new List<Byte>();                                                                             // Create a list of bytes to store the response in

                using (var childCancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken)) {

                    Boolean foundPreAmble = false;

                    while (!foundPreAmble) {                                                                                        // Wait for the preamble

                        await serialPortRx.LoadAsync(1).AsTask(childCancellationTokenSource.Token);                                 // Wait for one byte to come in
                        if ((serialPortRx.UnconsumedBufferLength > 0) && (serialPortRx.ReadByte() == (int) PreAmble.P0)) {          // Check whether it's the first preamble part or not

                            await serialPortRx.LoadAsync(1).AsTask(childCancellationTokenSource.Token);                             // Wait for one more byte to come in
                            if ((serialPortRx.UnconsumedBufferLength > 0) && (serialPortRx.ReadByte() == (int) PreAmble.P1)) {      // Check whether it's the second preamble part or not
                                foundPreAmble = true;
                            }

                        }
                    }

                    UInt32 bytesRead = await serialPortRx.LoadAsync(2).AsTask(childCancellationTokenSource.Token);                  // Wait for two bytes to come in
                    while (serialPortRx.UnconsumedBufferLength > 0)
                        response.Add(serialPortRx.ReadByte());                                                                      // Store the read bytes in the response list

                    UInt32 bytesReadPars = 0;

                    if (response.Count() > 1) {
                        bytesReadPars = await serialPortRx.LoadAsync(response[1]).AsTask(childCancellationTokenSource.Token);       // Wait for the parameter bytes to come in
                        while (serialPortRx.UnconsumedBufferLength > 0)
                            response.Add(serialPortRx.ReadByte());                                                                  // Store those bytes as well
                    }

                    // Print the read bytes
                    Debug.Write("Read " + (bytesRead + bytesReadPars) + " bytes: [");
                    foreach (byte aByte in response)
                        Debug.Write(" " + aByte);
                    Debug.WriteLine(" ]");
                }

                return new Tuple<int, List<Byte>>(0, response);

            } catch (Exception ex) {
                Debug.WriteLine(ex.Message);
                return new Tuple<int, List<Byte>>(2, null);
            }
        }

        private void cancelReadTask() {
            if (readCancellationTokenSource != null) {
                if (!readCancellationTokenSource.IsCancellationRequested) {
                    readCancellationTokenSource.Cancel();
                }
            }
        }


        public CommandResponse getEquivalentCommandResponse(Command command) {
            switch (command) {
                case Command.Sense:
                    return CommandResponse.Sense;
                case Command.Lock:
                    return CommandResponse.Lock;
                case Command.Unlock:
                    return CommandResponse.Unlock;
                case Command.Temperature:
                    return CommandResponse.Temperature;
                case Command.Dispense:
                    return CommandResponse.Dispense;
                case Command.Distance:
                    return CommandResponse.Distance;
                default:
                    return CommandResponse.ERROR;
            }
        }

        static public Boolean isCommandResponse(byte b) {

            switch ((CommandResponse) b) {
                case CommandResponse.Sense:         return true;
                case CommandResponse.Lock:          return true;
                case CommandResponse.Unlock:        return true;
                case CommandResponse.Temperature:   return true;
                case CommandResponse.Dispense:      return true;
                case CommandResponse.Distance:      return true;
                default:                            return false;
            }
        }
        
        static public Boolean isImportantException(byte b) {

            switch ((ComException) b) {
                case ComException.Locked:       return false;
                case ComException.Parameter:    return true;
                case ComException.TimeOut:      return true;
                case ComException.Unknown:      return true;
                default:                        return false;
            }
        }
        
        
        public void dispose() {
            serialPort?.Dispose();
            commandRight?.Dispose();
        }

    }
}
