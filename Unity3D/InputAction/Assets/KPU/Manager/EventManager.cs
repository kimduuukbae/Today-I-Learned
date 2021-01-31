using System;
using System.Collections.Generic;
using UnityEngine;

namespace KPU.Manager
{
    public class EventManager : SingletonBehaviour<EventManager>
    {
        private IDictionary<string, List<Action<object>>> EventDatabase =>
            _eventDatabase ?? (_eventDatabase = new Dictionary<string, List<Action<object>>>());

        private IDictionary<string, List<Action<object>>> _eventDatabase;

        public static void On(string eventName, Action<object> subscriber)
        {
            // 만약 없을 경우.
            if (!Instance.EventDatabase.ContainsKey(eventName))
                Instance.EventDatabase.Add(eventName, new List<Action<object>>());

            Instance.EventDatabase[eventName].Add(subscriber);
        }

        public static void Emit(string eventName, object parameter)
        {
            if (!Instance.EventDatabase.ContainsKey(eventName))
                Debug.LogWarning($"{eventName}에 해당하는 이벤트는 존재하지 않습니다.");
            else
                foreach (var action in Instance.EventDatabase[eventName])
                    action?.Invoke(parameter);
        }
    }
}