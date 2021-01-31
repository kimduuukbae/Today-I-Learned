using UnityEngine;

namespace TimeScale
{
    public class TimeScaleSliderUi : MonoBehaviour
    {
        private void Start()
        {
            Time.timeScale = 0f;
        }

        public void OnValueChanced(float val)
        {
            Time.timeScale = val;
            print(val);
        }
    }
}