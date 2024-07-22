using System;
using Rynex;

namespace Sandbox
{
    public class Player1 : Entity
    {
        private TransformComponent m_transform;
        private GeometryComponent m_Geometry;

        void OnCreate()
        {
            Console.WriteLine("Player1.OnCreate");

            m_transform = GetComponent<TransformComponent>();
            Console.WriteLine("C# -> GetComponent Finsihed");

        }
        void OnDestroy()
        {
            Console.WriteLine("Player1.OnDestroy");
        }

        void OnUpdate(float ts)
        {
            //Console.WriteLine($"Player.OnUpdate: {ts}");
            Vector3 translation = Translation;
            Vector3 speed = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.D))
                speed.X += 1f;
            if (Input.IsKeyDown(KeyCode.A))
                speed.X -= 1f;
            if (Input.IsKeyDown(KeyCode.W))
                speed.Y += 1f;
            if (Input.IsKeyDown(KeyCode.S))
                speed.Y -= 1f;

            translation += speed * ts;
            Translation = translation;
        }
        struct Geometry
        {
            public Vector3 Position;
            public Vector2 UV;
            public Vector3 Normal;

            public Geometry(Vector3 position, Vector2 uv, Vector3 normal)
            {
                Position = position;
                UV = uv;
                Normal = normal;
            }

            public Geometry(float[] vertex)
            {
                Position = new Vector3(vertex[0], vertex[1], vertex[2]);
                UV = new Vector2(vertex[3], vertex[4]);
                Normal = new Vector3(vertex[5], vertex[6], vertex[7]);
            }
        }
        void OnDraw()
        {
            Console.WriteLine("Player1.OnDraw");
            Geometry[] geometry = new Geometry[8];
            geometry[0] = new Geometry( new Vector3( -0.5f, -0.5f,  0.5f  ), new Vector2(0.0f, 0.0f), new Vector3( 0.75f, 0.25f, 0.25f) );
            geometry[1] = new Geometry( new Vector3(  0.5f, -0.5f,  0.5f  ), new Vector2(0.0f, 0.0f), new Vector3( 1.0f, 0.0f, 0.0f) );
            geometry[2] = new Geometry( new Vector3( -0.5f,  0.5f,  0.5f  ), new Vector2(0.0f, 0.0f), new Vector3( 0.0f, 1.0f, 0.0f) );
            geometry[3] = new Geometry( new Vector3(  0.5f,  0.5f,  0.5f  ), new Vector2(0.0f, 0.0f), new Vector3( 0.25f, 0.25f, 0.75f) );

            geometry[4] = new Geometry( new Vector3( -0.5f,  0.5f, -0.5f  ), new Vector2(0.0f, 1.0f), new Vector3( 0.0f, 0.0f, 1.0f) );
            geometry[5] = new Geometry( new Vector3(  0.5f,  0.5f, -0.5f  ), new Vector2(0.0f, 1.0f), new Vector3( 1.0f, 1.0f, 0.0f) );
            geometry[6] = new Geometry( new Vector3( -0.5f, -0.5f, -0.5f  ), new Vector2(0.0f, 1.0f), new Vector3( 0.0f, 1.0f, 1.0f) );
            geometry[7] = new Geometry( new Vector3(  0.5f, -0.5f, -0.5f  ), new Vector2(0.0f, 1.0f), new Vector3( 1.0f, 0.0f, 1.0f) );
            
            

            uint[] index = new uint[] { 
                0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 1, 7, 3, 5, 4, 4, 2, 6, 0
            };


            


            m_Geometry = GetComponent<GeometryComponent>();
            Console.WriteLine("C# ->  GetComponent Finsihed");
            m_Geometry.SetVertex(geometry);
            m_Geometry.SetIndex(index);

        }


    }

    
}
