using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rynex;

namespace Sanbox
{
    public class TestMain : Entity
    {
        private Entity EntityCreate;
        private Entity EntityCamera;
        private TransformComponent transformCamera1;
        protected void OnCreate()
        {
            Console.WriteLine("TestMain.OnCreate");
            EntityCreate = CurentScene.AddEntity("Test Create 1");
            EntityCreate?.AddComponent<SpriteRendererComponent>();
            EntityCreate?.AddComponent<SpriteRendererComponent>();
            
            EntityCamera = CurentScene.AddEntity("Test Camera 1");
            EntityCamera?.AddComponent<CameraComponent>();
            transformCamera1 = EntityCamera?.GetComponent<TransformComponent>();
            transformCamera1.Translation = new Vector3(0, 0, -1);
        }

        protected void OnUpdate(float ts)
        {

            Vector3 transform = transformCamera1.Translation;
            Console.WriteLine("TestMain.Update");
            transformCamera1.Translation = new Vector3(0,0, transform.Z + (ts));

            EntityCamera.UpdateEntitiyMatrix();
            
        }

        protected void OnDestroy()
        {
            Console.WriteLine("TestMain.OnDestroy");
            EntityCreate?.DestroyEntity();
            EntityCamera?.DestroyEntity();
           Console.WriteLine("TestMain.OnDestroy");
        }
    }
}