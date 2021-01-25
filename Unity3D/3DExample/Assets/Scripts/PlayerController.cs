using UnityEngine;

public class PlayerController : MonoBehaviour
{

    private Movement3D movement3D;
    private const string h = "Horizontal";
    private const string v = "Vertical";

    private void Awake()
    {
        movement3D = GetComponent<Movement3D>();
    }

    // Update is called once per frame
    void Update()
    {
        float x = Input.GetAxisRaw(h);
        float z = Input.GetAxisRaw(v);

        movement3D.SetDirection(new Vector3(x, 1, z));
    }
}
