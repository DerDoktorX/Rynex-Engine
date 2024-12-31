

using System.Runtime.CompilerServices;

namespace Rynex
{
    public struct Matrix4x4
    {
        private float[,] m_Elements;

        public Matrix4x4(float[,] values = null)
        {
            m_Elements = 16 == values.Length ? values : new float[4,4];
        }

        public float this[int r, int c]
        {
            get { return m_Elements[r, c]; }
            set { m_Elements[r, c] = value; }
        }

        public Vector4 this[int r]
        {
            get => new Vector4(m_Elements[r,0], m_Elements[r,1], m_Elements[r, 2] , m_Elements[r, 3]);

            set 
            {
                m_Elements[r, 0] = value.X;
                m_Elements[r, 1] = value.Y;
                m_Elements[r, 2] = value.Z; 
                m_Elements[r, 3] = value.W;
            }
        }
    }
}
