using UnityEngine;
using UnityEngine.InputSystem;

namespace InputSystem._2D.Scripts
{
    using First_Person;

    public class Virus : MonoBehaviour, FirstPersonActionControl.ICharacterActions
    {
        private Vector2 _moveVector;
        private InputAction inputAction;

        public void OnMove(InputAction.CallbackContext context)
        {
            throw new System.NotImplementedException();
        }

        public void OnJump(InputAction.CallbackContext context)
        {
            throw new System.NotImplementedException();
        }

        public void OnSprint(InputAction.CallbackContext context)
        {
            throw new System.NotImplementedException();
        }
    }
}