using UnityEngine;
using UnityEngine.AI;

public class Player : MonoBehaviour
{
    private NavMeshAgent _agent;
    private Camera cam;
    [SerializeField] private LayerMask mask;

    private void Awake()
    {
        _agent = GetComponent<NavMeshAgent>();
        cam = Camera.main; // Tag∞° MainCamera¿Œ GameObject
    }

    private void Update()
    {
        if(Input.GetMouseButtonDown(0))
        {
            var mousePosition = Input.mousePosition;
            var ray = cam.ScreenPointToRay(mousePosition);

            if (Physics.Raycast(ray, out var hitInfo, mask))
                _agent.SetDestination(hitInfo.point);
        }
    }
}

