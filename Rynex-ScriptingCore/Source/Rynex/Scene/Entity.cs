using System;
using System.Diagnostics.Contracts;

namespace Rynex
{

    public class Entity
    {
        protected internal Entity() { ID = 0; }
        internal Entity(ulong id)
        {
            ID = id;
            Console.WriteLine($"Create Entity! -> Constructor {ID}!");
            if(!HasComponent<TransformComponent>())
                AddComponent<TransformComponent>();
            Transform = GetComponent<TransformComponent>();
            if (!HasComponent<Matrix4x4Component>())
                AddComponent<Matrix4x4Component>();
            Matrix = GetComponent<Matrix4x4Component>();

        }
        ~Entity()
        {
            Console.WriteLine($"Destroy Entity! -> Destructor {ID}!");
            OnDestroy();
        }

        public readonly ulong ID;

        public TransformComponent Transform;
        public Matrix4x4Component Matrix;


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
