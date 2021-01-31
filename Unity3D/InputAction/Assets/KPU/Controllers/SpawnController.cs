using System.Collections;
using KPU.Manager;
using UnityEngine;

namespace KPU.Controllers
{
    public class SpawnController : MonoBehaviour
    {
        public string spawnTargetName = "enemy";
        public float spawnRate = 1f;
        private Coroutine _routine;

        private void Start()
        {
            EventManager.On("game_started", StartSpawn);
            EventManager.On("game_ended", StopSpawn);
        }

        private void StartSpawn(object obj)
        {
            _routine = StartCoroutine(SpawnRoutine());
        }

        private void StopSpawn(object obj)
        {
            StopCoroutine(_routine);
        }

        private IEnumerator SpawnRoutine()
        {
            while (true)
            {
                while (GameManager.Instance.State == State.Playing)
                {
                    ObjectPoolManager.Instance.Spawn(spawnTargetName, transform.position, transform.rotation);
                    yield return new WaitForSeconds(spawnRate);
                }
                yield return null;
            }
        }
    }
}