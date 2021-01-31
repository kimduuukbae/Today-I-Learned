// GENERATED AUTOMATICALLY FROM 'Assets/Scenes/InputSystem/First Person/Settings/FirstPerson.inputactions'

using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Utilities;

namespace InputSystem.First_Person
{
    public class @FirstPersonActionControl : IInputActionCollection, IDisposable
    {
        public InputActionAsset asset { get; }
        public @FirstPersonActionControl()
        {
            asset = InputActionAsset.FromJson(@"{
    ""name"": ""FirstPerson"",
    ""maps"": [
        {
            ""name"": ""MouseMove"",
            ""id"": ""5eb071a7-5921-4f2b-bdfe-eca1acca9128"",
            ""actions"": [
                {
                    ""name"": ""Up And Down"",
                    ""type"": ""PassThrough"",
                    ""id"": ""e02a7443-cdd4-4271-b1e9-b8fbe1d401c3"",
                    ""expectedControlType"": ""Axis"",
                    ""processors"": """",
                    ""interactions"": """"
                },
                {
                    ""name"": ""Left And Right"",
                    ""type"": ""PassThrough"",
                    ""id"": ""5edc5cee-5bcd-477a-aa55-07c418820ef8"",
                    ""expectedControlType"": ""Axis"",
                    ""processors"": ""AxisDeadzone"",
                    ""interactions"": """"
                }
            ],
            ""bindings"": [
                {
                    ""name"": """",
                    ""id"": ""83a7c1ec-9efa-439b-bf8a-5c4d28367c1f"",
                    ""path"": ""<Mouse>/delta/y"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Up And Down"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": false
                },
                {
                    ""name"": """",
                    ""id"": ""979df8d7-6dc6-48c3-99b9-ac4d6fce4102"",
                    ""path"": ""<Mouse>/delta/x"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Left And Right"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": false
                }
            ]
        },
        {
            ""name"": ""Character"",
            ""id"": ""a266ba14-27c1-4ed4-9cae-740cf964f862"",
            ""actions"": [
                {
                    ""name"": ""Move"",
                    ""type"": ""PassThrough"",
                    ""id"": ""2177ab41-ab8a-472a-9d0e-c090260df80b"",
                    ""expectedControlType"": ""Vector2"",
                    ""processors"": ""AxisDeadzone"",
                    ""interactions"": """"
                },
                {
                    ""name"": ""Jump"",
                    ""type"": ""Button"",
                    ""id"": ""714a7af1-5bc5-4cf7-bb0c-ebf7ebd035d0"",
                    ""expectedControlType"": ""Button"",
                    ""processors"": ""AxisDeadzone"",
                    ""interactions"": """"
                },
                {
                    ""name"": ""Sprint"",
                    ""type"": ""Button"",
                    ""id"": ""583cfbab-3f8c-4831-ad80-9531c00c0cb7"",
                    ""expectedControlType"": ""Button"",
                    ""processors"": ""AxisDeadzone"",
                    ""interactions"": """"
                }
            ],
            ""bindings"": [
                {
                    ""name"": ""Keyboard"",
                    ""id"": ""e3da1943-7867-474f-ac8e-15a99608185d"",
                    ""path"": ""2DVector(mode=1)"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": true,
                    ""isPartOfComposite"": false
                },
                {
                    ""name"": ""up"",
                    ""id"": ""0094c8b5-788c-493a-94b3-76772786c79a"",
                    ""path"": ""<Keyboard>/w"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": true
                },
                {
                    ""name"": ""down"",
                    ""id"": ""3e6c1a87-843d-4fe5-8ec7-7971715db10d"",
                    ""path"": ""<Keyboard>/s"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": true
                },
                {
                    ""name"": ""left"",
                    ""id"": ""add5b500-2b08-46af-b026-38b5a8d50f25"",
                    ""path"": ""<Keyboard>/a"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": true
                },
                {
                    ""name"": ""right"",
                    ""id"": ""9b35f34d-0cf2-45c5-9082-0dceab6be1d7"",
                    ""path"": ""<Keyboard>/d"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": true
                },
                {
                    ""name"": """",
                    ""id"": ""ff5866a8-4ba6-4d3e-90db-6a9ef0cd4656"",
                    ""path"": ""<Keyboard>/space"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Jump"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": false
                },
                {
                    ""name"": """",
                    ""id"": ""efbf8f24-617d-4656-8ada-f002c7790307"",
                    ""path"": ""<Keyboard>/leftShift"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Sprint"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": false
                }
            ]
        }
    ],
    ""controlSchemes"": [
        {
            ""name"": ""Default"",
            ""bindingGroup"": ""Default"",
            ""devices"": [
                {
                    ""devicePath"": ""<Keyboard>"",
                    ""isOptional"": false,
                    ""isOR"": false
                },
                {
                    ""devicePath"": ""<Mouse>"",
                    ""isOptional"": false,
                    ""isOR"": false
                }
            ]
        }
    ]
}");
            // MouseMove
            m_MouseMove = asset.FindActionMap("MouseMove", throwIfNotFound: true);
            m_MouseMove_UpAndDown = m_MouseMove.FindAction("Up And Down", throwIfNotFound: true);
            m_MouseMove_LeftAndRight = m_MouseMove.FindAction("Left And Right", throwIfNotFound: true);
            // Character
            m_Character = asset.FindActionMap("Character", throwIfNotFound: true);
            m_Character_Move = m_Character.FindAction("Move", throwIfNotFound: true);
            m_Character_Jump = m_Character.FindAction("Jump", throwIfNotFound: true);
            m_Character_Sprint = m_Character.FindAction("Sprint", throwIfNotFound: true);
        }

