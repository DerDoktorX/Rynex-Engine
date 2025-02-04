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
