using KPU.Manager;
using KPU.Time;
using UnityEngine;

namespace Timer.Ui
{
    public class RecordButtonUi : MonoBehaviour
    {
        public void Click()
        {
            EventManager.Emit("timer_record", TimeManager.Instance.Time);
        }
    }
}