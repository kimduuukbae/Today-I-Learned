using System;
using System.Collections.Generic;
using KPU.Manager;
using TMPro;
using UnityEngine;

namespace Timer.Ui
{
    public class TimeRecordUi : MonoBehaviour
    {
        private List<float> _times;
        private TextMeshProUGUI _recordTimeText;

        private void Awake()
        {
            _times = new List<float>();
            _recordTimeText = GetComponent<TextMeshProUGUI>();
        }

        private void Start()
        {
            EventManager.On("timer_record", OnTimeRecord);
        }

        private void OnTimeRecord(object obj)
        {
            var time = (float) obj;
            
            if (_times.Count >= 5) _times.RemoveAt(0);

            _times.Add(time);

            _recordTimeText.text = string.Empty;
            foreach (var f in _times)
            {
                var timeSpan = new TimeSpan(0, 0, 0, 0, (int) (f * 1000));
                _recordTimeText.text +=
                    $"{timeSpan.Hours:00}:{timeSpan.Minutes:00}:{timeSpan.Seconds:00}:{timeSpan.Milliseconds:000}" +
                    Environment.NewLine;
            }
        }
    }
}