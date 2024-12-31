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
        public static Vector4 operator /(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X / scalar, vector.Y / scalar, vector.Z / scalar, vector.W / scalar);
        }
        public static Vector4 operator +(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X + scalar, vector.Y + scalar, vector.Z + scalar, vector.W + scalar);
        }
        public static Vector4 operator -(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X - scalar, vector.Y - scalar, vector.Z - scalar, vector.W - scalar);
        }


        public static Vector4 operator *(float scalar, Vector4 vector)
        {
            return new Vector4(scalar*vector.X , scalar*vector.Y , scalar*vector.Z  , scalar*vector.W);
        }
        public static Vector4 operator /(float scalar, Vector4 vector)
        {
            return new Vector4( scalar/vector.X,  scalar / vector.Y,  scalar/vector.Z, scalar /vector.W);
        }
        public static Vector4 operator +(float scalar, Vector4 vector)
        {
            return new Vector4(scalar+vector.X , scalar + vector.Y, scalar+vector.Z , scalar + vector.W);
        }
        public static Vector4 operator -(float scalar, Vector4 vector)
        {
            return new Vector4(scalar - vector.X, scalar - vector.Y, scalar - vector.Z, scalar - vector.W);
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


        // Vector2
        public static Vector4 operator +(Vector4 a, Vector2 b)
        {
            return new Vector4(a.X + b.X, a.Y + b.Y, a.Z, a.W);
        }
        public static Vector4 operator -(Vector4 a, Vector2 b)
        {
            return new Vector4(a.X - b.X, a.Y - b.Y, a.Z, a.W);
        }
        public static Vector4 operator *(Vector4 a, Vector2 b)
        {
            return new Vector4(a.X * b.X, a.Y * b.Y, a.Z, a.W);
        }
        public static Vector4 operator /(Vector4 a, Vector2 b)
        {
            return new Vector4(a.X / b.X, a.Y / b.Y, a.Z, a.W);
        }

        // Vector3
        public static Vector4 operator +(Vector4 a, Vector3 b)
        {
            return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W);
        }
        public static Vector4 operator -(Vector4 a, Vector3 b)
        {
            return new Vector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W);
        }
        public static Vector4 operator *(Vector4 a, Vector3 b)
        {
            return new Vector4(a.X * b.X, a.Y * b.Y, a.Z * b.Z, a.W);
        }
        public static Vector4 operator /(Vector4 a, Vector3 b)
        {
            return new Vector4(a.X / b.X, a.Y / b.Y, a.Z / b.Z, a.W);
        }

        // Vector4
        public static Vector4 operator +(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.Z + b.W);
        }
        public static Vector4 operator -(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.Z - b.W);
        }
        public static Vector4 operator *(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X * b.X, a.Y * b.Y, a.Z * b.Z, a.Z * b.W);
        }
        public static Vector4 operator /(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X / b.X, a.Y / b.Y, a.Z / b.Z, a.Z / b.Z);
        }

    }
}
