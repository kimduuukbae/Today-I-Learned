using KPU;
using KPU.Manager;
using Manager.Tower;
using UnityEngine;

namespace Manager.Enemy
{
    public class Enemy : MonoBehaviour
    {
        public float damage = 1f;
        public float maxHealthPoint = 10f;
        public float speed = 1f;
        private Rigidbody2D _rigidBody;
        private float _healthPoint;

        private void Awake()
        {
            _rigidBody = GetComponent<Rigidbody2D>();
        }

        private void OnEnable()
        {
            _healthPoint = maxHealthPoint;
        }

        private void Start()
        {
            EventManager.On("game_ended", Hide);
            EventManager.On("game_started", Hide);
        }

        private void Hide(object obj) => gameObject.SetActive(false);

        private void Update()
        {
            if (_healthPoint <= 0)
            {
                Die();
                gameObject.SetActive(false);
                return;
            }
            
            if (GameManager.Instance.State == State.Playing)
            {
                _rigidBody.velocity = Vector2.left * speed; 
            }
            else
            {
                _rigidBody.velocity = Vector2.zero;
            }
        }

        private void Die()
        {
            EventManager.Emit("money_added", (int)maxHealthPoint);
            EventManager.Emit("score_added", 1);
        }

        public void Damage(float damage)
        {
            _healthPoint -= damage;
        }

        private void OnTriggerEnter2D(Collider2D other)
        {
            if (other.CompareTag("Player"))
            {
                other.GetComponent<TowerController>().Damage(damage);
                gameObject.SetActive(false);
            }
        }
    }
}
