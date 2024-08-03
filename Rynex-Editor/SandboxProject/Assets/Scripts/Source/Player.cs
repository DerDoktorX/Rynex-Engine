using System;
using System.Reflection;
using Rynex;

namespace Sandbox
{
    public class Player : Entity
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
        void Cube3DStrips()
        {
            Geometry[] geometry = new Geometry[18];
            // up
            geometry[0] = new Geometry(new Vector3(-0.5f, 0.5f, -0.5f), new Vector2(0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f));
            geometry[1] = new Geometry(new Vector3(0.5f, 0.5f, -0.5f), new Vector2(0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f));
            geometry[2] = new Geometry(new Vector3(-0.5f, 0.5f, 0.5f), new Vector2(0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f));
            geometry[3] = new Geometry(new Vector3(0.5f, 0.5f, 0.5f), new Vector2(0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f));
            // front                         
            geometry[4] = new Geometry(new Vector3(-0.5f, -0.5f, 0.5f), new Vector2(0.0f, 0.0f), new Vector3(0.0f, 0.0f, 1.0f));
            geometry[5] = new Geometry(new Vector3(0.5f, -0.5f, 0.5f), new Vector2(0.0f, 0.0f), new Vector3(0.0f, 0.0f, 1.0f));

            // down
            geometry[6] = new Geometry(new Vector3(-0.5f, -0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(0.0f, -1.0f, 0.0f));
            geometry[7] = new Geometry(new Vector3(0.5f, -0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(0.0f, -1.0f, 0.0f));

            geometry[8] = new Geometry(new Vector3(-0.5f, 0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(0.0f, 0.0f, -1.0f));
            geometry[9] = new Geometry(new Vector3(0.5f, 0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(0.0f, 0.0f, -1.0f));

            geometry[10] = new Geometry(new Vector3(0.5f, 0.5f, 0.5f), new Vector2(0.0f, 1.0f), new Vector3(1.0f, 0.0f, 0.0f));
            geometry[11] = new Geometry(new Vector3(0.5f, -0.5f, 0.5f), new Vector2(0.0f, 1.0f), new Vector3(1.0f, 0.0f, 0.0f));
            geometry[12] = new Geometry(new Vector3(0.5f, 0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(1.0f, 0.0f, 0.0f));
            geometry[13] = new Geometry(new Vector3(0.5f, -0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(1.0f, 0.0f, 0.0f));

            geometry[14] = new Geometry(new Vector3(-0.5f, 0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(-1.0f, 0.0f, 0.0f));
            geometry[15] = new Geometry(new Vector3(-0.5f, 0.5f, 0.5f), new Vector2(0.0f, 1.0f), new Vector3(-1.0f, 0.0f, 0.0f));
            geometry[16] = new Geometry(new Vector3(-0.5f, -0.5f, -0.5f), new Vector2(0.0f, 1.0f), new Vector3(-1.0f, 0.0f, 0.0f));
            geometry[17] = new Geometry(new Vector3(-0.5f, -0.5f, 0.5f), new Vector2(0.0f, 1.0f), new Vector3(-1.0f, 0.0f, 0.0f));

            uint[] index = new uint[] {
                0, 1, 2, 3, 4, 5, 6, 7, 8 , 9, 9, 10, 10, 11, 12, 13, 14, 14, 15, 16, 17
            };





            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.SetPrimitv(GeometryComponent.Primitv.TraingleStrips);
            m_Geometry.SetVertex(geometry);
            m_Geometry.SetIndex(index);
        }

        void Sphere3D(float radius = 1f, float widthSegments = 32, float heightSegments = 16, float phiStart = 0, float phiLength = (float)Math.PI * 2, float thetaStart = 0, float thetaLength = (float)Math.PI)
        {

            widthSegments = Math.Max(3.0f, (float)Math.Floor(widthSegments));
            heightSegments = Math.Max(2.0f, (float)Math.Floor(heightSegments));

            uint index = 0;
            uint[][] grid = new uint[(int)heightSegments+1][];
            uint[] verticesRow = new uint[(int)widthSegments+1];


            float thetaEnd = Math.Min(thetaStart + thetaLength, (float)Math.PI);

            Geometry[] geometry = new Geometry[160];
           
            for (int iy = 0, i = 0; iy <= heightSegments; iy++)
            {
                float v = iy / heightSegments;

                float uOffset = 0.0f;
 
                if (iy == 0.0f && thetaStart == 0.0f)
                {

                    uOffset = 0.5f / widthSegments;

                }
                else if (iy == heightSegments && thetaEnd == Math.PI)
                {

                    uOffset = -0.5f / widthSegments;

                }
                //Console.WriteLine("widthSegments:");
                //Console.WriteLine(widthSegments);
                for (int ix = 0; ix <= widthSegments; ix++, i++)
                {
                    float u = ix / widthSegments;
  
                    geometry[i] = new Geometry(
                        new Vector3(
                            -radius * (float)Math.Cos(phiStart + u * phiLength) * (float)Math.Sin(thetaStart + v * thetaLength),
                            radius * (float)Math.Cos(thetaStart + v * thetaLength),
                            radius * (float)Math.Sin(phiStart + u * phiLength) * (float)Math.Sin(thetaStart + v * thetaLength)
                        ),
                        new Vector2(0.0f, 0.0f),
                        new Vector3(0.0f, 0.0f, 0.0f)
                        );
                    geometry[i].Normal = new Vector3(geometry[i].Position.Normalize());
                   
                    verticesRow[ix] = index++;
                  
                }
                grid[iy] = verticesRow;
                

            }

            uint[] indices = new uint[672];
            uint i_indices = 0;
            for (int iy = 0; iy < heightSegments; iy++)
            {
                for (int ix = 0; ix < widthSegments; ix++)
                {
                    uint a = grid[iy][ix + 1];
                    uint b = grid[iy][ix];
                    uint c = grid[iy + 1][ix];
                    uint d = grid[iy + 1][ix + 1];

                    if (iy != 0 || thetaStart > 0) 
                    {
                        indices[i_indices] = a;
                        indices[i_indices + 1] = b;
                        indices[i_indices + 2] = d;
                        i_indices += 3;
                        Console.WriteLine(i_indices);
                    }
                    if (iy != heightSegments - 1 || thetaEnd < Math.PI) 
                    {
                        indices[i_indices] = b;
                        indices[i_indices + 1] = c;
                        indices[i_indices + 2] = d;
                        i_indices += 3;
                        Console.WriteLine(i_indices);
                    }                             
                                                  

                }

            }
            
            m_Geometry = GetComponent<GeometryComponent>();
            
            m_Geometry.SetVertex(geometry);
            m_Geometry.SetIndex(indices);
            m_Geometry.SetPrimitv(GeometryComponent.Primitv.TraingleStrips);
        }

        public int sides = 6;
        void Cube3D()
        {
            Geometry[] vertices = new Geometry[sides * 4];
            uint index = 0;

            // up
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Up);
            
            // front
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Front);

            // rigth
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Rigth);

            // left
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Left);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Left);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Left);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Left);

            // Down
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Down);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Down);
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Down);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Down);
            
            // back
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            Console.WriteLine(index);
            uint[] indicies = new uint[] {
                 0, 1, 2,       1, 2, 3,      // Up
                 4, 5, 6,       5, 6, 7,      // Front
                 8, 9, 10,      9, 10, 11,    // Rigth
                 12, 13, 14,    13, 14, 15,   // Left
                 16, 17, 18,    17, 18, 19,   // Down
                 20, 21, 22,    21, 22, 23,   // Back
            };
           
            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.SetPrimitv(GeometryComponent.Primitv.Patches);
            m_Geometry.SetVertex(vertices);
            m_Geometry.SetIndex(indicies);
            Console.WriteLine(indicies.Length);
        }

        void Plane3D()
        {
            Geometry[] vertices = new Geometry[sides * 1];
            uint index = 0;

            vertices[index++] = new Geometry(new Vector3( 0.5f, 0.0f,  0.5f), new Vector2(1.0f, 1.0f), Vector3.Up);
            vertices[index++] = new Geometry(new Vector3(-0.5f, 0.0f,  0.5f), new Vector2(1.0f, 0.0f), Vector3.Up);
            vertices[index++] = new Geometry(new Vector3( 0.5f, 0.0f, -0.5f), new Vector2(0.0f, 1.0f), Vector3.Up);
            vertices[index++] = new Geometry(new Vector3(-0.5f, 0.0f, -0.5f), new Vector2(0.0f, 0.0f), Vector3.Up);
            uint[] indicies = new uint[] {
                 //0, 1, 2,       1, 2, 3,      // Up
                 3, 2, 0, 1,
            };

            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.SetPrimitv(GeometryComponent.Primitv.Patches);
            m_Geometry.SetVertex(vertices);
            m_Geometry.SetIndex(indicies);
            Console.WriteLine(indicies.Length);
        }
        

        void OnDraw()
        {
            Console.WriteLine("Player.OnDraw");
            // Cube3D();
            // Sphere3D();
            Plane3D();
            Console.WriteLine("C# ->  GetComponent Finsihed");
        }


    }
    
}
