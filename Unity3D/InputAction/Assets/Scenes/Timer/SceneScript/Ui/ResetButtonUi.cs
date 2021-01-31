using KPU.Manager;
using KPU.Time;
using UnityEngine;

namespace Timer.Ui
{
    public class ResetButtonUi : MonoBehaviour
    {
        public void Click()
        {
            EventManager.Emit("timer_reset", null);
            TimeManager.Instance.ResetTime();
        }
    }
}
