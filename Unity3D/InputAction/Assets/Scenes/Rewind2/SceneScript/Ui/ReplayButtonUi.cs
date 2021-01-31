using KPU.Manager;
using UnityEngine;

namespace Rewind2.SceneScript.Ui
{
    public class ReplayButtonUi : MonoBehaviour
    {
        public void Click()
        {
            EventManager.Emit("replay_start", null);
        }
    }
}
