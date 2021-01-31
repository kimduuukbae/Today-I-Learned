using KPU.Manager;
using TMPro;
using UnityEngine;

namespace Rewind.SceneScript.Ui
{
    public class RewindButtonUi : MonoBehaviour
    {
        private TextMeshProUGUI _buttonText;
        private bool _isRewind;

        private void Awake()
        {
            _buttonText = GetComponentInChildren<TextMeshProUGUI>();
            _isRewind = false;
        }

        private void Start()
        {
            EventManager.On("rewind_start", OnRewindStart);
            EventManager.On("rewind_stop", OnRewindStop);
        }

        private void OnRewindStop(object obj)
        {
            _buttonText.text = "Rewind";
            _isRewind = false;
        }

        private void OnRewindStart(object obj)
        {
            _buttonText.text = "Play";
            _isRewind = true;
        }

        public void Click()
        {
            EventManager.Emit(_isRewind ? "rewind_stop" : "rewind_start", null);
        }
    }
}
