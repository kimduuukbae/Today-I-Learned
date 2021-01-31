using System.Collections.Generic;
using UnityEngine;

namespace InputSystem
{
    public class Platform : MonoBehaviour
    {
        public float speed = 4;
        public List<Transform> points;
        private int currentPoint;

        void Start()
        {
            currentPoint = 0;
        }

        void Update()
        {
            var targetIndex = currentPoint + 1 >= points.Count ? 0 : currentPoint + 1;
            var targetPosition = points[targetIndex].transform.position;
            transform.position = Vector3.LerpUnclamped(transform.position, targetPosition, Time.deltaTime * speed);

            if (Vector3.Distance(targetPosition, transform.position) <= 0.1f)
                currentPoint = currentPoint + 1 >= points.Count ? 0 : currentPoint + 1;
        }
    }
}