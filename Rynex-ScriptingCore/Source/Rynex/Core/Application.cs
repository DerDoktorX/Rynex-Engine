using Rynex;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Rynex
{
    public static class Application
    {
        static Vector2 GetWindowSize()
        {
            return InternalCalls.Application_GetWindowSize();
        }

        static Vector2 GetMousePixelPosition()
        {
            return InternalCalls.Application_GetMousePixelPosition();
        }

    }
}
