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
using System.Collections.Generic;
using Windows.UI.Core;

namespace BidonDispenser {
    public sealed partial class MainPage: Page {
        private MainModel mainModel = new MainModel();

        private Boolean windowsIot = false;
        private Boolean setupError = false;
        private int columnAmount = 0;

        //private Pn532Software nfcModule;
        private Pn532 nfcModule;
        private MicroController mc = null;

        public MainPage() {
            this.InitializeComponent();

            // Add the "unloadMainPage" function to the callbacks when the program is shutting down
            Unloaded += unloadMainPage;

            // Check on which device we're running
            System.Diagnostics.Debug.WriteLine("Running on "+AnalyticsInfo.VersionInfo.DeviceFamily);
            if (AnalyticsInfo.VersionInfo.DeviceFamily == "Windows.IoT") {
                windowsIot = true;
            }

            initializePromotionTimer();
            
            if (windowsIot) {
                mc = new MicroController();

                columnAmount = howManyColumnsAreThere();

                // TODO: Do change the UI according to the amount of columns

                if (!initButtons(columnAmount))
                    setupError = true;

                


                
                
                // Initialize the NFC module
                //nfcModule = new Pn532Software();
                //nfcModule = new Pn532(0);
                //nfcModule.setup();
            }
        }


        // Serial Test //////

        private void serialTest(object sender, RoutedEventArgs rea) {
            System.Diagnostics.Debug.WriteLine("Click: " + ((Button) sender).Name );

            if (!windowsIot)
                return;
            
            switch (((Button) sender).Name) {
                case "Sense":               mc.sendSenseCommand();              break;
                case "Lock":                mc.sendLockCommand();               break;
                case "Unlock":              mc.sendUnlockCommand();             break;
                case "Temperature":         mc.sendTemperatureCommand();        break;
                case "Dispense":            mc.sendDispenseCommand();           break;
                case "Distance":            mc.sendDistanceCommand();           break;
                default: System.Diagnostics.Debug.WriteLine("Unknown button"); return;
            }
        }


        // Buttons //////

        private Boolean buttonsDisabled = false;
        private readonly int[] BUTTON_PINS = { 20, 21, 26, 16, 19, 13, 12, 6 };
        private List<GpioPin> buttonPins = new List<GpioPin>();

        private Boolean initButtons(int amount) {
            GpioController gpio = GpioController.GetDefault();                                          // Get the default Gpio Controller

            if (gpio == null) {
                System.Diagnostics.Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            if (amount > 8)         amount = 8;                                                         // Max amount of eight
            else if (amount < 0)    amount = 0;                                                         // Min amount of zero
            
            for (int i = 0; i < amount; i++) {
                buttonPins.Add(gpio.OpenPin(BUTTON_PINS[i]));                                           // Open the button pins

                // Set the buttons' drive mode to input and pulldown (if supported)
                if (buttonPins[i].IsDriveModeSupported(GpioPinDriveMode.InputPullDown))
                    buttonPins[i].SetDriveMode(GpioPinDriveMode.InputPullDown);
                else
                    buttonPins[i].SetDriveMode(GpioPinDriveMode.Input);

                buttonPins[i].DebounceTimeout = TimeSpan.FromMilliseconds(50);                          // Set a debounce timeout of 50ms
                buttonPins[i].ValueChanged += buttonValueHasChanged;                                    // Add a callback
            }

            System.Diagnostics.Debug.WriteLine(amount+" buttons have been initialized");
            return true;
        }

        private void buttonValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {

            if (e.Edge == GpioPinEdge.RisingEdge) {

                if (buttonsDisabled) return;
                buttonsDisabled = true;

                int buttonNo = buttonPins.IndexOf(sender);
                System.Diagnostics.Debug.WriteLine("Button " + buttonNo + " has been pressed");

                switch (currentPanel) {

                    case uiPanel.pickColour:
                        // Update which colour has been selected
                        var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                            mainModel.selectedBottleColour = (MainModel.bottleColourName) buttonNo;
                        });
                        
                        // Show the "finishing up" panel
                        showFinishingUpPanel();
                        break;
                    
                    
                    case uiPanel.finishingUp:
                        // Cancel the operation if the user presses the left most button
                        if (buttonNo == 0)
                            showPickColourPanel();
                        break;
                    
                    
                    case uiPanel.thankYou:
                        // Return to the selection screen
                        stopThankYouTimer(null, null);
                        break;
                    
                    
                    default: break;
                }

                buttonsDisabled = false;
            }
        }


        // Door Sensor //////

        private readonly int DOOR_PIN = 5;
        private GpioPin doorSensorPin;
        