        public void Dispose()
        {
            UnityEngine.Object.Destroy(asset);
        }

        public InputBinding? bindingMask
        {
            get => asset.bindingMask;
            set => asset.bindingMask = value;
        }

        public ReadOnlyArray<InputDevice>? devices
        {
            get => asset.devices;
            set => asset.devices = value;
        }

        public ReadOnlyArray<InputControlScheme> controlSchemes => asset.controlSchemes;

        public bool Contains(InputAction action)
        {
            return asset.Contains(action);
        }

        public IEnumerator<InputAction> GetEnumerator()
        {
            return asset.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Enable()
        {
            asset.Enable();
        }

        public void Disable()
        {
            asset.Disable();
        }

        // MouseMove
        private readonly InputActionMap m_MouseMove;
        private IMouseMoveActions m_MouseMoveActionsCallbackInterface;
        private readonly InputAction m_MouseMove_UpAndDown;
        private readonly InputAction m_MouseMove_LeftAndRight;
        public struct MouseMoveActions
        {
            private @FirstPersonActionControl m_Wrapper;
            public MouseMoveActions(@FirstPersonActionControl wrapper) { m_Wrapper = wrapper; }
            public InputAction @UpAndDown => m_Wrapper.m_MouseMove_UpAndDown;
            public InputAction @LeftAndRight => m_Wrapper.m_MouseMove_LeftAndRight;
            public InputActionMap Get() { return m_Wrapper.m_MouseMove; }
            public void Enable() { Get().Enable(); }
            public void Disable() { Get().Disable(); }
            public bool enabled => Get().enabled;
            public static implicit operator InputActionMap(MouseMoveActions set) { return set.Get(); }
            public void SetCallbacks(IMouseMoveActions instance)
            {
                if (m_Wrapper.m_MouseMoveActionsCallbackInterface != null)
                {
                    @UpAndDown.started -= m_Wrapper.m_MouseMoveActionsCallbackInterface.OnUpAndDown;
                    @UpAndDown.performed -= m_Wrapper.m_MouseMoveActionsCallbackInterface.OnUpAndDown;
                    @UpAndDown.canceled -= m_Wrapper.m_MouseMoveActionsCallbackInterface.OnUpAndDown;
                    @LeftAndRight.started -= m_Wrapper.m_MouseMoveActionsCallbackInterface.OnLeftAndRight;
                    @LeftAndRight.performed -= m_Wrapper.m_MouseMoveActionsCallbackInterface.OnLeftAndRight;
                    @LeftAndRight.canceled -= m_Wrapper.m_MouseMoveActionsCallbackInterface.OnLeftAndRight;
                }
                m_Wrapper.m_MouseMoveActionsCallbackInterface = instance;
                if (instance != null)
                {
                    @UpAndDown.started += instance.OnUpAndDown;
                    @UpAndDown.performed += instance.OnUpAndDown;
                    @UpAndDown.canceled += instance.OnUpAndDown;
                    @LeftAndRight.started += instance.OnLeftAndRight;
                    @LeftAndRight.performed += instance.OnLeftAndRight;
                    @LeftAndRight.canceled += instance.OnLeftAndRight;
                }
            }
        }
        public MouseMoveActions @MouseMove => new MouseMoveActions(this);

        // Character
        private readonly InputActionMap m_Character;
        private ICharacterActions m_CharacterActionsCallbackInterface;
        private readonly InputAction m_Character_Move;
        private readonly InputAction m_Character_Jump;
        private readonly InputAction m_Character_Sprint;
        public struct CharacterActions
        {
            private @FirstPersonActionControl m_Wrapper;
            public CharacterActions(@FirstPersonActionControl wrapper) { m_Wrapper = wrapper; }
            public InputAction @Move => m_Wrapper.m_Character_Move;
            public InputAction @Jump => m_Wrapper.m_Character_Jump;
            public InputAction @Sprint => m_Wrapper.m_Character_Sprint;
            public InputActionMap Get() { return m_Wrapper.m_Character; }
            public void Enable() { Get().Enable(); }
            public void Disable() { Get().Disable(); }
            public bool enabled => Get().enabled;
            public static implicit operator InputActionMap(CharacterActions set) { return set.Get(); }
            public void SetCallbacks(ICharacterActions instance)
            {
                if (m_Wrapper.m_CharacterActionsCallbackInterface != null)
                {
                    @Move.started -= m_Wrapper.m_CharacterActionsCallbackInterface.OnMove;
                    @Move.performed -= m_Wrapper.m_CharacterActionsCallbackInterface.OnMove;
                    @Move.canceled -= m_Wrapper.m_CharacterActionsCallbackInterface.OnMove;
                    @Jump.started -= m_Wrapper.m_CharacterActionsCallbackInterface.OnJump;
                    @Jump.performed -= m_Wrapper.m_CharacterActionsCallbackInterface.OnJump;
                    @Jump.canceled -= m_Wrapper.m_CharacterActionsCallbackInterface.OnJump;
                    @Sprint.started -= m_Wrapper.m_CharacterActionsCallbackInterface.OnSprint;
                    @Sprint.performed -= m_Wrapper.m_CharacterActionsCallbackInterface.OnSprint;
                    @Sprint.canceled -= m_Wrapper.m_CharacterActionsCallbackInterface.OnSprint;
                }
                m_Wrapper.m_CharacterActionsCallbackInterface = instance;
                if (instance != null)
                {
                    @Move.started += instance.OnMove;
                    @Move.performed += instance.OnMove;
                    @Move.canceled += instance.OnMove;
                    @Jump.started += instance.OnJump;
                    @Jump.performed += instance.OnJump;
                    @Jump.canceled += instance.OnJump;
                    @Sprint.started += instance.OnSprint;
                    @Sprint.performed += instance.OnSprint;
                    @Sprint.canceled += instance.OnSprint;
                }
            }
        }
        public CharacterActions @Character => new CharacterActions(this);
        private int m_DefaultSchemeIndex = -1;
        public InputControlScheme DefaultScheme
        {
            get
            {
                if (m_DefaultSchemeIndex == -1) m_DefaultSchemeIndex = asset.FindControlSchemeIndex("Default");
                return asset.controlSchemes[m_DefaultSchemeIndex];
            }
        }
        public interface IMouseMoveActions
        {
            void OnUpAndDown(InputAction.CallbackContext context);
            void OnLeftAndRight(InputAction.CallbackContext context);
        }
        public interface ICharacterActions
        {
            void OnMove(InputAction.CallbackContext context);
            void OnJump(InputAction.CallbackContext context);
            void OnSprint(InputAction.CallbackContext context);
        }
    }
}
