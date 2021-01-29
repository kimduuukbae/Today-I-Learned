using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Assets.Scripts.UI
{
    public class StatePrinter : MonoBehaviour
    {
        private UnityEngine.UI.Text _text;
         
        void Start()
        {
            _text = GetComponent<UnityEngine.UI.Text>();
        }

        void Update()
        {
            _text.text = Assets.Scripts.GameManager.Instance.state.ToString();
        }
    }
}