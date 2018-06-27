using System;
using System.Threading;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Collections.Generic;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Gpio;
using Windows.System.Profile;
using Windows.Storage;

namespace BidonDispenser {
    public sealed partial class MainPage: Page {
        private MainModel mainModel = new MainModel();

        private Boolean windowsIot = false;
        private Boolean setupError = false;
        private int columnAmount = 0;
        private int emptyDistance = 109;
        
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

                // Initialize the LEDs and turn them off
                initializeLeds();
                
                // Check how many columns are hooked up
                columnAmount = howManyColumnsAreThere();
                Debug.WriteLine("There are " + columnAmount + " Columns");
                if (columnAmount == 0) setupError = true;

                // Sense the microcontroller, if it failed => retry once
                if (!(await mc.sense())) {
                    Thread.Sleep(500);

                    if (!(await mc.sense())) {
                        setupError = true;
                        Debug.WriteLine("Could not sense the microcontroller");
                    }
                }

                // Initialize the door sensor and use the "doorValueHasChanged" method to initalize certain sub-parts of the system
                initDoorSensor();
                doorValueHasChanged(null, null);
                

                // Only initialize the buttons if nothing went wrong
                //      By not initializing the buttons when something went wrong, we can ensure the user cannot interact with the machine in any way
                // But if something went wrong => show the "booting error" panel
                if (!setupError) {
                    initButtons(columnAmount);
                    Thread.Sleep(500);
                    if (currentPanel != uiPanel.doorOpenError)
                        showPickColourPanel();

                } else {
                    showBootingErrorPanel();
                }

            } catch (Exception ex) {
                Debug.WriteLine("EXCEPTION: " + ex.Message + "\n" + ex.StackTrace);
            }
        }


        // Test Related //////
        
        private void serialTest(object sender, RoutedEventArgs rea) {
            Debug.WriteLine("Click: " + ((Button) sender).Name );

            if (!windowsIot)
                return;
            
            switch (((Button) sender).Name) {
                case "Sense":               mc.sendSenseCommand();                          break;
                case "Lock":                mc.sendLockCommand();                           break;
                case "Unlock":              mc.sendUnlockCommand();                         break;
                case "Temperature":         mc.sendTemperatureCommand();                    break;
                case "Dispense":            mc.sendDispenseCommand(0);                      break;
                case "Distance":            mc.sendDistanceCommand((byte) emptyDistance);   break;
                default: Debug.WriteLine("Unknown button"); return;
            }
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
                        if (mainModel.isBottleAvailable(buttonNo)) {
                            // Update which colour has been selected
                            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                                mainModel.selectedBottleColour = (MainModel.bottleColourName) buttonNo;
                            });
                        
                            // Show the "finishing up" panel
                            showFinishingUpPanel();

                        }
                        break;
                    
                    
                    case uiPanel.finishingUp:
                        // Cancel the operation if the user presses the left most button
                        if (buttonNo == 0) {
                            showPickColourPanel();
                        } else if (buttonNo == 7) { 
                            dispenseBottle((byte) mainModel.selectedBottleColour);
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


                    case uiPanel.secret:
                        // Huh?
                        showFinishingUpPanel();
                        break;
                    
                    default: break;
                }

                buttonsDisabled = false;
            }
        }
        

        // Columns related //////

        private void setColumnEmptyStatus(byte byteVar) {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                mainModel.bottleOutOfStock = byteVar;
            });
        }

        private async Task updateColumnEmptyStatus() {
            var distanceResult = await mc.sendDistanceCommand((byte) emptyDistance);

            if ((distanceResult.Item1 == 0) && (distanceResult.Item2[1] == 1) && (distanceResult.Item2.Count > 2)) {
                setColumnEmptyStatus(distanceResult.Item2[2]);

            } else {
                Debug.WriteLine("An error occurred while checking for the empty status of the columns");
            }
        }
        
        private async void dispenseBottle(byte columnNo) {
            var distanceResult = await mc.sendDispenseCommand(columnNo);

            if ((distanceResult.Item1 == 0) && (distanceResult.Item2[1] == 1) && (distanceResult.Item2.Count > 2)) {
                setColumnEmptyStatus(distanceResult.Item2[2]);
            } else {
                Debug.WriteLine("An error occurred while checking for the empty status of the columns after the dispensing");
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

        private async void doorValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {
            if (currentPanel == uiPanel.bootingError)
                return;

            GpioPinValue pinVal = doorSensorPin.Read();

            if (pinVal == GpioPinValue.High) {
                doNotShowTheThankYouPanel();
                showDoorOpenErrorPanel();
                await mc.sendLockCommand();
                stopMaintenanceTimer();
            } else {
                await mc.sendUnlockCommand();
                await updateColumnEmptyStatus();

                if (currentPanel == uiPanel.doorOpenError)
                    showPickColourPanel();

                var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                    maintenanceTimerTick(null, null);
                    initializeMaintenanceTimer();
                });
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
            booting, pickColour, finishingUp, thankYou, doorOpenError, bootingError, secret
        }
        private uiPanel currentPanel = uiPanel.booting;

        private void showCommandTestPanel() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: CommandTestPanel");
                CommandTestPanel.Visibility = Visibility.Visible;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showBootingPanel() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: BootingPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Visible;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showPickColourPanel() {
            currentPanel = uiPanel.pickColour;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: PickColourPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Visible;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showFinishingUpPanel() {
            currentPanel = uiPanel.finishingUp;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: FinishingUpPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Visible;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showThankYouPanel() {
            currentPanel = uiPanel.thankYou;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: ThankYouPanel");
                startThankYouTimer();
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Visible;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showDoorOpenErrorPanel() {
            currentPanel = uiPanel.doorOpenError;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: DoorOpenErrorPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Visible;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showBootingErrorPanel() {
            currentPanel = uiPanel.bootingError;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: BootingErrorPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Visible;
                SecretPanel.Visibility = Visibility.Collapsed;
            });
        }

        private void showSecretPanel() {
            currentPanel = uiPanel.secret;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                Debug.WriteLine("Showing: DoorOpenErrorPanel");
                CommandTestPanel.Visibility = Visibility.Collapsed;
                BootingPanel.Visibility = Visibility.Collapsed;
                PickColourPanel.Visibility = Visibility.Collapsed;
                FinishingUpPanel.Visibility = Visibility.Collapsed;
                ThankYouPanel.Visibility = Visibility.Collapsed;
                DoorOpenErrorPanel.Visibility = Visibility.Collapsed;
                BootingErrorPanel.Visibility = Visibility.Collapsed;
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
        static int currentPromotionSource = 0;

        private void initializePromotionTimer() {
            promotionTimer = new DispatcherTimer();
            promotionTimer.Interval = TimeSpan.FromMilliseconds(promotionMsPerTick);
            promotionTimer.Tick += promotionTimerTick;
            promotionTimer.Start();

            Debug.WriteLine("The promotion timer has been initialized");
        }
        
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
        private const int maintenanceMinutesPerTick = 5;

        private void initializeMaintenanceTimer() {
            maintenanceTimer = new DispatcherTimer();
            maintenanceTimer.Interval = TimeSpan.FromMinutes(maintenanceMinutesPerTick);
            maintenanceTimer.Tick += maintenanceTimerTick;
            maintenanceTimer.Start();

            Debug.WriteLine("The maintenance timer has been initialized");
        }

        private async void maintenanceTimerTick(object sender, object e) {

            var distanceResult = await mc.sendTemperatureCommand();

            if ((distanceResult.Item1 == 0) && (distanceResult.Item2[1] == 3) && (distanceResult.Item2.Count > 4)) {
                mainModel.lowerTemperature = ((float) distanceResult.Item2[2]) / 5;

                logData("logFile", 
                    string.Format("{0:00.0}", (((float) distanceResult.Item2[2]) / 5)) + "," + 
                    string.Format("{0:00.0}", (((float) distanceResult.Item2[3]) / 5)) + "," + 
                    string.Format("{0:00.0}", (((float) distanceResult.Item2[4]) / 5)) + "\n"
                );

            } else {
                Debug.WriteLine("An error occurred while updating the temperature ");
            }
        }
        
        private async void logData(String documentName, String text) {
            try {
                StorageFile logFile = await KnownFolders.DocumentsLibrary.CreateFileAsync(documentName + ".txt", CreationCollisionOption.OpenIfExists);
                await FileIO.AppendTextAsync(logFile, text);

            } catch (Exception e) {
                Debug.WriteLine("An exception occurred while logging the data: "+e.Message+"\n"+e.StackTrace);
            }
        }

        private void stopMaintenanceTimer() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                if (maintenanceTimer != null) {
                    maintenanceTimer.Stop();
                    maintenanceTimer = null;
                }
            });
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
