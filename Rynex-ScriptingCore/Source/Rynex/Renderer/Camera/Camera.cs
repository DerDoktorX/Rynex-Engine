namespace Rynex
{
    public class Camera
    {
        private Matrix4x4 m_Matrix;
        public Camera()
        {
            m_Matrix = new Matrix4x4();
        }

        public Camera(Matrix4x4 matrix)
        {
            m_Matrix = matrix;
        }

        public Matrix4x4 GetProjektion()
        {
            return m_Matrix;
        }
    }
}
