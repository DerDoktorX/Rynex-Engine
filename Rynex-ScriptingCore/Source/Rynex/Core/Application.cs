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
