using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
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
        
        private mediaNames _mediaSource = mediaNames.gif;

        public event PropertyChangedEventHandler PropertyChanged;

        public mediaNames mediaSource {
            get => _mediaSource;
            set {
                _mediaSource = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(source)));
            }
        }


        public string source => promotionMedias[mediaSource];

    }
}
