using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.Gpio;
using Windows.Devices.SerialCommunication;
using Windows.Storage.Streams;
using System.Threading;
using System.Collections.ObjectModel;
using System.Threading.Tasks;

namespace BidonDispenser {
    public sealed partial class MainPage: Page {
        private MainModel mainModel = new MainModel();
        
        private GpioPin led;
        private Boolean serialInitialized = false;
        private Pn532 nfcModule;


        MicroController mc = null;
        private Collection<DeviceInformation> listOfDevices;
        private SerialDevice serialPort;
        private DataWriter dataWriteObject;
        private DataReader dataReaderObject;


        public MainPage() {
            this.InitializeComponent();

            DispatcherTimer t = new DispatcherTimer();
            t.Interval = TimeSpan.FromSeconds(20);
            t.Tick += T_Tick;
            t.Start();
            
            initGpio();
            //initUsart();
            mc = new MicroController();


            listOfDevices = new Collection<DeviceInformation>();
            listAvailablePorts();



            //nfcModule = new Pn532(0);
            //nfcModule.setup();

            // Serial stuff
            /* while (!serialInitialized);
            serialTx = new DataWriter(serialDev.OutputStream);
            serialRx = new DataReader(serialDev.InputStream);

            serialTx.WriteByte(0xAA);
            byte myData = serialRx.ReadByte();
            System.Diagnostics.Debug.WriteLine("Byte read = "+myData); */
        }

        private async void listAvailablePorts() {
            try {
                string aqs = SerialDevice.GetDeviceSelector();
                var dis = await DeviceInformation.FindAllAsync(aqs);

                for (int i = 0; i < dis.Count; i++) {
                    listOfDevices.Add(dis[i]);
                }
            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private async void connectHandler(object sender, RoutedEventArgs e) {

            byte[] data = { (byte) 'A' };
            while (!mc.isReady());
            mc.transmitCommand(data);

            //DeviceInformation entry = listOfDevices[0];

            //try {
            //    var task = SerialDevice.FromIdAsync(entry.Id);
            //    await task;
            //    serialPort = task.GetResults();

            //    if (serialPort == null) {
            //        System.Diagnostics.Debug.WriteLine("Could not find the device with id "+entry.Id);
            //        System.Diagnostics.Debug.WriteLine("Status: " + task.Status);
            //        System.Diagnostics.Debug.WriteLine("Err Code: " + task.ErrorCode);
            //        return;
            //    } else {
            //        System.Diagnostics.Debug.WriteLine("Connected!");
            //    }

            //    // Configure serial settings
            //    serialPort.WriteTimeout = TimeSpan.FromMilliseconds(1000);
            //    serialPort.ReadTimeout = TimeSpan.FromMilliseconds(1000);
            //    serialPort.BaudRate = 9600;
            //    serialPort.Parity = SerialParity.None;
            //    serialPort.StopBits = SerialStopBitCount.One;
            //    serialPort.DataBits = 8;
            //    serialPort.Handshake = SerialHandshake.None;
            //} catch (Exception ex) {
            //    System.Diagnostics.Debug.WriteLine(ex.Message);
            //}
        }

        private async void writeReadHandler(object sender, RoutedEventArgs e) {
            try {
                
                if (serialPort == null) {
                    System.Diagnostics.Debug.WriteLine("Could not find the device");
                    return;
                }

                dataWriteObject = new DataWriter(serialPort.OutputStream);
                dataReaderObject = new DataReader(serialPort.InputStream);

                System.Diagnostics.Debug.WriteLine("Writing...");
                dataWriteObject.WriteByte(0xAA);

                Task<UInt32> storeAsyncTask = dataWriteObject.StoreAsync().AsTask();
                UInt32 bytesWritten = await storeAsyncTask;
                System.Diagnostics.Debug.WriteLine("Wrote "+bytesWritten+" bytes");

                System.Diagnostics.Debug.WriteLine("Reading...");
                byte myData = dataReaderObject.ReadByte();



                System.Diagnostics.Debug.WriteLine("Byte read = " + myData);

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private async void disconnectHandler(object sender, RoutedEventArgs e) {
            try {
                if (serialPort == null) {
                    System.Diagnostics.Debug.WriteLine("Could not find the device");
                    return;
                }

                dataWriteObject.Dispose();
                dataReaderObject.Dispose();
                serialPort.Dispose();
                serialPort = null;

            } catch (Exception ex) {
                System.Diagnostics.Debug.WriteLine(ex.Message);
            }
        }

        private void initGpio() {
            GpioController gpio = GpioController.GetDefault();

            if (gpio == null)
                return;

            /// LED on GPIO 3
            led = gpio.OpenPin(3);
            led.SetDriveMode(GpioPinDriveMode.Output);
            led.Write(GpioPinValue.Low);
            DispatcherTimer ledTimer = new DispatcherTimer();
            ledTimer.Interval = TimeSpan.FromSeconds(0.5);
            ledTimer.Tick += ledTick;
            ledTimer.Start();
        }

        private async void initUsart() {
            try {
                String aqs = SerialDevice.GetDeviceSelector("UART0");
                var devices = await DeviceInformation.FindAllAsync(aqs, null);

                for (int i = 0; i < devices.Count; i++) {
                    System.Diagnostics.Debug.WriteLine("devices["+i+"] = "+devices[i].Id);
                }

                /// Has any serial device been found?
                if (devices.Count == 0) {
                    System.Diagnostics.Debug.WriteLine("No Serial Devices found!");
                    return;
                }
                
                var operation = SerialDevice.FromIdAsync(devices[0].Id);
                while (operation.Status != Windows.Foundation.AsyncStatus.Completed) {
                    System.Diagnostics.Debug.WriteLine("Task status = "+ operation.Status);
                    Thread.Sleep(500);
                }
                serialPort = await operation;

                /// Was the requested serial device found?
                if (serialPort == null) {
                    System.Diagnostics.Debug.WriteLine("Serial Device not found!");
                    return;
                } else {
                    System.Diagnostics.Debug.WriteLine("Serial Device found!");
                }

                serialPort.BaudRate = 9600;
                serialPort.DataBits = 8;
                serialPort.StopBits = SerialStopBitCount.One;
                serialPort.Parity = SerialParity.None;

                serialInitialized = true;
            } 
            catch(Exception e) {
                System.Diagnostics.Debug.WriteLine("[EXCEPTION]: "+e.Message);
                System.Diagnostics.Debug.WriteLine(e.StackTrace);
            }
        }

        static int index = 0;
        private void T_Tick(object sender, object e) {
            index++;
            mainModel.mediaSource = (MainModel.mediaNames) (index % 6);
        }

        private void ledTick(object sender, object e) {
            if (led.Read() == GpioPinValue.Low) {
                led.Write(GpioPinValue.High);
            } else {
                led.Write(GpioPinValue.Low);
            }
        }
        

    }
}
