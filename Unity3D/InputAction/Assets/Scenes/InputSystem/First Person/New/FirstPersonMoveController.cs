using UnityEngine;
using UnityEngine.InputSystem;

public class FirstPersonMoveController : MonoBehaviour, FirstPersonInputAction.IFirstPersonActions
{
    private FirstPersonInputAction _inputActions;
    private CharacterController controller;
    private Vector2 inputVector;

    [SerializeField] private float characterMoveSpeed = 10.0f;

    private const float gravity = 9.81f;
    private float fallingSpeed = 2.0f;

    public void OnMove(InputAction.CallbackContext context)
    {
        inputVector = context.ReadValue<Vector2>();   //normalized
    }

    private void Awake()
    {
        controller = GetComponent<CharacterController>();
    }

    private void OnEnable()
    {
        if(_inputActions == null)
        _inputActions = new FirstPersonInputAction();

        _inputActions.FirstPerson.SetCallbacks(instance : this);
        _inputActions.FirstPerson.Enable();
    }

    private void OnDisable()
    {
        _inputActions.FirstPerson.Disable();
    }

    void Update()
    {
        var v = (transform.forward * inputVector.y + transform.right * inputVector.x) * Time.deltaTime * characterMoveSpeed;
        controller.Move(v);
        // 강체 (rigidbody)가 아니므로, 중력의 영향을 받게 해야함

        fallingSpeed -= Time.deltaTime * gravity;

        if (controller.isGrounded == true)
            fallingSpeed = 0.0f;

        /*
         * isGrounded = Physics.CheckSphere() -> 어떤 지점에 동그라미를 만들어서, 어떤것이 닿고있는지 확인하는 함수
         * 플레이어의 발바닥에 EmptyObject를 만들어 Transform을 가져온 후, 
         * isGrounded = Physics.CheckSphere(obj.position, 0.1f, layerMask); 10cm
         */

        controller.Move(new Vector3(0.0f, fallingSpeed, 0.0f));
    }
}
