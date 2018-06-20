using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Gpio;
using System.Threading;
using Windows.System.Profile;
using System.Collections.Generic;
using Windows.UI.Core;
using System.Diagnostics;
using Windows.Storage;
using System.Threading.Tasks;

namespace BidonDispenser {
    public sealed partial class MainPage: Page {
        private MainModel mainModel = new MainModel();

        private Boolean windowsIot = false;
        private Boolean setupError = false;
        private int columnAmount = 0;
        
        private MicroController mc = null;


        public MainPage() {
            InitializeComponent();
            
            // Add the "unloadMainPage" function to the callbacks when the program is shutting down
            Unloaded += unloadMainPage;

            // Check on which device we're running
            Debug.WriteLine("Running on "+AnalyticsInfo.VersionInfo.DeviceFamily);
            if (AnalyticsInfo.VersionInfo.DeviceFamily == "Windows.IoT") windowsIot = true;

            // Initialize the promotion timer
            initializePromotionTimer();

            // If running on windows iot...
            if (windowsIot) {

                // Initialize the microcontroller and start the sense task
                mc = new MicroController();
                
                // Initialize the system when the program has loaded
                Loaded += async (sender, eventArgs) => { await initializeSystem(); };
                
                return;
            }
        }

        private async Task initializeSystem() {
            try {

                // Initialize the microcontroller
                await mc.initialize();

                // Sense the microcontroller
                if (!(await mc.sense())) setupError = true;

                // Initialize the LEDs and turn them off
                initializeLeds();
                
                // Check how many columns are hooked up
                columnAmount = howManyColumnsAreThere();
                Debug.WriteLine("There are " + columnAmount + " Columns");
                if (columnAmount == 0) setupError = true;

                // Initialize the door sensor
                initDoorSensor();
                doorValueHasChanged(null, null);

                // Only initialize the buttons if nothing went wrong
                //      By not initializing the buttons when something went wrong, we can ensure the user cannot interact with the machine in any way
                if (!setupError)
                    initButtons(columnAmount);
                else
                    showBootingErrorPanel();

            } catch (Exception ex) {
                Debug.WriteLine("EXCEPTION: " + ex.Message + "\n" + ex.StackTrace);
            }
        }


        // Test Related //////

        private Boolean stopCurrentTest = false;
        
        private void serialTest(object sender, RoutedEventArgs rea) {
            Debug.WriteLine("Click: " + ((Button) sender).Name );

            if (!windowsIot)
                return;
            
            switch (((Button) sender).Name) {
                case "Sense":               mc.sendSenseCommand();              break;
                case "Lock":                mc.sendLockCommand();               break;
                case "Unlock":              mc.sendUnlockCommand();             break;
                case "Temperature":         mc.sendTemperatureCommand();        break;
                case "Dispense":            mc.sendDispenseCommand(0);          break;
                case "Distance":            mc.sendDistanceCommand();           break;
                default: Debug.WriteLine("Unknown button"); return;
            }
        }

        private async void wbTest(object sender, RoutedEventArgs rea) {
            Debug.WriteLine("Click: " + ((Button) sender).Name );

            if (!windowsIot)
                return;
            
            switch (((Button) sender).Name) {
                case "DispenseTest":            dispenseTest();                 break;
                case "CoolingTest":             coolingTest();                  break;
                case "Stop":                    stopCurrentTest = true;         break;
                default: Debug.WriteLine("Unknown button"); return;
            }
        }

        private async void coolingTest() {

            Debug.WriteLine("Starting the cooling test");

            try {
                while (!stopCurrentTest) {
                    orangeLedState(GpioPinValue.High);
                    StorageFolder usb = (await KnownFolders.RemovableDevices.GetFoldersAsync())[0];
                    StorageFile logFile = await usb.CreateFileAsync("log.txt", CreationCollisionOption.OpenIfExists);

                    var result = await mc.sendTemperatureCommand();

                    if (result.Item2.Count > 4) {
                        // Log the temperatures on the USB
                        String data = ((double) result.Item2[2]) / 5.0 + "," + ((double) result.Item2[3]) / 5.0 + "," + ((double) result.Item2[4]) / 5.0 + "\n";
                        await FileIO.AppendTextAsync(logFile, data);

                        // Update the temperature in the UI
                        double lowerTemp = result.Item2[2];
                        mainModel.lowerTemperature = lowerTemp / 5.0;
                    }

                    orangeLedState(GpioPinValue.Low);
                    Thread.Sleep(60 * 1000);    // One minutes
                }

                stopCurrentTest = false;

            } catch (Exception e) {
                Debug.WriteLine("EXCEPTION CATCHED: "+e.Message);
            } finally {
                orangeLedState(GpioPinValue.Low);
            }

            Debug.WriteLine("Stopped the cooling test");
        }

