using KPU;
using KPU.Manager;
using UnityEngine;

namespace Manager.UI
{
    public class GameMainMenu : MonoBehaviour
    {
        private void Start()
        {
            GameManager.Instance.SetState(State.SceneLoaded);

            EventManager.On("game_started", HidePanel);
            EventManager.On("game_ended", ShowPanel);
            EventManager.On("game_resumed", HidePanel);
            EventManager.On("game_paused", ShowPanel);
        }

        private void ShowPanel(object param) => gameObject.SetActive(true);
        private void HidePanel(object param) => gameObject.SetActive(false);

        #region Events

        public void GameStartButtonClicked() => EventManager.Emit("game_started", null);
        public void GamePausedButtonClicked() => EventManager.Emit("game_paused", null);
        public void GameResumeButtonClicked() => EventManager.Emit("game_resumed", null);
        public void GameEndButtonClicked() => EventManager.Emit("game_ended", null);

        #endregion
    }
}