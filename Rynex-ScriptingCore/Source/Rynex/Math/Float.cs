namespace Rynex
{
    public static class MathUtils
    {
        public static float Lerp(float a, float b, float t)
        {
            return a + (b - a) * t;
        }

        public static double Lerp(double a, double b, float t)
        {
            return a + (b - a) * t;
        }



    }

}
