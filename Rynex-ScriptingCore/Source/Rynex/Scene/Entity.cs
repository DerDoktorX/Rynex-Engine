using System;

namespace Rynex
{

    public class Entity
    {
        protected internal Entity() { ID = 0; }
        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }

            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }




#if false

        public bool HasComponent<T>() where T: Component, new()
        {
            Type componentType = typeof(T);
            Console.WriteLine($"C# -> Entity_HasComponent =  ( ID: {ID},  Component: {componentType} )");

            return InternalCalls.Entity_HasComponent(ID, componentType);

        }
#endif
        public T GetComponent<T>() where T : Component, new()
        {

            //if(!HasComponent<T>())
            //    return null;

            Console.WriteLine("C# -> GetComponent");
            T componten = new T() { Entity = this };
            return componten;
        }

    }


}
