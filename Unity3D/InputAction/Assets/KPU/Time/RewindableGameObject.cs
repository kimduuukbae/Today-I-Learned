using System.Collections;
using System.Collections.Generic;
using System.Linq;
using KPU.Manager;
using UnityEngine;

namespace KPU.Time
{
    [RequireComponent(typeof(Rigidbody))]
    public class RewindableGameObject : MonoBehaviour, IRewindable
    {
        public float recordTime = 5f;
        private List<RewindRecord> _rewindRecords;
        private Rigidbody _rigidbody;
        private Coroutine _recordRoutine;
        private Coroutine _rewindRoutine;
        private RewindRecord _last;

        private void Awake()
        {
            _rigidbody = GetComponent<Rigidbody>();
            _rewindRecords = new List<RewindRecord>();
        }

        public void Start()
        {
            Record();
            EventManager.On("rewind_start", OnRewind);
            EventManager.On("rewind_stop", OnRewindStop);
            
        }

        private void OnRewindStop(object obj)
        {
           StopCoroutine(_rewindRoutine);
           Record();
           _rigidbody.isKinematic = false;
           _rigidbody.velocity = _last.Velocity;
           _rigidbody.angularVelocity = _last.AngularVelocity;
           transform.position = _last.Position;
           transform.rotation = _last.Rotation;
        }

        private void OnRewind(object obj)
        {
            Rewind();
        }

        public void Record()
        {
            _recordRoutine = StartCoroutine(RecordRoutine());
        }

        private IEnumerator RecordRoutine()
        {
            while (enabled)
            {
                if (_rewindRecords.Count * UnityEngine.Time.fixedDeltaTime > recordTime)
                    _rewindRecords.RemoveAt(0);
                
                _rewindRecords.Add(new RewindRecord
                {
                    AngularVelocity = _rigidbody.angularVelocity,
                    Velocity = _rigidbody.velocity,
                    Position = transform.position,
                    Rotation = transform.rotation
                });

                yield return new WaitForFixedUpdate();
            }
        }

        public void Rewind()
        {
            if (_recordRoutine != null)
            {
                StopCoroutine(_recordRoutine);
                _recordRoutine = null;
            }

            _rewindRoutine = StartCoroutine(RewindRoutine());
        }

        private IEnumerator RewindRoutine()
        {
            while (_rewindRecords.Count > 0)
            {
                _last = _rewindRecords.Last();

                transform.position = _last.Position;
                transform.rotation = _last.Rotation;

                _rigidbody.isKinematic = true;
                _rigidbody.velocity = _last.Velocity;
                _rigidbody.angularVelocity = _last.AngularVelocity;

                yield return new WaitForFixedUpdate();
                _rewindRecords.Remove(_last);
            }
        }
    }
}