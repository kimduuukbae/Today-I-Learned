using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace KPU.Manager
{
    public class ObjectPoolManager : SingletonBehaviour<ObjectPoolManager>
    {
        [Serializable]
        public class PrefabObjectKeyValuePair
        {
            public string name;
            public GameObject prefab;
        }

        public List<PrefabObjectKeyValuePair> prefabs;

        private IDictionary<string, IList<GameObject>> _objectPool;

        private IDictionary<string, IList<GameObject>> ObjectPool =>
            _objectPool ?? (_objectPool = new Dictionary<string, IList<GameObject>>());

        public GameObject Spawn(string spawnTargetName, Vector3 position = default, Quaternion rot = default)
        {
            var foundedPrefabData = prefabs.FirstOrDefault(_ => _.name == spawnTargetName);

            if (foundedPrefabData == null) throw new Exception($"{spawnTargetName} 이라는 오브젝트는 없습니다.");

            if (!ObjectPool.ContainsKey(spawnTargetName))
                ObjectPool.Add(spawnTargetName, new List<GameObject>());

            var founded = ObjectPool[spawnTargetName].FirstOrDefault(_ => !_.activeInHierarchy);

            if (founded != null)
                founded.SetActive(true);
            else
            {
                founded = Instantiate(foundedPrefabData.prefab);
                ObjectPool[spawnTargetName].Add(founded);
            }

            if (position != default) founded.transform.position = position;
            if (rot != default) founded.transform.rotation = rot;

            return founded;
        }
    }
}