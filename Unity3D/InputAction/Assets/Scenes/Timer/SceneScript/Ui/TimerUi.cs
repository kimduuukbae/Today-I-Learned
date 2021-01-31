using System;
using KPU.Time;
using TMPro;
using UnityEngine;

namespace Timer.Ui
{
    public class TimerUi : MonoBehaviour
    {
        private TextMeshProUGUI _timerText;

        private void Awake()
        {
            _timerText = GetComponent<TextMeshProUGUI>();
        }

        private void Update()
        {
            var time = new TimeSpan(0, 0, 0, 0, (int) (TimeManager.Instance.Time * 1000));
            
            _timerText.text = $"{time.Hours:00}:{time.Minutes:00}:{time.Seconds:00}.{time.Milliseconds:000}";
        }
    }
}