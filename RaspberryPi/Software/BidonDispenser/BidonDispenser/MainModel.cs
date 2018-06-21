using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace BidonDispenser {
    internal class MainModel : INotifyPropertyChanged {

        public enum promotionMediaName {
            whyJoinThePipe, buyOneGiveOne, cityCleanUp, waterTaps, waterKiosk
        }

        public enum bottleColourName {
            blue, yellow, green, orange, red, pink, white, black
        }

        private Dictionary<promotionMediaName, String> _promotionMedia = new Dictionary<promotionMediaName, String>() {
            [promotionMediaName.whyJoinThePipe] = "ms-appx:///Assets/Images/Promotions/HappyWaterThingy.png",
            [promotionMediaName.buyOneGiveOne]  = "ms-appx:///Assets/Images/Promotions/BuyOneGiveOne.png",
            [promotionMediaName.cityCleanUp]    = "ms-appx:///Assets/Images/Promotions/CleanUp.png",
            [promotionMediaName.waterTaps]      = "ms-appx:///Assets/Images/Promotions/WaterTaps.png",
            [promotionMediaName.waterKiosk]     = "ms-appx:///Assets/Images/Promotions/WaterKiosk.png"
        };
        public ReadOnlyDictionary<promotionMediaName, String> promotionMedia => new ReadOnlyDictionary<promotionMediaName, String> (_promotionMedia);
        
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

        private Dictionary<byte, String> outOfStockOrNotImage = new Dictionary<byte, string>() {
            [0b00000000] = "ms-appx:///Assets/Images/Misc/Nothing.png",
            [0b00000001] = "ms-appx:///Assets/Images/Misc/RedX.png"
        };


        public event PropertyChangedEventHandler PropertyChanged;

        private int _promotionTimerTickCounter = 0;
        public int promotionTimerTickCounter {
            get => _promotionTimerTickCounter;
            set {
                _promotionTimerTickCounter = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(progressBarValue)));
            }
        }

        private double _lowerTemperature = 30.0;
        public double lowerTemperature {
            get => _lowerTemperature;
            set {
                _lowerTemperature = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(bottleTemperature)));
            }
        }

        private promotionMediaName _promotionSource = 0;
        public promotionMediaName promotionSource {
            get => _promotionSource;
            set {
                _promotionSource = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImage)));
            }
        }
        
        private promotionMediaName _promotionSourcePreload = 0;
        public promotionMediaName promotionSourcePreload {
            get => _promotionSourcePreload;
            set {
                _promotionSourcePreload = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImagePreload)));
            }
        }

        private bottleColourName _selectedBottleColour = 0;
        public bottleColourName selectedBottleColour {
            get => _selectedBottleColour;
            set {
                _selectedBottleColour = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(selectedColourImage)));
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(selectedColourText)));
            }
        }

        private List<String> bottleStringBindingNames = new List<String>() { nameof(bottleString0), nameof(bottleString1), nameof(bottleString2), nameof(bottleString3), nameof(bottleString4), nameof(bottleString5), nameof(bottleString6), nameof(bottleString7) };
        private byte _bottleOutOfStock = 0x00;
        public byte bottleOutOfStock {
            get => _bottleOutOfStock;
            set {
                if (value != _bottleOutOfStock) {

                    // Check which value has changed an update the binding of said value
                    for (int i = 0; i < 8; i++) {
                        if ((value >> i) != (_bottleOutOfStock >> i)) PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(bottleStringBindingNames[i]));
                    }

                    // Update the variable with the new value
                    _bottleOutOfStock = value;
                }
            }
        }

        // Bindings
        public int progressBarValue         => promotionTimerTickCounter;                   // Progressbar
        public double bottleTemperature     => lowerTemperature;                            // Bottle Temperature
        public String promotionImage        => promotionMedia[promotionSource];             // Promotion image
        public String promotionImagePreload => promotionMedia[promotionSourcePreload];      // Promotion image - preload
        public String selectedColourText    => bottleColourText[selectedBottleColour];      // Selected bottle colour text
        public String selectedColourImage   => bottleColourImage[selectedBottleColour];     // Selected bottle colour image
        
        public String bottleString0         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b00000001)];
        public String bottleString1         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b00000010)];
        public String bottleString2         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b00000100)];
        public String bottleString3         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b00001000)];
        public String bottleString4         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b00010000)];
        public String bottleString5         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b00100000)];
        public String bottleString6         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b01000000)];
        public String bottleString7         => outOfStockOrNotImage[(byte) (bottleOutOfStock & 0b10000000)];

    }
}
