using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Rynex;

namespace Rynex
{
    public struct Vector2
    {
        public float X, Y;

        // KeyWords
        public static Vector2 Zero => new Vector2(0.0f);


        // Vector3 <-> Scalar operations
        // Set
        public Vector2(float scalar)
        {
            X = scalar;
            Y = scalar;
        }

        // operaters
        public static Vector2 operator *(Vector2 vector, float scalar)
        {
            return new Vector2(vector.X * scalar, vector.Y * scalar);
        }


    // Vector2 <-> Vector2 operations
        // Set
        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        // operaters
        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X + b.X, a.Y + b.Y);
        }

    }
}