        private Boolean initDoorSensor() {
            GpioController gpio = GpioController.GetDefault();                                          // Get the default Gpio Controller

            if (gpio == null) {
                System.Diagnostics.Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            doorSensorPin = gpio.OpenPin(DOOR_PIN);                                                     // Open the door sensor pin

            // Set the doorpin's drive mode to input and pullup (if supported)
            if (doorSensorPin.IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                doorSensorPin.SetDriveMode(GpioPinDriveMode.InputPullUp);
            else
                doorSensorPin.SetDriveMode(GpioPinDriveMode.Input);

            doorSensorPin.DebounceTimeout = TimeSpan.FromMilliseconds(50);                              // Set a debounce timeout of 50ms
            doorSensorPin.ValueChanged += doorValueHasChanged;                                          // Add a callback

            System.Diagnostics.Debug.WriteLine("The misc gpio has been initialized");
            return true;
        }

        private async void doorValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {
            GpioPinValue pinVal = doorSensorPin.Read();

            if (pinVal == GpioPinValue.Low) {
                int status = -1;

                do {

                    status = await mc.sendUnlockCommand();

                } while (status == 2);                                  // Send the unlock command until succesfull

                // Should we execute the mainenance check and restart the maintenance timer here?
                //maintenanceTimer.Stop();
                //maintenanceTimerTick(null, null);
                //initializeMaintenanceTimer();

            } else { 
                while ((await mc.sendLockCommand()) == 2);              // Send the lock command until succesfull
            }
        }


        // Column Amount Selector Jumper //////

        private readonly int COLUMNSELECTOR_PIN = 23;
        
        private int howManyColumnsAreThere() {
            GpioController gpio = GpioController.GetDefault();                  // Get the default Gpio Controller

            if (gpio != null) {

                GpioPin columnSelectorPin = gpio.OpenPin(COLUMNSELECTOR_PIN);   // Open the column selector pin
                columnSelectorPin.SetDriveMode(GpioPinDriveMode.Input);         // Set the pin to input

                GpioPinValue pinVal = columnSelectorPin.Read();                 // Read the pin value
                columnSelectorPin.Dispose();                                    // Dispose the pin again

                if (pinVal == GpioPinValue.High)                                // High = 4 columns
                    return 4;
                else if (pinVal == GpioPinValue.Low)                            // Low = 8 columns
                    return 8;
                else
                    return 0;                                                   // Err = 0 columns

            } else {
                System.Diagnostics.Debug.WriteLine("There is no Gpio controller on this device");
                return 0;                                                       // Err = 0 columns
            }
        }


        // Commands //////

        private async void sendCommand(MicroController.Command command) {

            Task<int> commandTask = null;

            switch (command) {
                case MicroController.Command.Sense:             commandTask = mc.sendSenseCommand();            break;
                case MicroController.Command.Lock:              commandTask = mc.sendLockCommand();             break;
                case MicroController.Command.Unlock:            commandTask = mc.sendUnlockCommand();           break;
                case MicroController.Command.Temperature:       commandTask = mc.sendTemperatureCommand();      break;
                case MicroController.Command.Dispense:          commandTask = mc.sendDispenseCommand();         break;
                case MicroController.Command.Distance:          commandTask = mc.sendDistanceCommand();         break;
            }

            if (commandTask == null)
                return;

            int status = await commandTask;

            // LEFT OFF HERE

        }
        
        
        // Panel Show //////

        private enum uiPanel {
            pickColour, finishingUp, thankYou
        }
        private uiPanel currentPanel = uiPanel.pickColour;

        private void showCommandTestPanel() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                CommandTestPanel.Visibility = Visibility.Visible;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                ThankYouFamPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showPickColourPanel() {
            currentPanel = uiPanel.pickColour;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Visible;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                ThankYouFamPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showFinishingUpPanel() {
            currentPanel = uiPanel.finishingUp;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Visible;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                ThankYouFamPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showThankYouPanel() {
            currentPanel = uiPanel.thankYou;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                startThankYouTimer();
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Visible;
                ThankYouFamPanel.Visibility = Visibility.Collapsed;
            });
        }


        // Thank You Timer //////

        private DispatcherTimer thankYouTimer;
        
        private void startThankYouTimer() {
            thankYouTimer = new DispatcherTimer();
            thankYouTimer.Interval = TimeSpan.FromSeconds(5);
            thankYouTimer.Tick += stopThankYouTimer;
            thankYouTimer.Start();
        }

        private void stopThankYouTimer(object sender, object e) {
            thankYouTimer.Stop();
            showPickColourPanel();
        }


        // Promotion Timer //////

        private DispatcherTimer promotionTimer;
        private const Double promotionMsPerTick = 200;
        private const int msUntilPromotionMediaSwitch = 30_000;

        private void initializePromotionTimer() {
            promotionTimer = new DispatcherTimer();
            promotionTimer.Interval = TimeSpan.FromMilliseconds(promotionMsPerTick);
            promotionTimer.Tick += promotionTimerTick;
            promotionTimer.Start();

            System.Diagnostics.Debug.WriteLine("The promotion timer has been initialized");
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


        // Maintenance Timer //////

        private DispatcherTimer maintenanceTimer;
        private const int maintenanceMinutesPerTick = 15;

        private void initializeMaintenanceTimer() {
            maintenanceTimer = new DispatcherTimer();
            maintenanceTimer.Interval = TimeSpan.FromMinutes(maintenanceMinutesPerTick);
            maintenanceTimer.Tick += maintenanceTimerTick;
            maintenanceTimer.Start();

            System.Diagnostics.Debug.WriteLine("The maintenance timer has been initialized");
        }

        private async void maintenanceTimerTick(object sender, object e) {
            int status = await mc.sendTemperatureCommand();                        // Send the temperature check command
            
            // TODO: Decide what to do with each status    
        }
        
        
        // Program shutdown //////

        private void unloadMainPage(object sender, object args) {

            // Dispose the NFC module and the microcontroller
            nfcModule?.dispose();
            mc?.dispose();


            // Dispose all pins
            foreach (GpioPin button in buttonPins)
                button?.Dispose();

            doorSensorPin?.Dispose();


            // Stop all timers
            thankYouTimer?.Stop();
            promotionTimer?.Stop();
            maintenanceTimer?.Stop();
        }

    }
}
