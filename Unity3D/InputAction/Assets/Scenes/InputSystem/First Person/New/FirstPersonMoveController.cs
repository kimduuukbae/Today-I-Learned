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
        // ��ü (rigidbody)�� �ƴϹǷ�, �߷��� ������ �ް� �ؾ���

        fallingSpeed -= Time.deltaTime * gravity;

        if (controller.isGrounded == true)
            fallingSpeed = 0.0f;

        /*
         * isGrounded = Physics.CheckSphere() -> � ������ ���׶�̸� ����, ����� ����ִ��� Ȯ���ϴ� �Լ�
         * �÷��̾��� �߹ٴڿ� EmptyObject�� ����� Transform�� ������ ��, 
         * isGrounded = Physics.CheckSphere(obj.position, 0.1f, layerMask); 10cm
         */

        controller.Move(new Vector3(0.0f, fallingSpeed, 0.0f));
    }
}
