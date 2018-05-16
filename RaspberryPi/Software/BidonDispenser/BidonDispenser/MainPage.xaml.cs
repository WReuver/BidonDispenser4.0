using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.Gpio;
using Windows.Devices.Spi;

namespace BidonDispenser
{
    public sealed partial class MainPage : Page
    {
        private MainModel mainModel = new MainModel();

        private const int LED = 3;
        private GpioPin led;

        public MainPage() {
            this.InitializeComponent();
            DispatcherTimer t = new DispatcherTimer();
            t.Interval = TimeSpan.FromSeconds(10);
            t.Tick += T_Tick;
            t.Start();

            DispatcherTimer ledTimer = new DispatcherTimer();
            ledTimer.Interval = TimeSpan.FromSeconds(0.5);
            ledTimer.Tick += ledTick;
            ledTimer.Start();

            initGpio();
        }

        private void initGpio() {
            var gpio = GpioController.GetDefault();

            if (gpio == null) return;

            led = gpio.OpenPin(LED);
            led.SetDriveMode(GpioPinDriveMode.Output);
            led.Write(GpioPinValue.Low);
        }

        static int index = 0;
        private void T_Tick(object sender, object e) {
            index++;
            mainModel.mediaSource = (MainModel.mediaNames)(index % 6);
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
