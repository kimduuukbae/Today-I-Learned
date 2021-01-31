using System.Collections;
using UnityEngine;

namespace TimeScale
{
    public class Block : MonoBehaviour
    {
        public float lifeSpan = 1;
        private void OnEnable()
        {
            StartCoroutine(Live());
        }

        private IEnumerator Live()
        {
            yield return new WaitForSeconds(lifeSpan);
            gameObject.SetActive(false);
        }
    }
}