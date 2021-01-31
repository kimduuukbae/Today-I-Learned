using KPU.Manager;
using TMPro;
using UnityEngine;

namespace Manager.UI
{
    public class GameStatusTextUi : MonoBehaviour
    {
        public TextMeshProUGUI text;

        void Update()
        {
            text.text = GameManager.Instance.State.ToString();
        }
    }
}
