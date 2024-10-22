using Rynex;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    class PlayerTest2 : Entity
    {
        private TransformComponent m_transform;

        void OnCreate()
        {
            Console.WriteLine("PlayerTest2.OnCreate");
            if(HasComponent<TransformComponent>())
                m_transform = GetComponent<TransformComponent>();

            Console.WriteLine("C# -> GetComponent Finsihed");

        }

        void OnDestroy()
        {
            Console.WriteLine("PlayerTest2.OnDestroy");
        }

        void OnUpdate(float ts)
        {
            Console.WriteLine("PlayerTest2.OnUpdate");
        }

        void OnDraw()
        {
            Console.WriteLine("PlayerTest2.OnDraw");
        }
    }
}
