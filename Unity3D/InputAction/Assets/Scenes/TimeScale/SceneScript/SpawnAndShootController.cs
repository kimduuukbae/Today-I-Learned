using System.Collections;
using KPU.Manager;
using UnityEngine;
using Random = UnityEngine.Random;

namespace TimeScale
{
    public class SpawnAndShootController : MonoBehaviour
    {
        public float spawnRate = 0.5f;
        public string spawnTarget = "block";
        public float power = 5;

        private void Start()
        {
            StartCoroutine(Spawn());
        }

        private IEnumerator Spawn()
        {
            while (enabled)
            {
                var spawnedGameObject = ObjectPoolManager.Instance.Spawn(spawnTarget);
                spawnedGameObject.transform.position = transform.position;
                spawnedGameObject.transform.localScale = Vector3.one * Random.Range(0.1f, 1f);

                var block = spawnedGameObject.GetComponent<Block>();
                block.lifeSpan = Random.Range(1f, 2f);

                var rigidBody = spawnedGameObject.GetComponent<Rigidbody>();
                rigidBody.velocity = Vector3.zero;

                rigidBody.AddForce(new Vector3(Random.Range(-1, 1), Random.Range(-1, 1), Random.Range(-1, 1)) * power);
                rigidBody.AddTorque(new Vector3(Random.Range(-1, 1), Random.Range(-1, 1), Random.Range(-1, 1)) * power);

                yield return new WaitForSeconds(spawnRate);
            }
        }
    }
}