using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace BidonDispenser
{
    public sealed partial class MainPage : Page
    {
        private HelloViewModel viewModel = new HelloViewModel();

        public MainPage() {
            this.InitializeComponent();
            DispatcherTimer t = new DispatcherTimer();
            t.Interval = TimeSpan.FromSeconds(0.5);
            t.Tick += T_Tick;
            t.Start();
        }

        static int index = 0;
        private void T_Tick(object sender, object e) {
            index++;
            viewModel.DisplayLanguage = (HelloViewModel.Language)(index % 3);
        }
    }
}
