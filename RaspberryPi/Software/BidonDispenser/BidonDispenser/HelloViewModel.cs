using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace BidonDispenser {
    internal class HelloViewModel : INotifyPropertyChanged {

        public enum Language {
            English, Dutch, German
        }

        private Dictionary<Language, string> model = new Dictionary<Language, string>() {
            [Language.English] = "Hello World!",
            [Language.Dutch] = "Hallo Wereld!",
            [Language.German] = "Tbh, I can't be bothered."
        };

        private Language _DisplayLanguage = Language.English;

        public event PropertyChangedEventHandler PropertyChanged;

        public Language DisplayLanguage {
            get => _DisplayLanguage;
            set {
                _DisplayLanguage = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Message)));
            }
        }

        public string Message => model[DisplayLanguage];
    }
}
