using System.Collections;
using KPU;
using KPU.Manager;
using UnityEngine;

namespace Manager.Tower
{
    public class TowerController : MonoBehaviour
    {
        public float maxHealth = 10f;
        public float shootSpeed = 1f;
        public int level = 1;

        private float _health;

        public float Health => _health;

        // Start is called before the first frame update
        void Start()
        {
            EventManager.On("game_started", OnStart);
            EventManager.On("game_ended", OnEnded);
            gameObject.SetActive(false);
        }


        private void OnStart(object obj)
        {
            gameObject.SetActive(true);
            _health = maxHealth;
            level = 1;
            StartCoroutine(ShootRoutine());
        }

        private void OnEnded(object obj)
        {
            gameObject.SetActive(false);
        }

        private IEnumerator ShootRoutine()
        {
            while (true)
            {
                while (GameManager.Instance.State == State.Playing)
                {
                    var missileGameObject = ObjectPoolManager.Instance.Spawn("missile", transform.position,
                        Quaternion.Euler(Vector3.right));
                    var component = missileGameObject.GetComponent<Missile>();
                    component.SetLevel(level);

                    yield return new WaitForSeconds(shootSpeed);
                }

                yield return null;
            }
        }

        public void Damage(float damage)
        {
            _health -= damage;
            if(_health <= 0) EventManager.Emit("game_ended", null);
        }
    }
}