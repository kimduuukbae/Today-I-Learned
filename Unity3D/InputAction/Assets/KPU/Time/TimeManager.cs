namespace KPU.Time
{
    public class TimeManager : SingletonBehaviour<TimeManager>
    {
        private bool _timerActive;
        private float _time;
        public float Time => _time;
        public bool TimerActive => _timerActive;

        private void Update()
        {
            if (_timerActive)
                _time += UnityEngine.Time.deltaTime;
        }

        public void Active()
        {
            _timerActive = true;
        }

        public void DeActive()
        {
            _timerActive = false;
        }

        public void ResetTime()
        {
            _timerActive = false;
            _time = 0f;
        }
    }
}