using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BidonDispenser {
    internal class MainModel : INotifyPropertyChanged {

        public enum promotionMediaName {
            /*gif, */whyJoinThePipe, buyOneGiveOne, cityCleanUp, waterTaps, waterKiosk
        }

        private Dictionary<promotionMediaName, String> _promotionMedia = new Dictionary<promotionMediaName, string>() {
            //[promotionMediaName.gif]            = "ms-appx:///Assets/Images/Promotions/BottleColours.gif",
            [promotionMediaName.whyJoinThePipe] = "ms-appx:///Assets/Images/Promotions/HappyWaterThingy.png",
            [promotionMediaName.buyOneGiveOne]  = "ms-appx:///Assets/Images/Promotions/BuyOneGiveOne.png",
            [promotionMediaName.cityCleanUp]    = "ms-appx:///Assets/Images/Promotions/CleanUp.png",
            [promotionMediaName.waterTaps]      = "ms-appx:///Assets/Images/Promotions/WaterTaps.png",
            [promotionMediaName.waterKiosk]     = "ms-appx:///Assets/Images/Promotions/WaterKiosk.png"
        };
        public ReadOnlyDictionary<promotionMediaName, String> promotionMedia => new ReadOnlyDictionary<promotionMediaName, String> (_promotionMedia);

        public enum bottleColourName {
            blue, yellow, green, orange, red, pink, white, black
        }

        private Dictionary<bottleColourName, String> _bottleColours = new Dictionary<bottleColourName, string>() {
            [bottleColourName.blue]     = "ms-appx:///Assets/Images/BottleColours/BlueBottle.png",
            [bottleColourName.yellow]   = "ms-appx:///Assets/Images/BottleColours/YellowBottle.png",
            [bottleColourName.green]    = "ms-appx:///Assets/Images/BottleColours/GreenBottle.png",
            [bottleColourName.orange]   = "ms-appx:///Assets/Images/BottleColours/OrangeBottle.png",
            [bottleColourName.red]      = "ms-appx:///Assets/Images/BottleColours/RedBottle.png",
            [bottleColourName.pink]     = "ms-appx:///Assets/Images/BottleColours/PinkBottle.png",
            [bottleColourName.white]    = "ms-appx:///Assets/Images/BottleColours/WhiteBottle.png",
            [bottleColourName.black]    = "ms-appx:///Assets/Images/BottleColours/BlackBottle.png"
        };
        public ReadOnlyDictionary<bottleColourName, String> bottleColours => new ReadOnlyDictionary<bottleColourName, String>(_bottleColours);


        public event PropertyChangedEventHandler PropertyChanged;

        private int _promotionTimerTickCounter = 0;
        public int promotionTimerTickCounter {
            get => _promotionTimerTickCounter;
            set {
                _promotionTimerTickCounter = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(progressBarValue)));
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

        public int progressBarValue => promotionTimerTickCounter;
        public String promotionImage => promotionMedia[promotionSource];
        public String promotionImagePreload => promotionMedia[promotionSourcePreload];

    }
}
