using UnityEngine;

namespace InputSystem.Legacy_Input.Scripts
{
    public class CharacterMover : MonoBehaviour
    {
        private CharacterController _characterController;
        private Animator _animator;
        [SerializeField] private float speed = 10f;
        [SerializeField] private float rotateSpeed = 100f;
        [SerializeField] private float gravity = 9.81f;
        [SerializeField] private Transform groundCheckTransform;
        [SerializeField] private float distanceToCheckGround = 0.1f;

        private float fallingSpeed;

        private void Awake()
        {
            _characterController = GetComponent<CharacterController>();
            _animator = GetComponent<Animator>();
        }

        private void Update()
        {
            var isGrounded = Physics.CheckSphere(groundCheckTransform.position, distanceToCheckGround * 2);
            if (isGrounded)
                fallingSpeed = 0f;
            var hor = Input.GetAxis("Horizontal") * rotateSpeed * Time.deltaTime;
            var ver = Input.GetAxis("Vertical") * speed * Time.deltaTime;
            var dir = new Vector3(hor, 0, ver);
            transform.Rotate(0, hor, 0);
            _animator.SetFloat("Speed", dir.magnitude);
            _characterController.Move(transform.forward * ver + new Vector3(0, fallingSpeed, 0) );
            fallingSpeed -= Time.deltaTime * gravity;
        }
    }
}