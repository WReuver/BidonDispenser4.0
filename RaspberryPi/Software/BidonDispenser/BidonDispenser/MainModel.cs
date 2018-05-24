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
            gif, whyJoinThePipe, buyOneGiveOne, cityCleanUp, waterTaps, waterKiosk
        }

        private Dictionary<promotionMediaName, String> _promotionMedia = new Dictionary<promotionMediaName, string>() {
            [promotionMediaName.gif]            = "ms-appx:///Assets/Images/BottleColours.gif",
            [promotionMediaName.whyJoinThePipe] = "ms-appx:///Assets/Images/HappyWaterThingy.png",
            [promotionMediaName.buyOneGiveOne]  = "ms-appx:///Assets/Images/BuyOneGiveOne.png",
            [promotionMediaName.cityCleanUp]    = "ms-appx:///Assets/Images/CleanUp.png",
            [promotionMediaName.waterTaps]      = "ms-appx:///Assets/Images/WaterTaps.png",
            [promotionMediaName.waterKiosk]     = "ms-appx:///Assets/Images/WaterKiosk.png"
        };
        public ReadOnlyDictionary<promotionMediaName, String> promotionMedia => new ReadOnlyDictionary<promotionMediaName, String> (_promotionMedia);
        

        public event PropertyChangedEventHandler PropertyChanged;


        private int _promotionTimerTickCounter = 0;
        public int promotionTimerTickCounter {
            get => _promotionTimerTickCounter;
            set {
                _promotionTimerTickCounter = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(progressBarValue)));
            }
        }
        
        private promotionMediaName _promotionSource = promotionMediaName.gif;
        public promotionMediaName promotionSource {
            get => _promotionSource;
            set {
                _promotionSource = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImage)));
            }
        }
        
        private promotionMediaName _promotionSourcePreload = promotionMediaName.gif;
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
