namespace InputSystem.First_Person.scripts
{
    using System;
    using First_Person;
    using UnityEngine;
    using UnityEngine.InputSystem;

    public class FirstPersonCamera : MonoBehaviour, FirstPersonActionControl.IMouseMoveActions
    {
        public float mouseSensitivity = 10f;
        private Camera _camera;
        private float _mouseYDelta;
        private float _mouseXDelta;

        private float _xRot;
        private float _yRot;

        private FirstPersonActionControl _actionControl;

        public void OnUpAndDown(InputAction.CallbackContext context) =>
            _mouseYDelta = context.ReadValue<float>();

        public void OnLeftAndRight(InputAction.CallbackContext context) =>
            _mouseXDelta = context.ReadValue<float>();

        void Awake()
        {
            _camera = GetComponentInChildren<Camera>();
        }

        private void Start()
        {
            Cursor.visible = false;
            Cursor.lockState = CursorLockMode.Locked;
        }

        void OnEnable()
        {
            if (_actionControl == null)
            {
                _actionControl = new FirstPersonActionControl();
                _actionControl.MouseMove.SetCallbacks(this);
            }

            _actionControl.MouseMove.Enable();
        }


        private void Update()
        {
            var mouseX = _mouseXDelta * Time.deltaTime * mouseSensitivity;
            var mouseY = _mouseYDelta * Time.deltaTime * mouseSensitivity;

            _xRot -= mouseY;
            _xRot = Mathf.Clamp(_xRot, -90, 90);
            _yRot += mouseX;

            _camera.transform.localRotation = Quaternion.Euler(_xRot, 0, 0);
            transform.Rotate(new Vector3(0, mouseX * 2, 0));
        }
    }
}