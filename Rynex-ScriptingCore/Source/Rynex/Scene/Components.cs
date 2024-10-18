using System;

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



    public class GeometryComponent : Component
    {

        // old
        public void SetVertex<T>(T[] vertices, uint size = 0) where T : unmanaged
        {
            unsafe
            {
                fixed (T* vertexPointer = vertices)
                    InternalCalls.GeometryComponent_SetVertex(Entity.ID, (IntPtr)vertexPointer, size != 0 ? size : (uint)sizeof(T) * (uint)vertices.Length);
            }
        }

#if true
        public void SetIndex<T>(T[] index, uint count = 0)
        {
            unsafe
            {
                fixed (T* indexPointer = index)
                    InternalCalls.GeomtryComponent_SetIndex(Entity.ID, (IntPtr)indexPointer, count != 0 ? count : (uint)index.Length);
            }
        }
#endif

        public void SetPrimitv(VertexArray.Primitv primitv)
        {
            InternalCalls.GeomtryComponent_SetPrimitv(Entity.ID, (int)primitv);
        }
        // new 

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
}
