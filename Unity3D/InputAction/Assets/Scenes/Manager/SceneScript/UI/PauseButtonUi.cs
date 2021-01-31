using KPU.Manager;
using UnityEngine;

namespace Manager.UI
{
    public class PauseButtonUi : MonoBehaviour
    {
        private void Start()
        {
            EventManager.On("game_started", Show);
            EventManager.On("game_resumed", Show);
            EventManager.On("game_paused", Hide);
            EventManager.On("game_ended", Hide);
            gameObject.SetActive(false);
        }

        private void Show(object obj) => gameObject.SetActive(true);
        private void Hide(object obj) => gameObject.SetActive(false);

        public void Pause() => EventManager.Emit("game_paused", null);
    }
}
