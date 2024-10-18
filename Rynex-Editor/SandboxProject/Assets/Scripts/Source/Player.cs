using System;
using System.Reflection;
using Microsoft.Win32.SafeHandles;
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
        public struct Geometry
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
            m_Geometry.SetPrimitv(VertexArray.Primitv.TraingleStrips);
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
            m_Geometry.SetPrimitv(VertexArray.Primitv.TraingleStrips);
        }

        public int sides = 6;
        void Cube3D()
        {
            Geometry[] vertices = new Geometry[sides * 4];
            uint index = 0; 
            uint size = 4; 
            uint grid = size + 1;            
            
            uint size1 = 2;
            uint grid1 = size1 + 1;
#if true
            // up
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Up);
            
            // front
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Front);


            // rigth
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Rigth);
            

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
            vertices[index++] = new Geometry(new Vector3(  0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3( -0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3(  0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3( -0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Back);

#else
            
           
            float sideSize = 0.5f;

            float segmentHeight = sideSize / grid;
            float segmentWidth = sideSize / grid;

            

            float heightHalf = sideSize / 2;
            float widthHalf = sideSize / 2;
            float depthHalf = sideSize / 2;

            Vector3 vector, normale;
            Vector2 uv; 
            for (uint iy = 0, i = 0; iy < grid; iy++)
            {
                float y = iy * segmentHeight - heightHalf;
                for (uint ix = 0; ix < grid; ix++, i++)
                {
                    float x = ix * segmentWidth - widthHalf;
                    vector = new Vector3( x * -1, y * -1, depthHalf);
                    normale = Vector3.Up;
                    uv = new Vector2(ix / size, 1 - (iy / size));
                    vertices[index++] = new Geometry(vector, uv, normale);
                }
            }
#endif
#if true
            uint[] quadMask = new uint[] {
                  0, 1, 2,         1, 3, 2,
            };
              //uint[] indicies = new uint[] {
              //     0, 1, 2,         1, 3, 2,      // Up
              //     4, 5, 6,         5, 7, 6,      // Front
              //     8, 9, 10,      9, 10, 11,    // Rigth
              //     12, 13, 14,    13, 14, 15,   // Left
              //     //16, 17, 18,    17, 18, 19,   // Down
              //     //20, 21, 22,    21, 22, 23,   // Back
              //};
            uint quadsNum = 6;
            uint[] indicies = new uint[quadsNum*6];
            for (uint ix = 0; ix < quadsNum; ix++)
            {
                for (uint iy = 0; iy < 6; iy++)
                {
                    indicies[(ix*6) + iy] = quadMask[iy] + (ix * 4);
                }
            }
#else
            uint[] indicies = new uint[6 * size];

            Console.WriteLine("Befor for index calculater");
            for (uint iy = 0, i = 0; iy < size; iy++)
            {
                for(uint ix = 0; ix < size; ix++, i+=6)
                {
                    uint a = ix + grid1 * iy;
                    uint b = ix + grid1 * (iy + 1);
                    uint c = (ix + 1) + grid1 * (iy + 1);
                    uint d = (ix + 1) + grid1 * iy;

                    indicies[i + 0] = a;
                    indicies[i + 1] = b;
                    indicies[i + 2] = d;

                    indicies[i + 3] = b;
                    indicies[i + 4] = c;
                    indicies[i + 5] = d;
                    Console.WriteLine(a);
                    Console.WriteLine(b);
                    Console.WriteLine(d);

                    Console.WriteLine(b);
                    Console.WriteLine(c);
                    Console.WriteLine(d);
                   // Console.WriteLine(i + 5);
                }
            }
#endif
            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.SetPrimitv(VertexArray.Primitv.Traingle);
            m_Geometry.SetVertex(vertices);
            m_Geometry.SetIndex(indicies);
            Console.WriteLine(indicies.Length);
        }
        void CalculatePlane()
        {

        }

        void Plane3D()
        {
            Geometry[] vertices = new Geometry[sides * 5];
            uint index = 0;

            vertices[index++] = new Geometry(new Vector3( 0.5f, 0.5f,  0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3(-0.5f, 0.5f,  0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3( 0.5f, 0.5f, -0.5f), Vector2.Zero, Vector3.Up);
            vertices[index++] = new Geometry(new Vector3(-0.5f, 0.5f, -0.5f), Vector2.Zero, Vector3.Up);
#if true
            // front
            vertices[index++] = new Geometry(new Vector3( 0.5f, -0.5f, 0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3(-0.5f, -0.5f, 0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3( 0.5f,  0.5f, 0.5f), Vector2.Zero, Vector3.Front);
            vertices[index++] = new Geometry(new Vector3(-0.5f,  0.5f, 0.5f), Vector2.Zero, Vector3.Front);

            // rigth
            vertices[index++] = new Geometry(new Vector3(0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Rigth);
            vertices[index++] = new Geometry(new Vector3(0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Rigth);

            // left
            vertices[index++] = new Geometry(new Vector3(-0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Left);
            vertices[index++] = new Geometry(new Vector3(-0.5f,  0.5f,  0.5f), Vector2.Zero, Vector3.Left);
            vertices[index++] = new Geometry(new Vector3(-0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Left);
            vertices[index++] = new Geometry(new Vector3(-0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Left);

            // Down
            vertices[index++] = new Geometry(new Vector3( 0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Down);
            vertices[index++] = new Geometry(new Vector3(-0.5f, -0.5f,  0.5f), Vector2.Zero, Vector3.Down);
            vertices[index++] = new Geometry(new Vector3( 0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Down);
            vertices[index++] = new Geometry(new Vector3(-0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Down);

            // back
            vertices[index++] = new Geometry(new Vector3( 0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3(-0.5f,  0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3( 0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Back);
            vertices[index++] = new Geometry(new Vector3(-0.5f, -0.5f, -0.5f), Vector2.Zero, Vector3.Back);
#endif
            uint[] indicies = new uint[] {                            
                 //0, 1, 2,       1, 2, 3,      // Up                 
                 0, 1, 2, 3,

                4, 5, 6, 7,
#if true
                 8, 9, 10, 11,
                 12, 13, 14, 15,
                 16, 17, 18, 19,
                 20, 21, 22, 23,
#endif
            };

            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.SetPrimitv(VertexArray.Primitv.Patches);
            m_Geometry.SetVertex(vertices);
            m_Geometry.SetIndex(indicies);
            Console.WriteLine(indicies.Length);
        }

        void Cube3DNewAPI()
        {

            float[] vertices = {
               //   Postion             UV              Normale
              0.5f,  0.5f, -0.5f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
             -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
              0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
             -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
                                    
              0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,
             -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,
              0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,
             -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,
                                    
              0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
              0.5f,  0.5f, -0.5f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
              0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
              0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
                                    
             -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
             -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
             -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
             -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
                                    
              0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
             -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
              0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
             -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
                                    
              0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
             -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
              0.5f,  0.5f, -0.5f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
             -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f
            };

            VertexBuffer vertexBuffer = new VertexBuffer(vertices, 8 * sizeof(float) * 6 * 4, BufferDataUsage.StaticDraw);
            BufferLayout layout = new BufferLayout(new BufferElement[] {
                new BufferElement(ShaderDataType.Float3, "a_Postion"),
                new BufferElement(ShaderDataType.Float2, "a_UV"),
                new BufferElement(ShaderDataType.Float3, "a_Normals")
            });

            vertexBuffer.SetLayout(layout);

            //----------------------------------------------
            uint[] quadMask = new uint[] {
                  0, 1, 2,         1, 3, 2,
            };
            uint quadsNum = 6;
            uint[] indicies = new uint[quadsNum * 6];
            for (uint ix = 0; ix < quadsNum; ix++)
            {
                for (uint iy = 0; iy < 6; iy++)
                {
                    indicies[(ix * 6) + iy] = quadMask[iy] + (ix * 4);
                }
            }
            IndexBuffer indexBuffer = new IndexBuffer(indicies, (uint)indicies.Length, BufferDataUsage.StaticDraw);
            VertexArray vertexArray = new VertexArray();
            vertexArray.AddVertexBuffer(vertexBuffer);
            vertexArray.SetIndexBuffer(indexBuffer);
            vertexArray.SetPrimitv(VertexArray.Primitv.Traingle);
            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.Buffer = vertexBuffer;
            m_Geometry.Geomtry = vertexArray;
            vertexArray.UnBind();
        }

        void Box3D_AABB()
        {
            float[] vertices = {
                -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f,  1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
                -1.0f,  1.0f,  1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,

                -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
                -1.0f,  1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f
            };
            VertexBuffer vertexBuffer = new VertexBuffer(vertices, 8 * sizeof(float) * 2 * 4, BufferDataUsage.StaticDraw);
            BufferLayout layout = new BufferLayout(new BufferElement[] {
                new BufferElement(ShaderDataType.Float3, "a_Postion"),
                new BufferElement(ShaderDataType.Float2, "a_UV"),
                new BufferElement(ShaderDataType.Float3, "a_Normals")
            });

            vertexBuffer.SetLayout(layout);
            uint[] indicies = {
                0, 1,
                1, 2, 
                2, 3,
                3, 0, 
                0, 4,
                4, 5,
                5, 6,
                6, 7,
                7, 4,
                7, 3,
                6, 2,
                5, 1
                // 1, 5, 
                // 6, 2, 6, 7
            };
            
            IndexBuffer indexBuffer = new IndexBuffer(indicies, (uint)indicies.Length, BufferDataUsage.StaticDraw);
            VertexArray vertexArray = new VertexArray();
            vertexArray.AddVertexBuffer(vertexBuffer);
            vertexArray.SetIndexBuffer(indexBuffer);

            vertexArray.SetPrimitv(VertexArray.Primitv.Line);
            m_Geometry = GetComponent<GeometryComponent>();
            m_Geometry.Buffer = vertexBuffer;
            m_Geometry.Geomtry = vertexArray;
            vertexArray.UnBind();
        }

        void OnDraw()
        {
            Console.WriteLine("Player.OnDraw");
            Box3D_AABB();
            //Cube3DNewAPI();
            //Cube3D();

            Console.WriteLine("C# ->  GetComponent Finsihed");
           
            //tex.Destroy();
        }


    }
    
}
