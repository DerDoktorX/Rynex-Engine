using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Rynex
{
    public static class Event
    {
        public static bool IsMouseOnViewPort()
        {
            return InternalCalls.Input_IsMouseOnViewPort();
        }

        public static bool IsWindowResize()
        {
            return InternalCalls.Input_IsWindowResize();
        }
    }
}
