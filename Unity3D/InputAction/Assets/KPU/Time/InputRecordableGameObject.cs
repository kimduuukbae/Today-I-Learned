using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using KPU.Manager;
using UnityEngine;

namespace KPU.Time
{
    public class InputRecordableGameObject : MonoBehaviour, IInputRecordable
    {
        public IList<InputRecord> InputRecords { get; private set; }

        private Rigidbody _rigidbody;

        private float _playtime;
        private Vector2 _inputAxis;

        private bool _replayMode;

        private Vector3 _initialPosition;
        private Quaternion _initialRotation;
        private Coroutine _replayRoutine;

        private void Awake()
        {
            _replayMode = false;
            _rigidbody = GetComponent<Rigidbody>();
            InputRecords = new Collection<InputRecord>();
            _initialPosition = transform.position;
            _initialRotation = transform.rotation;
        }

        private void Start()
        {
            EventManager.On("replay_start", OnReplayStart);
        }


        private void Update()
        {
            if (!_replayMode)
            {
                _inputAxis = new Vector2(Input.GetAxis("Horizontal"), Input.GetAxis("Vertical"));
                if (Input.anyKeyDown)
                {
                    Record(new InputRecord
                    {
                        InputAxis = _inputAxis,
                        RecordedTime = _playtime
                    });
                }

                _playtime += UnityEngine.Time.deltaTime;
            }
        }

        private void FixedUpdate()
        {
            _rigidbody.AddForce(new Vector3(_inputAxis.x, _inputAxis.y, 0));
        }

        public void Record(InputRecord inputRecord)
        {
            InputRecords.Add(inputRecord);
        }

        private void OnReplayStart(object obj)
        {
            _replayMode = true;
            if (_replayRoutine != null) StopCoroutine(_replayRoutine);
            _replayRoutine = StartCoroutine(ReplayRoutine());
        }

        private IEnumerator ReplayRoutine()
        {
            var replayTime = 0f;
            var offset = 0.01f;
            transform.position = _initialPosition;
            transform.rotation = _initialRotation;
            _rigidbody.velocity = Vector3.zero;
            _rigidbody.angularVelocity = Vector3.zero;
            while (_replayMode)
            {
                var foundedRecord = InputRecords.FirstOrDefault(_ => Mathf.Abs(_.RecordedTime - replayTime) <= offset);

                if (foundedRecord != null)
                    _inputAxis = foundedRecord.InputAxis;

                replayTime += UnityEngine.Time.deltaTime;
                yield return null;
            }
        }
    }
}