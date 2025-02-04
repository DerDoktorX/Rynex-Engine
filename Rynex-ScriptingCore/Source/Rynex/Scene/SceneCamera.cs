

namespace Rynex
{
    public class SceneCamera : Camera
    {
        public enum ProjectionType { Perspectiv = 0, Orthographic = 1 };

        private Entity Entity;
        internal SceneCamera(Entity entity)
        {
            Entity = entity;
        }

        public void SetViewPortSize(uint withe, uint heigth)
        {
            InternalCalls.CameraComponent_Camera_SetViewPortSize(Entity.ID, withe, heigth);
        }
        public void SetOrthoGrafic(float sizen, float nearClip, float farClip)
        {
            InternalCalls.CameraComponent_Camera_SetOrthoGrafic(Entity.ID, sizen, nearClip, farClip);
        }
        public void SetPerspectiv(float verticleFow, float nearClip, float farClip)
        {
            InternalCalls.CameraComponent_Camera_SetPerspectiv(Entity.ID, verticleFow, nearClip, farClip);
        }
        public void SetOrthograficSize(float size)
        {
            InternalCalls.CameraComponent_Camera_SetOrthograficSize(Entity.ID, size);
        }
        public void SetOrthograficNearClipe(float size)
        {
            InternalCalls.CameraComponent_Camera_SetOrthograficNearClipe(Entity.ID, size);
        }
        public void SetOrthograficFarClipe(float size)
        {
            InternalCalls.CameraComponent_Camera_SetOrthograficFarClipe(Entity.ID, size);

        }

        public float GetOrthographicSize()
        {
            return InternalCalls.CameraComponent_Camera_GetOrthographicSize(Entity.ID);
        }
        public float GetOrthographicNearClipe()
        {
            return InternalCalls.CameraComponent_Camera_GetOrthographicNearClipe(Entity.ID);
        }
        public float GetOrthographicFarClipe()
        {
            return InternalCalls.CameraComponent_Camera_GetOrthographicFarClipe(Entity.ID);
        }

        public void SetPerspectivVerticleFOV(float verticleVow)
        {
            InternalCalls.CameraComponent_Camera_SetPerspectivVerticleFOV(Entity.ID, verticleVow);
        }
        public void SetPerspectivNearClipe(float nearClip)
        {
            InternalCalls.CameraComponent_Camera_SetPerspectivNearClipe(Entity.ID, nearClip);
        }
        public void SetPerspectivFarClipe(float farClipe)
        {
            InternalCalls.CameraComponent_Camera_SetPerspectivFarClipe(Entity.ID, farClipe);
        }

        public float GetPerspectivVerticleFOV()
        {
            return InternalCalls.CameraComponent_Camera_GetPerspectivVerticleFOV(Entity.ID);
        }
        public float GetPerspectivNearClipe()
        {
            return InternalCalls.CameraComponent_Camera_GetPerspectivNearClipe(Entity.ID);
        }
        public float GetPerspectivFarClipe()
        {
            return InternalCalls.CameraComponent_Camera_GetPerspectivFarClipe(Entity.ID);
        }

        public ProjectionType GetProjectionType()
        {
            return InternalCalls.CameraComponent_Camera_GetProjectionType(Entity.ID);
        }
        public void SetProjectionType(ProjectionType type)
        {
            InternalCalls.CameraComponent_Camera_SetProjectionType(Entity.ID, type);
        }

        public new Matrix4x4 GetProjektion()
        {
            return InternalCalls.CameraComponent_Camera_GetProjektion(Entity.ID);
        }
#if flase
        public void SetProjektion(Matrix4x4 matrix)
        {
            InternalCalls.CameraComponent_Camera_SetProjektion(Entity.ID, ref matrix);
        }
#endif
    }
}
