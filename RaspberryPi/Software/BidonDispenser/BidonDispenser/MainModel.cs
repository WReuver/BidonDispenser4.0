using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BidonDispenser {
    internal class MainModel : INotifyPropertyChanged {

        public enum mediaNames {
            gif, buyOneGiveOne, cityCleanUp, whyJoinThePipe, waterKiosk, waterTaps
        }

        private Dictionary<mediaNames, String> promotionMedias = new Dictionary<mediaNames, string>() {
            [mediaNames.gif]            = "ms-appx:///Assets/Images/BottleColours.gif",
            [mediaNames.buyOneGiveOne]  = "ms-appx:///Assets/Images/BuyOneGiveOne.png",
            [mediaNames.cityCleanUp]    = "ms-appx:///Assets/Images/CleanUp.png",
            [mediaNames.whyJoinThePipe] = "ms-appx:///Assets/Images/HappyWaterThingy.png",
            [mediaNames.waterKiosk]     = "ms-appx:///Assets/Images/WaterKiosk.png",
            [mediaNames.waterTaps]      = "ms-appx:///Assets/Images/WaterTaps.png"
        };

        public event PropertyChangedEventHandler PropertyChanged;

        private mediaNames _mediaSource = mediaNames.gif;
        public mediaNames mediaSource {
            get => _mediaSource;
            set {
                _mediaSource = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImage)));
            }
        }

        private mediaNames _mediaSource2 = mediaNames.gif;
            public mediaNames mediaSource2 {
            get => _mediaSource2;
            set {
                _mediaSource2 = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(promotionImagePreload)));
            }
        }

        public String promotionImage => promotionMedias[mediaSource];
        public String promotionImagePreload => promotionMedias[mediaSource2];

    }
}
