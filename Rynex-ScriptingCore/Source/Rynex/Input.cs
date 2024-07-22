using System;
using System.Runtime.CompilerServices;

namespace Rynex
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keyCode)
        {
            return InternalCalls.Input_IsKeyDown(keyCode); 
        }
    }
}
