using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Rynex;

namespace Rynex
{
    public struct Vector3
    {
        public float X, Y, Z;

    // KeyWords
        public static Vector3 Zero => new Vector3(0.0f);

        public static Vector3 Rigth =>  new Vector3(  1.0f,  0.0f,  0.0f );
        public static Vector3 Left =>   new Vector3( -1.0f,  0.0f,  0.0f );
        public static Vector3 Up =>     new Vector3(  0.0f,  1.0f,  0.0f );
        public static Vector3 Down =>   new Vector3(  0.0f, -1.0f,  0.0f );
        public static Vector3 Front =>  new Vector3(  0.0f,  0.0f,  1.0f );
        public static Vector3 Back =>   new Vector3(  0.0f,  0.0f, -1.0f );


        // Vector3 <-> Scalar operations
        // Set
        public Vector3(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }
        public Vector3(Vector3 vector3)
        {
            X = vector3.X;
            Y = vector3.Y;
            Z = vector3.Z;
        }
        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        // operaters
        // Scalar
        public static Vector3 operator *(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
        }
        public static Vector3 operator /(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X / scalar, vector.Y / scalar, vector.Z / scalar);
        }
        public static Vector3 operator +(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X + scalar, vector.Y + scalar, vector.Z + scalar);
        }
        public static Vector3 operator -(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X - scalar, vector.Y - scalar, vector.Z - scalar);
        }
        

        // Vector3
        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }
        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }
        public static Vector3 operator *(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
        }
        public static Vector3 operator /(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X / b.X, a.Y / b.Y, a.Z / b.Z);
        }

        // Vector2
        public static Vector3 operator +(Vector3 a, Vector2 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z);
        }
        public static Vector3 operator -(Vector3 a, Vector2 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z);
        }
        public static Vector3 operator *(Vector3 a, Vector2 b)
        {
            return new Vector3(a.X * b.X, a.Y * b.Y, a.Z);
        }
        public static Vector3 operator /(Vector3 a, Vector2 b)
        {
            return new Vector3(a.X / b.X, a.Y / b.Y, a.Z);
        }


    }
}
