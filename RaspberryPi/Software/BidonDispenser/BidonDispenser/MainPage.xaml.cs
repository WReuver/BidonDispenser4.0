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

        private Boolean windowsIot = false;         // A simple boolean which will indicate whether the program is running on Windows IoT or not
        private Boolean setupError = false;         // A simple boolean which will indicate when something went wrong while initializing the system
        private int columnAmount = 0;               // The amount of columns that are connected, the amount will be read later on in the system initialization
        private int emptyDistance = 109;            // From which distance a column should be considered as empty
        private MicroController mc = null;          // A reference to the soon to be created microcontroller object

        
        // Constructor
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

        // Asynchronous Initialization Method
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
        
        // This method is used to test all of the commands the raspberry pi can send to the microcontroller, this method will only be called by the command test panel
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

        private readonly int ORANGELED_PIN = 27;            // The pin number the orange led is connected to
        private readonly int REDLED_PIN = 22;               // The pin number the red led is connected to
        private GpioPin orangeLed;                          // A reference where the orangeled object will be stored in
        private GpioPin redLed;                             // A reference where the redled object will be stored in

        // Initialize the LEDs
        private Boolean initializeLeds() {
            GpioController gpio = GpioController.GetDefault();                          // Get the default Gpio Controller

            if (gpio == null) {                                                         // Check whether the device has a Gpio Controller
                Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            // Open the LED pins
            orangeLed = gpio.OpenPin(ORANGELED_PIN);
            redLed = gpio.OpenPin(REDLED_PIN);

            // Set the drive mode of both the LED pins
            orangeLed.SetDriveMode(GpioPinDriveMode.Output);
            redLed.SetDriveMode(GpioPinDriveMode.Output);

            // Write both of the LEDs to low
            orangeLed.Write(GpioPinValue.Low);
            redLed.Write(GpioPinValue.Low);

            Debug.WriteLine("The LEDs have been initialized");
            return true;
        }

        // Set the orange led state
        private void orangeLedState(GpioPinValue val) {
            orangeLed?.Write(val);
        }

        // Set the red led state
        private void redLedState(GpioPinValue val) {
            redLed?.Write(val);
        }
        
        
        // Buttons //////

        private Boolean buttonsDisabled = false;                                                        // A boolean used to make sure only one button press is processed at a time
        private readonly int[] BUTTON_PINS = { 20, 21, 26, 16, 19, 13, 12, 6 };                         // The pin numbers the buttons are connected to
        private List<GpioPin> buttonPins = new List<GpioPin>();                                         // A list which will contain all the gpio pins of the buttons

        // Claim and configure all the buttons
        private Boolean initButtons(int amount) {
            GpioController gpio = GpioController.GetDefault();                                          // Get the default Gpio Controller

            if (gpio == null) {                                                                         // Check whether the device has a Gpio Controller
                Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            if (amount > 8)         amount = 8;                                                         // Max amount of eight
            else if (amount < 0)    amount = 0;                                                         // Min amount of zero
            
            for (int i = 0; i < amount; i++) {
                buttonPins.Add(gpio.OpenPin(BUTTON_PINS[i]));                                           // Open all of the button pins

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

        // Callback method for when a button is pressed
        private void buttonValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {

            if (e.Edge == GpioPinEdge.FallingEdge) {

                if (buttonsDisabled) return;                                                            // If a button is already being processed => return
                buttonsDisabled = true;                                                                 // Set the buttonsDisabled to true

                int buttonNo = buttonPins.IndexOf(sender);                                              // Check which button has been pressed
                Debug.WriteLine("Button " + buttonNo + " has been pressed");

                switch (currentPanel) {                                                                 // Execute an action dependant on which panel is currently showing

                    case uiPanel.pickColour:
                        if (mainModel.isBottleAvailable(buttonNo)) {                                    // If the selected bottle is not available => break
                            // Update which colour has been selected
                            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                                mainModel.selectedBottleColour = (MainModel.bottleColourName) buttonNo;
                            });
                        
                            // Show the "finishing up" panel
                            showFinishingUpPanel();

                        }
                        break;
                    
                    
                    case uiPanel.finishingUp:
                        if (buttonNo == 0) {                                                            // Cancel the operation if the user presses the left most button
                            showPickColourPanel();
                        } else if (buttonNo == 7) {                                                     // Dispense the bottle if the user presses the right most button
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

        // Update the "bottleOutOfStock" variable in the main model
        private void setColumnEmptyStatus(byte byteVar) {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                mainModel.bottleOutOfStock = byteVar;
            });
        }

        // Command the microcontroller to measure the distance of all columns and return the empty status of all of them
        private async Task updateColumnEmptyStatus() {
            var distanceResult = await mc.sendDistanceCommand((byte) emptyDistance);

            if ((distanceResult.Item1 == 0) && (distanceResult.Item2[1] == 1) && (distanceResult.Item2.Count > 2)) {
                setColumnEmptyStatus(distanceResult.Item2[2]);

            } else {
                Debug.WriteLine("An error occurred while checking for the empty status of the columns");
            }
        }
        
        // Command the microcontroller to dispense a bottle, afterwards the microcontroller will return the empty status of all of the columns as well
        private async void dispenseBottle(byte columnNo) {
            var distanceResult = await mc.sendDispenseCommand(columnNo);

            if ((distanceResult.Item1 == 0) && (distanceResult.Item2[1] == 1) && (distanceResult.Item2.Count > 2)) {
                setColumnEmptyStatus(distanceResult.Item2[2]);
            } else {
                Debug.WriteLine("An error occurred while checking for the empty status of the columns after the dispensing");
            }
        }
        

        // Door Sensor //////

        private readonly int DOOR_PIN = 5;                                                      // The pin number the doorSensor is connected to
        private GpioPin doorSensorPin;                                                          // A reference where the doorSensor object will be stored in
        
        // Initialize the door sensor pin
        private Boolean initDoorSensor() {
            GpioController gpio = GpioController.GetDefault();                                  // Get the default Gpio Controller

            if (gpio == null) {                                                                 // Check whether the device has a Gpio Controller
                Debug.WriteLine("There is no Gpio controller on this device");
                return false;
            }

            doorSensorPin = gpio.OpenPin(DOOR_PIN);                                             // Open the door sensor pin

            // Set the doorpin's drive mode to input and pullup (if supported)
            if (doorSensorPin.IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                doorSensorPin.SetDriveMode(GpioPinDriveMode.InputPullUp);
            else
                doorSensorPin.SetDriveMode(GpioPinDriveMode.Input);

            doorSensorPin.DebounceTimeout = TimeSpan.FromMilliseconds(50);                      // Set a debounce timeout of 50ms
            doorSensorPin.ValueChanged += doorValueHasChanged;                                  // Add a callback

            Debug.WriteLine("The misc gpio has been initialized");
            return true;
        }

        // Callback for when the door sensor pin's value has changed
        private async void doorValueHasChanged(GpioPin sender, GpioPinValueChangedEventArgs e) {
            if (currentPanel == uiPanel.bootingError)                                           // If there was a booting error => return
                return;

            GpioPinValue pinVal = doorSensorPin.Read();                                         // Read the pin value

            if (pinVal == GpioPinValue.High) {                                                  // If the door is opened
                doNotShowTheThankYouPanel();                                                    // Turn off the thank you timer (if it is running)
                showDoorOpenErrorPanel();                                                       // Show the "doorOpenError" Panel
                await mc.sendLockCommand();                                                     // Lock the microcontroller
                stopMaintenanceTimer();                                                         // Stop the maintenance timer
            } else {
                await mc.sendUnlockCommand();                                                   // Unlock the microcontroller
                await updateColumnEmptyStatus();                                                // Get the empty status' of all the columns and update the UI accordingly

                if (currentPanel == uiPanel.doorOpenError)                                      // Only show the pick colourpanel if the current panel is the doorOpenError panel since the program can also get here when a boot error occurs
                    showPickColourPanel();

                var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                    maintenanceTimerTick(null, null);                                           // Trick the system in thinking the maintenance timer has reached it's target value
                    initializeMaintenanceTimer();                                               // Start the maintenance timer again
                });
            }
        }


        // Column Amount Selector Jumper //////

        private readonly int COLUMNSELECTOR_PIN = 23;                                           // The pin number the columnSelector is connected to

        // Initialize the column selector pin, check how many columns there are and at last dispose the column selector pin again
        private int howManyColumnsAreThere() {                                                  
            GpioController gpio = GpioController.GetDefault();                                  // Get the default Gpio Controller
                                                                                                
            if (gpio != null) {                                                                 
                                                                                                
                GpioPin columnSelectorPin = gpio.OpenPin(COLUMNSELECTOR_PIN);                   // Open the column selector pin
                columnSelectorPin.SetDriveMode(GpioPinDriveMode.Input);                         // Set the pin to input
                                                                                                
                GpioPinValue pinVal = columnSelectorPin.Read();                                 // Read the pin value
                columnSelectorPin.Dispose();                                                    // Dispose the pin again
                                                                                                
                if (pinVal == GpioPinValue.High)                                                // High = 4 columns
                    return 4;                                                                   
                else if (pinVal == GpioPinValue.Low)                                            // Low = 8 columns
                    return 8;                                                                   
                else                                                                            
                    return 0;                                                                   // Err = 0 columns
                                                                                                
            } else {                                                                            
                Debug.WriteLine("There is no Gpio controller on this device");                  
                return 0;                                                                       // Err = 0 columns
            }
        }
        

        // Panel Show //////

        // Enum used to represent all the UI panels which can be shown
        private enum uiPanel {
            booting, pickColour, finishingUp, thankYou, doorOpenError, bootingError, secret
        }
        private uiPanel currentPanel = uiPanel.booting;                                         // A variable used to keep track of which panel is currently showing

        // Show the command test panel
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

        // Show the booting panel
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

        // Show the pick colour panel
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

        // Show the finishing up panel
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

        // Show the thank you panel
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
        
        // Show the door open error panel
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

        // Show the booting error panel
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

        // Show the very secret panel
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

        private DispatcherTimer thankYouTimer;                                              // A timer used to stop showing the thank you panel after a few seconds once it is shown
        
        // Initialize and start the thank you timer
        private void startThankYouTimer() {
            thankYouTimer = new DispatcherTimer();
            thankYouTimer.Interval = TimeSpan.FromSeconds(3);
            thankYouTimer.Tick += stopThankYouTimer;
            thankYouTimer.Start();
        }

        // Stop the thank you timer and show the pick colour panel
        private void stopThankYouTimer(object sender, object e) {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                if (thankYouTimer != null) {
                    thankYouTimer.Stop();
                    thankYouTimer = null;
                    showPickColourPanel();
                }
            });
        }

        // Stop the thank you timer but do not show the pick colour panel
        private void doNotShowTheThankYouPanel() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                if (thankYouTimer != null) {
                    thankYouTimer.Stop();
                    thankYouTimer = null;
                }
            });
        }


        // Promotion Timer //////

        private DispatcherTimer promotionTimer;                                             // A timer used to update the promotion images and teh progress bar
        private const Double promotionMsPerTick = 200;                                      // At which amount of milliseconds the progressbar should update
        private const int msUntilPromotionMediaSwitch = 30_000;                             // At which amount of milliseconds the promotion image should switch
        static int currentPromotionSource = 0;                                              // Index used to determine which promotion to show next

        // Initialize and start the promotion timer
        private void initializePromotionTimer() {
            promotionTimer = new DispatcherTimer();
            promotionTimer.Interval = TimeSpan.FromMilliseconds(promotionMsPerTick);
            promotionTimer.Tick += promotionTimerTick;
            promotionTimer.Start();

            Debug.WriteLine("The promotion timer has been initialized");
        }
        
        // Callback for the promotion timer
        private void promotionTimerTick(object sender, object e) {
            // Update the promotionTimerTickCounter in the main model
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

        private DispatcherTimer maintenanceTimer;                                           // A timer used to log the temperature and update the lower bottle temperature in the UI
        private const int maintenanceMinutesPerTick = 5;                                    // At which amount of minutes the callback should be called

        // Initialize and start the maintenance timer
        private void initializeMaintenanceTimer() {
            maintenanceTimer = new DispatcherTimer();
            maintenanceTimer.Interval = TimeSpan.FromMinutes(maintenanceMinutesPerTick);
            maintenanceTimer.Tick += maintenanceTimerTick;
            maintenanceTimer.Start();

            Debug.WriteLine("The maintenance timer has been initialized");
        }

        // Callback for the maintenance timer
        private async void maintenanceTimerTick(object sender, object e) {

            // Execute the temperature command
            var distanceResult = await mc.sendTemperatureCommand();

            // Invalid value correction
            for (int i = 1; i < distanceResult.Item2.Count; i++)
                distanceResult.Item2[i] &= 0b01111111;


            if ((distanceResult.Item1 == 0) && (distanceResult.Item2[1] == 3) && (distanceResult.Item2.Count > 4)) {
                // Set the lower temperature in the main model
                mainModel.lowerTemperature = ((float) distanceResult.Item2[2]) / 2;

                // Log the temperature data
                logData("logFile", 
                    string.Format("{0:0.0}", (((float) distanceResult.Item2[2]) / 2)) + "," + 
                    string.Format("{0:0.0}", (((float) distanceResult.Item2[3]) / 2)) + "," + 
                    string.Format("{0:0.0}", (((float) distanceResult.Item2[4]) / 2)) + "\n"
                );

            } else {
                Debug.WriteLine("An error occurred while updating the temperature ");
            }
        }
        
        // Log a string in the documents folder with a given document name
        private async void logData(String documentName, String text) {
            try {
                StorageFile logFile = await KnownFolders.DocumentsLibrary.CreateFileAsync(documentName + ".txt", CreationCollisionOption.OpenIfExists);
                await FileIO.AppendTextAsync(logFile, text);

            } catch (Exception e) {
                Debug.WriteLine("An exception occurred while logging the data: "+e.Message+"\n"+e.StackTrace);
            }
        }

        // Stop the maintenance timer
        private void stopMaintenanceTimer() {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => {
                if (maintenanceTimer != null) {
                    maintenanceTimer.Stop();
                    maintenanceTimer = null;
                }
            });
        }

        
        // Program shutdown //////

        // This method disposes all the used resources once the program will shutdown
        private void unloadMainPage(object sender, object args) {

            // Dispose the microcontroller
            mc?.dispose();


            // Dispose all pins
            foreach (GpioPin button in buttonPins)
                button?.Dispose();

            doorSensorPin?.Dispose();

            orangeLed?.Dispose();
            redLed?.Dispose();


            // Stop all timers
            thankYouTimer?.Stop();
            promotionTimer?.Stop();
            maintenanceTimer?.Stop();
        }

        private void TextBlock_SelectionChanged(object sender, RoutedEventArgs e)
        {

        }

        private void TextBlock_SelectionChanged_1(object sender, RoutedEventArgs e)
        {

        }
    }
}
