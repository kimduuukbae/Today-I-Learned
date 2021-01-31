// GENERATED AUTOMATICALLY FROM 'Assets/Scenes/InputSystem/First Person/New/FirstPersonInputAction.inputactions'

using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Utilities;

public class @FirstPersonInputAction : IInputActionCollection, IDisposable
{
    public InputActionAsset asset { get; }
    public @FirstPersonInputAction()
    {
        asset = InputActionAsset.FromJson(@"{
    ""name"": ""FirstPersonInputAction"",
    ""maps"": [
        {
            ""name"": ""FirstPerson"",
            ""id"": ""6d069f12-daa9-4ef6-9226-5a467d5569f3"",
            ""actions"": [
                {
                    ""name"": ""Move"",
                    ""type"": ""PassThrough"",
                    ""id"": ""25495e4e-b3f8-4ef5-a365-e955d30f59b2"",
                    ""expectedControlType"": ""Vector2"",
                    ""processors"": """",
                    ""interactions"": """"
                }
            ],
            ""bindings"": [
                {
                    ""name"": ""2D Vector"",
                    ""id"": ""71d85d69-c283-4741-aff7-f720c34fa446"",
                    ""path"": ""2DVector"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": true,
                    ""isPartOfComposite"": false
                },
                {
                    ""name"": ""up"",
                    ""id"": ""fa52da2a-634f-49e4-8418-b0992400652c"",
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
                    ""id"": ""60fb725b-9732-4b41-a7e7-e116f7d689f4"",
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
                    ""id"": ""cf493ebc-cc33-46d7-9364-5c08e6f20323"",
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
                    ""id"": ""7d44557d-6b9c-47bd-9bdb-751117f07c1c"",
                    ""path"": ""<Keyboard>/d"",
                    ""interactions"": """",
                    ""processors"": """",
                    ""groups"": """",
                    ""action"": ""Move"",
                    ""isComposite"": false,
                    ""isPartOfComposite"": true
                }
            ]
        }
    ],
    ""controlSchemes"": []
}");
        // FirstPerson
        m_FirstPerson = asset.FindActionMap("FirstPerson", throwIfNotFound: true);
        m_FirstPerson_Move = m_FirstPerson.FindAction("Move", throwIfNotFound: true);
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

    // FirstPerson
    private readonly InputActionMap m_FirstPerson;
    private IFirstPersonActions m_FirstPersonActionsCallbackInterface;
    private readonly InputAction m_FirstPerson_Move;
    public struct FirstPersonActions
    {
        private @FirstPersonInputAction m_Wrapper;
        public FirstPersonActions(@FirstPersonInputAction wrapper) { m_Wrapper = wrapper; }
        public InputAction @Move => m_Wrapper.m_FirstPerson_Move;
        public InputActionMap Get() { return m_Wrapper.m_FirstPerson; }
        public void Enable() { Get().Enable(); }
        public void Disable() { Get().Disable(); }
        public bool enabled => Get().enabled;
        public static implicit operator InputActionMap(FirstPersonActions set) { return set.Get(); }
        public void SetCallbacks(IFirstPersonActions instance)
        {
            if (m_Wrapper.m_FirstPersonActionsCallbackInterface != null)
            {
                @Move.started -= m_Wrapper.m_FirstPersonActionsCallbackInterface.OnMove;
                @Move.performed -= m_Wrapper.m_FirstPersonActionsCallbackInterface.OnMove;
                @Move.canceled -= m_Wrapper.m_FirstPersonActionsCallbackInterface.OnMove;
            }
            m_Wrapper.m_FirstPersonActionsCallbackInterface = instance;
            if (instance != null)
            {
                @Move.started += instance.OnMove;
                @Move.performed += instance.OnMove;
                @Move.canceled += instance.OnMove;
            }
        }
    }
    public FirstPersonActions @FirstPerson => new FirstPersonActions(this);
    public interface IFirstPersonActions
    {
        void OnMove(InputAction.CallbackContext context);
    }
}
