using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

       
    }



    public class GeometryComponent : Component
    {
       
        public void SetVertex<T>(T[] vertices) where T : unmanaged
        {
            unsafe
            {
                fixed (T* vertexPointer = vertices)
                    InternalCalls.GeometryComponent_SetVertex(Entity.ID, (IntPtr)vertexPointer, (uint)sizeof(T) * (uint)vertices.Length); 
            }
        }

#if true
        public void SetIndex<T>(T[] index)
        {
            unsafe
            {
                fixed (T* vertexPointer = index)
                    InternalCalls.GeomtryComponent_SetIndex(Entity.ID, (IntPtr)vertexPointer, (uint)index.Length);
            }
        }
#endif

    }
}
