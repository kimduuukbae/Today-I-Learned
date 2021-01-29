using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Assets.Scripts
{
    public class GameManager : SingletonBehaviour<GameManager>
    {
        public GameState state;
    }
}