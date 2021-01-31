using UnityEngine;

namespace KPU
{
    public class SingletonBehaviour<T> : MonoBehaviour where T : MonoBehaviour
    {
        public static T Instance
        {
            get
            {
                if (_instance == null) _instance = FindObjectOfType<T>();
                if (_instance == null)
                {
                    var go = GameObject.Find(typeof(T).Name) ?? new GameObject(typeof(T).Name);
                    _instance = go.GetComponent<T>() ?? go.AddComponent<T>();
                }

                return _instance;
            }
        }

        private static T _instance;
    }
}