using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Assets.Scripts
{
    public class EventManager : SingletonBehaviour<EventManager>
    {
        private IDictionary<string, List<System.Action<object>>> _eventDatabase;

        private void Awake()
        {
            _eventDatabase = new Dictionary<string, List<System.Action<object>>>();
        }

        public static void On(string eventName, System.Action<object> subscriberAction)
        {
            if (Instance._eventDatabase.ContainsKey(eventName) == false)
                Instance._eventDatabase.Add(eventName, new List<System.Action<object>>());
                
            Instance._eventDatabase[eventName].Add(subscriberAction);
        }

        public static void Emit(string eventName, object parameter)
        {
            if(Instance._eventDatabase.ContainsKey(eventName) == false)
            {
                Debug.LogWarning($"{eventName}가 존재하지 않습니다.");
                return;
            }

            foreach (var action in Instance._eventDatabase[eventName])
                action.Invoke(parameter);
        }
    }
}