        private async void dispenseTest() {

            Debug.WriteLine("Starting the dispense test");
            byte columnIndex = 0;

            try {
                while (!stopCurrentTest) {
                    redLedState(GpioPinValue.High);

                    await mc.sendDispenseCommand(columnIndex++);

                    if (columnIndex > 7) columnIndex = 0;

                    redLedState(GpioPinValue.Low);
                    Thread.Sleep(1000);         // One second
                }

                stopCurrentTest = false;

            } catch (Exception e) {
                Debug.WriteLine("EXCEPTION CATCHED: " + e.Message);
            } finally {
                redLedState(GpioPinValue.Low);
            }

            Debug.WriteLine("Stopped the dispense test");
        }


        // LEDs //////

        private readonly int ORANGELED_PIN = 27;
        private readonly int REDLED_PIN = 22;
        private GpioPin orangeLed;
        private GpioPin redLed;

        private Boolean initializeLeds() {
            GpioController gpio = GpioController.GetDefault();                                          // Get the default Gpio Controller

            if (gpio == null) {
                Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            orangeLed = gpio.OpenPin(ORANGELED_PIN);
            redLed = gpio.OpenPin(REDLED_PIN);

            orangeLed.SetDriveMode(GpioPinDriveMode.Output);
            redLed.SetDriveMode(GpioPinDriveMode.Output);

            orangeLed.Write(GpioPinValue.Low);
            redLed.Write(GpioPinValue.Low);

            Debug.WriteLine("The LEDs have been initialized");
            return true;
        }

        private void orangeLedState(GpioPinValue val) {
            orangeLed?.Write(val);
        }

        private void redLedState(GpioPinValue val) {
            redLed?.Write(val);
        }
        
        
        // Buttons //////

        private Boolean buttonsDisabled = false;
        private readonly int[] BUTTON_PINS = { 20, 21, 26, 16, 19, 13, 12, 6 };
        private List<GpioPin> buttonPins = new List<GpioPin>();

        private Boolean initButtons(int amount) {
            GpioController gpio = GpioController.GetDefault();                                          // Get the default Gpio Controller

            if (gpio == null) {
                Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            if (amount > 8)         amount = 8;                                                         // Max amount of eight
            else if (amount < 0)    amount = 0;                                                         // Min amount of zero
            
            for (int i = 0; i < amount; i++) {
                buttonPins.Add(gpio.OpenPin(BUTTON_PINS[i]));                                           // Open the button pins

                // Set the buttons' drive mode to input and pullup (if supported)
                if (buttonPins[i].IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                    buttonPins[i].SetDriveMode(GpioPinDriveMode.InputPullUp);
                else
                    buttonPins[i].SetDriveMode(GpioPinDriveMode.Input);

                buttonPins[i].DebounceTimeout = TimeSpan.FromMilliseconds(50);                          // Set a debounce timeout of 50ms
                buttonPins[i].ValueChanged += buttonValueHasChanged;                                    // Add a callback
            }

            Debug.WriteLine(amount+" buttons have been initialized");
            return true;
        }

        private void buttonValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {

            if (e.Edge == GpioPinEdge.FallingEdge) {

                if (buttonsDisabled) return;
                buttonsDisabled = true;

                int buttonNo = buttonPins.IndexOf(sender);
                Debug.WriteLine("Button " + buttonNo + " has been pressed");

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
                        if (buttonNo == 0) {
                            showPickColourPanel();
                        } else if (buttonNo == 7) { 
                            mc.sendDispenseCommand((byte) mainModel.selectedBottleColour);
                            showThankYouPanel();
                        } else {
                            var huh = true;
                            for (int i = 1; i < (columnAmount - 1); i++) huh &= (buttonPins[i].Read() == GpioPinValue.Low);
                            if ( huh ) showSecretPanel();
                        }
                        break;
                    
                    
                    case uiPanel.thankYou:
                        // Return to the selection screen
                        stopThankYouTimer(null, null);
                        break;

                    case uiPanel.doorOpen:
                        // Do nothing when the error screen is shown
                        showDoorOpenErrorPanel();
                        break;

                    case uiPanel.secret:
                        // Huh?
                        showFinishingUpPanel();
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
                Debug.WriteLine("There is no Gpio controller on this device");
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

            Debug.WriteLine("The misc gpio has been initialized");
            return true;
        }

        private void doorValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {
            GpioPinValue pinVal = doorSensorPin.Read();

            if (pinVal == GpioPinValue.High) {
                doNotShowTheThankYouPanel();
                showDoorOpenErrorPanel();
                mc.sendLockCommand();
            } else {
                mc.sendUnlockCommand();
                showPickColourPanel();
            }
        }


        // Column Amount Selector Jumper //////

        private readonly int COLUMNSELECTOR_PIN = 23;
        
        private int howManyColumnsAreThere() {
            GpioController gpio = GpioController.GetDefault();                      // Get the default Gpio Controller

            if (gpio != null) {

                GpioPin columnSelectorPin = gpio.OpenPin(COLUMNSELECTOR_PIN);       // Open the column selector pin
                columnSelectorPin.SetDriveMode(GpioPinDriveMode.Input);             // Set the pin to input

                GpioPinValue pinVal = columnSelectorPin.Read();                     // Read the pin value
                columnSelectorPin.Dispose();                                        // Dispose the pin again

                if (pinVal == GpioPinValue.High)                                    // High = 4 columns
                    return 4;
                else if (pinVal == GpioPinValue.Low)                                // Low = 8 columns
                    return 8;
                else
                    return 0;                                                       // Err = 0 columns

            } else {
                Debug.WriteLine("There is no Gpio controller on this device");
                return 0;                                                           // Err = 0 columns
            }
        }
        

        // Panel Show //////

        private enum uiPanel {
            pickColour, finishingUp, thankYou, doorOpen, boot, secret
        }
        private uiPanel currentPanel = uiPanel.pickColour;

        private void showCommandTestPanel() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: CommandTestPanel");
                CommandTestPanel.Visibility = Visibility.Visible;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenError.Visibility = Visibility.Collapsed;
                BootingError.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showPickColourPanel() {
            currentPanel = uiPanel.pickColour;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: PickColourPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Visible;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenError.Visibility = Visibility.Collapsed;
                BootingError.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showFinishingUpPanel() {
            currentPanel = uiPanel.finishingUp;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: FinishingUpPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Visible;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenError.Visibility = Visibility.Collapsed;
                BootingError.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showThankYouPanel() {
            currentPanel = uiPanel.thankYou;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: ThankYouPanel");
                startThankYouTimer();
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Visible;
                DoorOpenError.Visibility = Visibility.Collapsed;
                BootingError.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showDoorOpenErrorPanel() {
            currentPanel = uiPanel.doorOpen;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: DoorOpenErrorPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenError.Visibility = Visibility.Visible;
                BootingError.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showBootingErrorPanel() {
            currentPanel = uiPanel.boot;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: BootingErrorPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenError.Visibility = Visibility.Collapsed;
                BootingError.Visibility = Visibility.Visible;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showSecretPanel() {
            currentPanel = uiPanel.secret;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: DoorOpenErrorPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenError.Visibility = Visibility.Collapsed;
                BootingError.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Visible;
            });
        }


        // Thank You Timer //////

        private DispatcherTimer thankYouTimer;
        
        private void startThankYouTimer() {
            thankYouTimer = new DispatcherTimer();
            thankYouTimer.Interval = TimeSpan.FromSeconds(3);
            thankYouTimer.Tick += stopThankYouTimer;
            thankYouTimer.Start();
        }

        private void stopThankYouTimer(object sender, object e) {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                if (thankYouTimer != null) {
                    thankYouTimer.Stop();
                    thankYouTimer = null;
                    showPickColourPanel();
                }
            });
        }

        private void doNotShowTheThankYouPanel() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                if (thankYouTimer != null) {
                    thankYouTimer.Stop();
                    thankYouTimer = null;
                }
            });
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

            Debug.WriteLine("The promotion timer has been initialized");
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
        private const int maintenanceMinutesPerTick = 10;

        private void initializeMaintenanceTimer() {
            maintenanceTimer = new DispatcherTimer();
            maintenanceTimer.Interval = TimeSpan.FromMinutes(maintenanceMinutesPerTick);
            maintenanceTimer.Tick += maintenanceTimerTick;
            maintenanceTimer.Start();

            Debug.WriteLine("The maintenance timer has been initialized");
        }

        private async void maintenanceTimerTick(object sender, object e) {

            Tuple<int, List<Byte>> result;
            Boolean retry = false;
            Boolean hasRetried = false;

            do {
                if (retry) hasRetried = true;                   // Update the "hasTried" variable
                result = await mc.sendLockCommand();            // Get the result
                
                switch (result.Item1) {

                    case 0:  retry = MicroController.isImportantException(result.Item2[0]); break;      // If there was an excpetion in the physical microcontroller => retry
                    case 1:  retry = false; break;                                                      // If the serial port was not initialized => stop
                    case 2:  retry = true;  break;                                                      // If there was an exception in the microcontroller class => retry
                    case 3:  retry = true;  break;                                                      // If the mutex has timed out => retry
                    default: retry = false; break;                                                      // Should not be possible
                }
            } while (retry && !hasRetried);

            if ((result.Item1 == 0) && (result.Item2.Count > 2)) {
                mainModel.lowerTemperature = result.Item2[2] / 5;                                       // Update the lower temperature
            }
        }
        
        
        // Program shutdown //////

        private void unloadMainPage(object sender, object args) {

            // Dispose the microcontroller
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
