using UnityEngine;
using System;
using UnityEngine.InputSystem;

namespace InputSystem.First_Person.scripts
{
    public class FirstPersonMover : MonoBehaviour, FirstPersonActionControl.ICharacterActions
    {
        private CharacterController _characterController;
        private Vector2 _inputAxis;
        private float _gravity = -9.8f;
        private float _fallVelocity;
        private bool _isJump;
        private bool _isSprint;

        [SerializeField] private float speed = 10;
        [SerializeField] private float jumpHeight = 3f;
        [SerializeField] private Transform groundCheckTransform;
        [SerializeField] private float groundDistanceToCheck;
        [SerializeField] private LayerMask groundLayer;
        private FirstPersonActionControl _actionControl;

        private void Awake()
        {
            _characterController = GetComponent<CharacterController>();
        }

        private void OnEnable()
        {
            _actionControl = new FirstPersonActionControl();
            _actionControl.Character.SetCallbacks(this);

            _actionControl.Character.Enable();
        }

        private void Update()
        {
            var isGrounded = Physics.CheckSphere(groundCheckTransform.position, groundDistanceToCheck, groundLayer);
            if (isGrounded)
            {
                if (_isJump) _fallVelocity = Mathf.Sqrt(jumpHeight * -2f * _gravity);
                else _fallVelocity = -1f; 
            }

            _fallVelocity += _gravity * Time.deltaTime;

            var moveVector = _inputAxis * (_isSprint ? 2f : 1f);
            var dir = transform.right * moveVector.x + transform.forward * moveVector.y +
                      new Vector3(0, _fallVelocity, 0);

            _characterController.Move(dir * (Time.deltaTime * speed));
        }

        public void OnMove(InputAction.CallbackContext context)
        {
            _inputAxis = context.ReadValue<Vector2>();
            print(_inputAxis);
        }

        public void OnJump(InputAction.CallbackContext context)
        {
            if (context.phase == InputActionPhase.Performed) _isJump = true;
            else if (context.phase == InputActionPhase.Canceled) _isJump = false;
        }

        public void OnSprint(InputAction.CallbackContext context)
        {
            if (context.phase == InputActionPhase.Performed) _isSprint = true;
            else if (context.phase == InputActionPhase.Canceled) _isSprint = false;
        }
    }
}