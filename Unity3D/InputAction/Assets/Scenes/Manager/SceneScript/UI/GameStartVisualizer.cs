using KPU.Manager;
using UnityEngine;

namespace Manager.UI
{
    public class GameStartVisualizer : MonoBehaviour
    {
        private void Start()
        {
            EventManager.On("game_started", Hide);
            EventManager.On("game_ended", Show);
        }

        private void Hide(object obj) => gameObject.SetActive(false);
        private void Show(object obj) => gameObject.SetActive(true);
    }
}
