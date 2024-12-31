using System;
using System.Runtime.InteropServices;

namespace Rynex
{
  
    public struct Vector3
    {
        public float X, Y, Z;

        // KeyWords
        public static Vector3 Zero => new Vector3(0.0f);

        public static Vector3 Rigth => new Vector3(1.0f, 0.0f, 0.0f);
        public static Vector3 Left => new Vector3(-1.0f, 0.0f, 0.0f);
        public static Vector3 Up => new Vector3(0.0f, 1.0f, 0.0f);
        public static Vector3 Down => new Vector3(0.0f, -1.0f, 0.0f);
        public static Vector3 Front => new Vector3(0.0f, 0.0f, 1.0f);
        public static Vector3 Back => new Vector3(0.0f, 0.0f, -1.0f);


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


        public Vector3 Normalize()
        {
            float length = Length();
            if (length == 0)
                return new Vector3(0, 0, 0);
            return new Vector3(X / length, Y / length, Z / length);
        }

        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
        }

        public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            return new Vector3(MathUtils.Lerp(a.X, b.X, t), MathUtils.Lerp(a.Y, b.Y, t), MathUtils.Lerp(a.Z, b.Z, t));
        }

        // The value returned equals a + (b - a) * t (which can also be written a * (1-t) + b*t).
        public Vector3 Lerp(Vector3 a, float t)
        {
            Vector3 b = this;
            return new Vector3(MathUtils.Lerp(a.X, b.X, t), MathUtils.Lerp(a.Y, b.Y, t), MathUtils.Lerp(a.Z, b.Z, t));
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


        public static Vector3 operator *(float scalar, Vector3 vector)
        {
            return new Vector3(scalar * vector.X, scalar * vector.Y, scalar * vector.Z);
        }
        public static Vector3 operator /(float scalar, Vector3 vector)
        {
            return new Vector3(scalar / vector.X, scalar / vector.Y, scalar / vector.Z);
        }
        public static Vector3 operator +(float scalar, Vector3 vector)
        {
            return new Vector3(scalar + vector.X, scalar + vector.Y, scalar + vector.Z);
        }
        public static Vector3 operator -(float scalar, Vector3 vector)
        {
            return new Vector3(scalar - vector.X, scalar - vector.Y, scalar - vector.Z);
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

        // Vector4
        public static Vector3 operator +(Vector3 a, Vector4 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }
        public static Vector3 operator -(Vector3 a, Vector4 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }
        public static Vector3 operator *(Vector3 a, Vector4 b)
        {
            return new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
        }
        public static Vector3 operator /(Vector3 a, Vector4 b)
        {
            return new Vector3(a.X / b.X, a.Y / b.Y, a.Z / b.Z);
        }
    }
}
