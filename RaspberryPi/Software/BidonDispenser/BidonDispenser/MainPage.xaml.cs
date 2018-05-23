using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.Gpio;
using Windows.Devices.Spi;
using Windows.Devices.SerialCommunication;

namespace BidonDispenser {
    public sealed partial class MainPage: Page {
        private MainModel mainModel = new MainModel();

        private const int LED = 3;
        private GpioPin led;
        private SerialDevice serialDev;

        private Pn532 nfcModule;

        public MainPage() {
            this.InitializeComponent();
            DispatcherTimer t = new DispatcherTimer();
            t.Interval = TimeSpan.FromSeconds(20);
            t.Tick += T_Tick;
            t.Start();

            DispatcherTimer ledTimer = new DispatcherTimer();
            ledTimer.Interval = TimeSpan.FromSeconds(0.5);
            ledTimer.Tick += ledTick;
            ledTimer.Start();

            initGpio();
            initUsart();
            //nfcModule = new Pn532(0);
            //nfcModule.setup();
        }

        private void initGpio() {
            GpioController gpio = GpioController.GetDefault();

            if (gpio == null)
                return;

            led = gpio.OpenPin(LED);
            led.SetDriveMode(GpioPinDriveMode.Output);
            led.Write(GpioPinValue.Low);
        }

        private async void initUsart() {
            try {
                String aqs = SerialDevice.GetDeviceSelector();
                DeviceInformation deviceInfo = (await DeviceInformation.FindAllAsync(aqs))[0];
                serialDev = await SerialDevice.FromIdAsync(deviceInfo.Id);

                if (serialDev == null) {
                    System.Diagnostics.Debug.WriteLine("Serial Device not found!");
                    return;
                }

                serialDev.BaudRate = 9600;
                serialDev.DataBits = 8;
                serialDev.StopBits = SerialStopBitCount.One;
                serialDev.Parity = SerialParity.None;


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
