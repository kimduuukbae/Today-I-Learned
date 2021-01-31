using System.Linq;
using KPU;
using KPU.Manager;
using UnityEngine;

namespace Manager.Tower
{
    public class Missile : MonoBehaviour
    {
        public float initialPower = 10f;
        public float initialSpeed = 10f;
        private float _power;
        private float _speed;
        private Rigidbody2D _rigidBody;
        private Vector3 _lastRigidBodyVelocity;

        public void SetLevel(int towerLevel)
        {
            _power = initialPower * towerLevel / 10 * 2;
            _speed = initialSpeed + towerLevel * 0.1f;
        }

        private void Awake()
        {
            _rigidBody = GetComponent<Rigidbody2D>();
        }

        private void OnEnable()
        {
            _power = initialPower;
            _speed = initialSpeed;
        }

        private void Start()
        {
            EventManager.On("game_ended", Hide);
        }

        private void Hide(object obj) => gameObject.SetActive(false);

        private void Update()
        {
            if (GameManager.Instance.State == State.Playing)
            {
                if (_lastRigidBodyVelocity != default)
                {
                    _rigidBody.velocity = _lastRigidBodyVelocity;
                    _lastRigidBodyVelocity = default;
                }

                Collider2D[] targets = new Collider2D[5];
                Physics2D.OverlapCircleNonAlloc(new Vector2(transform.position.x, transform.position.y), 20,
                    targets);

                var candidateTarget = targets.Where(_ => _ != null)
                    .Aggregate((item, nextItem) =>
                    {
                        var position = transform.position;
                        var dist1 = (position - item.transform.position).magnitude;
                        var dist2 = (position - nextItem.transform.position).magnitude;
                        return dist1 > dist2 ? item : nextItem;
                    });
                var vec = (candidateTarget.transform.position - transform.position);
                _rigidBody.AddForce(vec.normalized * _speed);
            }
            else
            {
                if (_lastRigidBodyVelocity == default) _lastRigidBodyVelocity = _rigidBody.velocity;
            }
        }

        private void OnTriggerEnter2D(Collider2D other)
        {
            if (other.CompareTag("Enemy"))
            {
                other.GetComponent<Enemy.Enemy>()?.Damage(_power);
                gameObject.SetActive(false);
            }
        }
    }
}