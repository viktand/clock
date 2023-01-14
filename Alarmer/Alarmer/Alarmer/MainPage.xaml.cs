using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace Alarmer
{
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();
        }

        private void Button_AlarmOn(object sender, EventArgs e)
        {
            AlarmOnOff(true);
        }

        private void Button_AlarmOff(object sender, EventArgs e)
        {
            AlarmOnOff(false);
        }

        private async void AlarmOnOff(bool sw)
        {
            Uri uri = new Uri($"http://192.168.0.196:80?alarm={(sw ? "ON" : "OFF")}");
            HttpClient client = new HttpClient();
            try
            {
                HttpResponseMessage response = await client.GetAsync(uri);
                if (response.IsSuccessStatusCode)
                {
                    string content = await response.Content.ReadAsStringAsync();
                }
            }
            catch { }
        }

        private void Button_SetAlarm(object sender, EventArgs e)
        {
            var timespan = piker.Time;
            var time = timespan.Minutes * 60 + timespan.Hours * 3600;
            SendTimeAlarm(time);
        }

        private async void SendTimeAlarm(int time)
        {
            Uri uri = new Uri($"http://192.168.0.196:80/alarm=SET:{time}");
            HttpClient client = new HttpClient();
            try
            {
                HttpResponseMessage response = await client.GetAsync(uri);
                if (response.IsSuccessStatusCode)
                {
                    string content = await response.Content.ReadAsStringAsync();
                }
            }
            catch { }
        }
    }
}
