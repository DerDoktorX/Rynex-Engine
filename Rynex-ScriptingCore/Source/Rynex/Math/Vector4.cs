
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Rynex;

namespace Rynex
{
    public struct Vector4
    {
        public float X, Y, Z, W;

    // KeyWords
        public static Vector4 Zero => new Vector4(0.0f);


    // Vector3 <-> Scalar operations
        // Set
        public Vector4(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
            W = scalar;
        }

        // operaters
        public static Vector4 operator *(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X * scalar, vector.Y * scalar, vector.Z * scalar, vector.W * scalar);
        }



    // Vector4 <-> Vector4 operations
        // Set
        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        // operaters
        public static Vector4 operator +(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
        }

    }
}
