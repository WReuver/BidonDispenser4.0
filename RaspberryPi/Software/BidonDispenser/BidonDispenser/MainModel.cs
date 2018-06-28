using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace BidonDispenser {
    internal class MainModel : INotifyPropertyChanged {

        // Enum for the promotion media
        public enum promotionMediaName {
            whyJoinThePipe, buyOneGiveOne, cityCleanUp, waterTaps, waterKiosk
        }

        // Enum for the bottle colour names
        public enum bottleColourName {
            blue, yellow, green, orange, red, pink, white, black
        }

        // Dictionary for the promotion media
        private Dictionary<promotionMediaName, String> _promotionMedia = new Dictionary<promotionMediaName, String>() {
            [promotionMediaName.whyJoinThePipe] = "ms-appx:///Assets/Images/Promotions/HappyWaterThingy.png",
            [promotionMediaName.buyOneGiveOne]  = "ms-appx:///Assets/Images/Promotions/BuyOneGiveOne.png",
            [promotionMediaName.cityCleanUp]    = "ms-appx:///Assets/Images/Promotions/CleanUp.png",
            [promotionMediaName.waterTaps]      = "ms-appx:///Assets/Images/Promotions/WaterTaps.png",
            [promotionMediaName.waterKiosk]     = "ms-appx:///Assets/Images/Promotions/WaterKiosk.png"
        };
        public ReadOnlyDictionary<promotionMediaName, String> promotionMedia => new ReadOnlyDictionary<promotionMediaName, String> (_promotionMedia);

        // Dictionary for the bottle colour texts
        private Dictionary<bottleColourName, String> _bottleColourText = new Dictionary<bottleColourName, String>() {
            [bottleColourName.blue]     = "You have selected the blue bottle.",
            [bottleColourName.yellow]   = "You have selected the yellow bottle.",
            [bottleColourName.green]    = "You have selected the green bottle.",
            [bottleColourName.orange]   = "You have selected the orange bottle.",
            [bottleColourName.red]      = "You have selected the red bottle.",
            [bottleColourName.pink]     = "You have selected the pink bottle.",
            [bottleColourName.white]    = "You have selected the white bottle.",
            [bottleColourName.black]    = "You have selected the black bottle."
        };
        public ReadOnlyDictionary<bottleColourName, String> bottleColourText => new ReadOnlyDictionary<bottleColourName, String>(_bottleColourText);

        // Dictionary for the bottle colour images
        private Dictionary<bottleColourName, String> _bottleColourImage = new Dictionary<bottleColourName, String>() {
            [bottleColourName.blue]     = "ms-appx:///Assets/Images/BottleColours/BlueBottle.png",
            [bottleColourName.yellow]   = "ms-appx:///Assets/Images/BottleColours/YellowBottle.png",
            [bottleColourName.green]    = "ms-appx:///Assets/Images/BottleColours/GreenBottle.png",
            [bottleColourName.orange]   = "ms-appx:///Assets/Images/BottleColours/OrangeBottle.png",
            [bottleColourName.red]      = "ms-appx:///Assets/Images/BottleColours/RedBottle.png",
            [bottleColourName.pink]     = "ms-appx:///Assets/Images/BottleColours/PinkBottle.png",
            [bottleColourName.white]    = "ms-appx:///Assets/Images/BottleColours/WhiteBottle.png",
            [bottleColourName.black]    = "ms-appx:///Assets/Images/BottleColours/BlackBottle.png"
        };
        public ReadOnlyDictionary<bottleColourName, String> bottleColourImage => new ReadOnlyDictionary<bottleColourName, String>(_bottleColourImage);

        // Dictionary for the out of stock or not images
        private Dictionary<byte, String> outOfStockOrNotImage = new Dictionary<byte, string>() {
            [0b00000000] = "ms-appx:///Assets/Images/Misc/Nothing.png",
            [0b00000001] = "ms-appx:///Assets/Images/Misc/RedX.png"
        };



        // An event handler used to make the binding update
        public event PropertyChangedEventHandler PropertyChanged;

        // Promotion timer stuff for the progressbar
        private int _promotionTimerTickCounter = 0;
        public int promotionTimerTickCounter {
            get => _promotionTimerTickCounter;
            set {
                _promotionTimerTickCounter = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(progressBarValue)));
            }
        }

        // Temperature related stuff
        private String lowerBottleTemperature {
            get { return string.Format("{0:0.0}", lowerTemperature) + "°C"; }
        }
        private double _lowerTemperature = 30.0;
        public double lowerTemperature {
            get => _lowerTemperature;
            set {
                _lowerTemperature = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(bottleTemperature)));
            }
        }

        // Promotion media related stuff
        private promotionMediaName _promotionSource = 0;
        public promotionMediaName promotionSource {
            get => _promotionSource;
            set {
                _promotionSource = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImage)));
            }
        }
        
        // Promotion media preload related stuff
        private promotionMediaName _promotionSourcePreload = 0;
        public promotionMediaName promotionSourcePreload {
            get => _promotionSourcePreload;
            set {
                _promotionSourcePreload = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImagePreload)));
            }
        }

        // Selected bottle colour related stuff
        private bottleColourName _selectedBottleColour = 0;
        public bottleColourName selectedBottleColour {
            get => _selectedBottleColour;
            set {
                _selectedBottleColour = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(selectedColourImage)));
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(selectedColourText)));
            }
        }

        // Column empty status related sutff
        private List<String> bottleStringBindingNames = new List<String>() { nameof(bottleString0), nameof(bottleString1), nameof(bottleString2), nameof(bottleString3), nameof(bottleString4), nameof(bottleString5), nameof(bottleString6), nameof(bottleString7) };
        private byte _bottleOutOfStock = 0b00000000;
        public byte bottleOutOfStock {
            get => _bottleOutOfStock;
            set {
                if (value != _bottleOutOfStock) {

                    byte oldVal = _bottleOutOfStock;

                    // Update the variable with the new value
                    _bottleOutOfStock = value;

                    // Check which value has changed an update the binding of said value
                    for (int i = 0; i < 8; i++) {
                        if (((value >> i) & 1) != ((oldVal >> i) & 1)) PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(bottleStringBindingNames[i]));
                    }

                }
            }
        }

        // Method used to check if bottle n is out of stock
        public Boolean isBottleAvailable(int bottleNo) {

            if ((bottleOutOfStock & (1 << bottleNo)) == 0)
                return true;
            else
                return false;
        }



        // Bindings
        public int progressBarValue         => promotionTimerTickCounter;                                                           // Progressbar
        public String bottleTemperature     => lowerBottleTemperature;                                                              // Bottle Temperature
        public String promotionImage        => promotionMedia[promotionSource];                                                     // Promotion image
        public String promotionImagePreload => promotionMedia[promotionSourcePreload];                                              // Promotion image - preload
        public String selectedColourText    => bottleColourText[selectedBottleColour];                                              // Selected bottle colour text
        public String selectedColourImage   => bottleColourImage[selectedBottleColour];                                             // Selected bottle colour image
        
        public String bottleString0         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b00000001)) >> 0)];        // Bottle Out of stock overlay for bottle 0
        public String bottleString1         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b00000010)) >> 1)];        // Bottle Out of stock overlay for bottle 1
        public String bottleString2         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b00000100)) >> 2)];        // Bottle Out of stock overlay for bottle 2
        public String bottleString3         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b00001000)) >> 3)];        // Bottle Out of stock overlay for bottle 3
        public String bottleString4         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b00010000)) >> 4)];        // Bottle Out of stock overlay for bottle 4
        public String bottleString5         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b00100000)) >> 5)];        // Bottle Out of stock overlay for bottle 5
        public String bottleString6         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b01000000)) >> 6)];        // Bottle Out of stock overlay for bottle 6
        public String bottleString7         => outOfStockOrNotImage[(byte) (((byte) (bottleOutOfStock & 0b10000000)) >> 7)];        // Bottle Out of stock overlay for bottle 7

    }
}
