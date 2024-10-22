using System;

namespace Rynex
{

    public class Entity
    {
        protected internal Entity() { ID = 0; }
        internal Entity(ulong id)
        {
            ID = id;
            // Console.WriteLine($"Create Entity! -> Constructor {id}!");
        }
        ~Entity()
        {
            // Console.WriteLine("Destroy Entity! -> Destructor!");
            OnDestroy();
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


        public bool HasComponent<T>() where T: Component, new()
        {
            Type componentType = typeof(T);
            Console.WriteLine($"C# -> Entity_HasComponent =  ( ID: {ID},  Component: {componentType} )");

            return InternalCalls.Entity_HasComponent(ID, componentType);

        }
        public T GetComponent<T>() where T : Component, new()
        {

            if(!HasComponent<T>())
                return null;
            T component = new T();
            component.Entity = this;
            return component;
        }
        public bool AddComponent<T>() where T : Component, new()
        {

            if (!HasComponent<T>())
            {
                InternalCalls.Entity_AddComponent(ID, typeof(T));
                return true;
            } 
            return false;
        }

        protected virtual void OnCreate() { }
        protected virtual void OnDestroy()
        {
            // Console.WriteLine("Destroy Entity!"); 
        }

        protected virtual void OnUpdate(float ts) { }

        public void DestroyEntity()
        {
            InternalCalls.Entity_Dealete(ID);
        }

        public void UpdateEntitiyMatrix()
        {
            InternalCalls.Entity_UpdateMatrixe(ID);
        }

    }


}
