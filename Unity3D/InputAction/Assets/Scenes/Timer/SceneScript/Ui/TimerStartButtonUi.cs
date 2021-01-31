using KPU.Manager;
using KPU.Time;
using TMPro;
using UnityEngine;

namespace Timer.Ui    
{
    public class TimerStartButtonUi : MonoBehaviour
    {
        private TextMeshProUGUI _text;

        private void Awake()
        {
            _text = GetComponentInChildren<TextMeshProUGUI>();
        }

        private void Start()
        {
            EventManager.On("timer_start", OnTimerStart);
            EventManager.On("timer_stop", OnTimerStop);
            EventManager.On("timer_reset", OnTimerReset);
            
            _text.text = "Start";
        }

        private void OnTimerReset(object obj)
        {
            _text.text = "Start";
        }

        private void OnTimerStop(object obj)
        {
            _text.text = "Resume";
            TimeManager.Instance.DeActive();
        }

        private void OnTimerStart(object obj)
        {
            _text.text = "Stop";
            TimeManager.Instance.Active();
        }

        public void Click()
        {
            EventManager.Emit(TimeManager.Instance.TimerActive ? "timer_stop" : "timer_start", null);
        }
    }
}