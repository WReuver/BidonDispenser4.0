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
using Windows.System.Profile;
using Windows.UI.ViewManagement;

namespace BidonDispenser {
    public sealed partial class MainPage: Page {
        private MainModel mainModel = new MainModel();

        private const Double promotionMsPerTick = 200;
        private const int msUntilPromotionMediaSwitch = 30_000;
        
        private GpioPin led;
        private Pn532 nfcModule;
        
        MicroController mc = null;

        private Boolean windowsIot = false;

        public MainPage() {
            this.InitializeComponent();

            // Check on which device we're running
            System.Diagnostics.Debug.WriteLine("Running on "+AnalyticsInfo.VersionInfo.DeviceFamily);
            if (AnalyticsInfo.VersionInfo.DeviceFamily == "Windows.IoT") {
                windowsIot = true;
            }

            initializePromotionTimer();


            if (windowsIot) {
                initGpio();
                mc = new MicroController();
                
                //nfcModule = new Pn532(0);
                //nfcModule.setup();
                
            }
        }

        private void serialSense(object sender, RoutedEventArgs e) {
            if (!windowsIot)
                return;

            byte[] data = { (byte) MicroController.Command.Sense, 0x00 };
            while (!mc.serialInitialized);
            mc.transmitCommand(data);
            mc.waitForResponse();
        }

        private void serialLock(object sender, RoutedEventArgs e) {
            if (!windowsIot)
                return;

            byte[] data = { (byte) MicroController.Command.Lock, 0x00 };
            while (!mc.serialInitialized);
            mc.transmitCommand(data);
            mc.waitForResponse();
        }

        private void serialUnlock(object sender, RoutedEventArgs e) {
            if (!windowsIot)
                return;

            byte[] data = { (byte) MicroController.Command.Unlock, 0x00 };
            while (!mc.serialInitialized);
            mc.transmitCommand(data);
            mc.waitForResponse();
        }

        private void serialTemperatureCheck(object sender, RoutedEventArgs e) {
            if (!windowsIot)
                return;

            byte[] data = { (byte) MicroController.Command.TemperatureCheck, 0x01, 0x07 };
            while (!mc.serialInitialized);
            mc.transmitCommand(data);
            mc.waitForResponse();
        }

        private void serialDispense(object sender, RoutedEventArgs e) {
            if (!windowsIot)
                return;

            byte[] data = { (byte) MicroController.Command.Dispense, 0x01, 0x00 };
            while (!mc.serialInitialized);
            mc.transmitCommand(data);
            mc.waitForResponse();
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
        
        private void initializePromotionTimer() {
            DispatcherTimer promotionTimer = new DispatcherTimer();
            promotionTimer.Interval = TimeSpan.FromMilliseconds(promotionMsPerTick);
            promotionTimer.Tick += promotionTimerTick;
            promotionTimer.Start();
        }

        static int currentPromotionSource = 0;

        private void promotionTimerTick(object sender, object e) {
            if (mainModel.promotionTimerTickCounter >= msUntilPromotionMediaSwitch) {
                mainModel.promotionTimerTickCounter = 0;
            } else {
                mainModel.promotionTimerTickCounter += (int) promotionMsPerTick;
            }

            // This "preload" promotion source has been added to stop the screen from flickering when loading the next source
            if (mainModel.promotionTimerTickCounter == (msUntilPromotionMediaSwitch - 2000)) {
                currentPromotionSource = (currentPromotionSource + 1) % mainModel.promotionMedia.Count;         // Update which promotion source to show
                mainModel.promotionSourcePreload = (MainModel.promotionMediaName) (currentPromotionSource);     // Load the preload promotion source
            }

            if (mainModel.promotionTimerTickCounter >= msUntilPromotionMediaSwitch) {
                mainModel.promotionSource = (MainModel.promotionMediaName) (currentPromotionSource);            // Load the promotion source
            }
        }
        
        private void ledTick(object sender, object e) {
            if (!windowsIot)
                return;

            if (led.Read() == GpioPinValue.Low) {
                led.Write(GpioPinValue.High);
            } else {
                led.Write(GpioPinValue.Low);
            }
        }
        

    }
}
