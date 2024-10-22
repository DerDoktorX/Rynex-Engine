using System;
using System.Reflection;
using Rynex;

namespace Rynex
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {

        public Vector3 Translation
        {
            get
            {
                
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }

            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 rotation);
                return rotation;
            }

            set
            {
                InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 scale);
                return scale;
            }

            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
            }
        }


        public bool Change
        {
            get
            {
                InternalCalls.TransformComponent_GetChange(Entity.ID, out bool change);
                return change;
            }

            set
            {
                InternalCalls.TransformComponent_SetChange(Entity.ID, ref value);
            }
        }
    }

    public class GeomtryComponent : Component
    {

#if false        // old
        public void SetVertex<T>(T[] vertices, uint size = 0) where T : unmanaged
        {
            unsafe
            {
                fixed (T* vertexPointer = vertices)
                    InternalCalls.GeometryComponent_SetVertex(Entity.ID, (IntPtr)vertexPointer, size != 0 ? size : (uint)sizeof(T) * (uint)vertices.Length);
            }
        }


        public void SetIndex<T>(T[] index, uint count = 0)
        {
            unsafe
            {
                fixed (T* indexPointer = index)
                    InternalCalls.GeomtryComponent_SetIndex(Entity.ID, (IntPtr)indexPointer, count != 0 ? count : (uint)index.Length);
            }
        }


        public void SetPrimitv(VertexArray.Primitv primitv)
        {
            InternalCalls.GeomtryComponent_SetPrimitv(Entity.ID, (int)primitv);
        }
        // new 
#endif

        public VertexBuffer Buffer
        {
            get
            {
                InternalCalls.GeometryComponent_GetBuffer(Entity.ID, out ulong handle);
                return new VertexBuffer(handle);
            }

            set
            {
                ulong uuid = value.Handle.UUID;
                Console.WriteLine($"C# ->  Set Buffer = VertexBuffer {uuid}, {(ulong)uuid} Begin");
                InternalCalls.GeometryComponent_SetBuffer(Entity.ID, ref uuid);
                Console.WriteLine($"C# ->  Set Buffer = VertexBuffer {uuid}, {(ulong)uuid} Ende");
            }
        }

        public VertexArray Geomtry
        {
            get
            {
                InternalCalls.GeometryComponent_GetGeometry(Entity.ID, out ulong handle);
                return new VertexArray(handle);
            }

            set
            {


                ulong uuid = value.Handle.UUID;
                Console.WriteLine($"C# ->  Set Geomtry = VertexArray {uuid}, {(ulong)uuid} Begin");
                InternalCalls.GeometryComponent_SetGeometry(Entity.ID, ref uuid);
                Console.WriteLine($"C# ->  Set Geomtry = VertexArray {uuid}, {(ulong)uuid} Ende");
            }
        }
    }

    public class TagComponent : Component
    {
        public string Tag
        {
            get
            {
                InternalCalls.TagComponent_GetTag(Entity.ID, out string name);
                return name;
            }

            set
            {
                InternalCalls.TagComponent_SetTag(Entity.ID, ref value);
            }
        }
    }

    public class Matrix4x4Component : Component
    {
        public Matrix4x4 Matrix4x4
        {
            get
            {
                InternalCalls.Matrix4x4Component_GetMatrix4x4(Entity.ID, out Matrix4x4 matrix);
                return matrix;
            }

            set
            {
                InternalCalls.Matrix4x4Component_SetMatrix4x4(Entity.ID, ref value);
            }
        }

        public Matrix4x4 GlobleMatrix4x4
        {
            get
            {
                InternalCalls.Matrix4x4Component_GetMatrix4x4(Entity.ID, out Matrix4x4 matrix);
                return matrix;
            }

            set
            {
                InternalCalls.Matrix4x4Component_SetMatrix4x4(Entity.ID, ref value);
            }
        }
    }

    public class FrameBufferComponent : Component
    {
        public Framebuffer Framebuffer
        {
            get
            {
                InternalCalls.FrameBufferComponent_GetFrameBuffer(Entity.ID, out ulong handle);
                return new Framebuffer(handle);
            }

            set
            {
                ulong uuid = value.Handle.UUID;
                InternalCalls.FrameBufferComponent_SetFrameBuffer(Entity.ID, ref uuid);
            }
        }
    }

    public class MeshComponent : Component
    {
        public Model Model
        {
            get
            {
                InternalCalls.MeshComponent_GetModelR(Entity.ID, out ulong handle);
                return new Model(handle);
            }

            set
            {
                ulong uuid = value.Handle.UUID;
                InternalCalls.MeshComponent_SetModelR(Entity.ID, ref uuid);
            }
        }
    }

    public class SpriteRendererComponent : Component
    {
        public Vector4 Color
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }

            set
            {
                InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, ref value);
            }
        }

        public Texture Texture
        {
            get
            {
                InternalCalls.MeshComponent_GetModelR(Entity.ID, out ulong handle);
                return new Texture(handle);
            }

            set
            {
                ulong uuid = value.Handle.UUID;
                InternalCalls.MeshComponent_SetModelR(Entity.ID, ref uuid);
            }
        }
    }

    public class CameraComponent : Component
    {
#if false
        public SceneCamera Camera
        {
            get
            {
                InternalCalls.CameraComponent_Camera(Entity.ID,);
                return Primary;
            }

            set
            {
                InternalCalls.CameraComponent_SetPrimary(Entity.ID);
            }
        }
#endif
        public Matrix4x4 ViewMatrixe
        {
            get
            {
                InternalCalls.Matrix4x4Component_GetMatrix4x4(Entity.ID, out Matrix4x4 matrix);
                return matrix;
            }

            set
            {
                InternalCalls.Matrix4x4Component_SetMatrix4x4(Entity.ID, ref value);
            }
        }
#if false
        public Matrix4x4 ProjectionMatrixe
        {
            get
            {
                InternalCalls.CameraComponent_GetProjection(Entity.ID, out Matrix4x4 matrix);
                return matrix;
            }

            set
            {
                InternalCalls.CameraComponent_SetProjection(Entity.ID, ref value);
            }
        }
#endif
        public bool Primary
        {
            get
            {
                InternalCalls.CameraComponent_GetPrimary(Entity.ID, out bool primary);
                return Primary;
            }

            set
            {
                InternalCalls.CameraComponent_SetPrimary(Entity.ID, ref value);
            }
        }

        public bool FixedAspectRotaion
        {
            get
            {
                InternalCalls.CameraComponent_GetPrimary(Entity.ID, out bool fixedAspectRotaion);
                return fixedAspectRotaion;
            }

            set
            {
               
                InternalCalls.CameraComponent_SetPrimary(Entity.ID, ref value);
            }
        }
    }

    public class MaterialComponent : Component
    {
        public Shader Shader
        {
            get
            {
                InternalCalls.MaterialComponent_GetShader(Entity.ID, out ulong handle);
                return new Shader(handle);
            }

            set
            {
                ulong uuid = value.Handle.UUID;
                InternalCalls.MaterialComponent_SetShader(Entity.ID, ref uuid);
            }
        }

        public Vector3 Color
        {
            get
            {
                InternalCalls.MaterialComponent_GetColor(Entity.ID, out Vector3 color);
                return color;
            }

            set
            {
                InternalCalls.MaterialComponent_SetColor(Entity.ID, ref value);
            }
        }
    }

    public class ScriptComponent : Component
    {
        public string ScriptName
        {
            get
            {
                InternalCalls.ScriptComponent_GetName(Entity.ID, out string name);
                return name;
            }

            set
            {
                InternalCalls.ScriptComponent_SetName(Entity.ID, ref value);
            }
        }
    }
}
