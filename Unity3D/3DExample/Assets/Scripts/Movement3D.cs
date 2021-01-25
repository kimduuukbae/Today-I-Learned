using UnityEngine;

public class Movement3D : MonoBehaviour
{
    [SerializeField]
    private float moveSpeed = 5.0f;
    private const float gravity = -9.81f;
    private const float jumpForce = 3.0f;

    private Vector3 moveDirection;
    
    private CharacterController controller;

    private void Awake()
    {
        controller = GetComponent<CharacterController>();
    }

    void Update()
    {
        if (!controller.isGrounded)
            moveDirection.y += gravity * Time.deltaTime;

        controller.Move(moveDirection * moveSpeed * Time.deltaTime);
    }

    public void SetDirection(Vector3 dir)
    {
        moveDirection = new Vector3(dir.x, moveDirection.y, dir.z);
    }

    public void Jump()
    {
        if (controller.isGrounded)
            moveDirection.y = jumpForce;
    }
}
