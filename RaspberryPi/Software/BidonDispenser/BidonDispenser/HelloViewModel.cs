using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace BidonDispenser {
    internal class HelloViewModel : INotifyPropertyChanged {

        public enum Language {
            English, Dutch, German
        }

        private Dictionary<Language, string> languages = new Dictionary<Language, string>() {
            [Language.English]  = "Hello World!",
            [Language.Dutch]    = "Hallo Wereld!",
            [Language.German]   = "Tbh, I cannot be bothered."
        };

        private Language _DisplayLanguage = Language.English;

        public event PropertyChangedEventHandler PropertyChanged;

        public Language DisplayLanguage {
            get => _DisplayLanguage;
            set {
                _DisplayLanguage = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(message)));
            }
        }

        public string message => languages[DisplayLanguage];
    }
}
