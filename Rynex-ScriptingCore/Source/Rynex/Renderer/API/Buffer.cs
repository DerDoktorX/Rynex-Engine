using System;

namespace Rynex
{
    public enum ShaderDataType
    {
        None = 0,
        Float, Float2, Float3, Float4, Float3x3, Float4x4, FloatArray,
        Int, Int2, Int3, Int4, Int3x3, Int4x4, IntArray,
        Uint, Uint2, Uint3, Uint4, Uint3x3, Uint4x4, UintArray,
        Bool,
        Struct,
        Texture,
    };

    public enum BufferDataUsage
    {
        None = 0,
        DynamicDraw,
        StaticDraw,
    };

    public struct BufferElement
    {
        public string Name;
        public ShaderDataType Type;
        public uint Offset;
        public uint Size;
        public bool Normilized;


        public BufferElement(ShaderDataType type, string name, bool normilized = false)
        {
            Name = name;
            Type = type;
            Normilized = normilized;
            Offset = 0;
            Size = 0;
        }


        public uint GetCompontsCount()
        {
            switch (Type)
            {
                case ShaderDataType.Float: return 1;
                case ShaderDataType.Float2: return 2;
                case ShaderDataType.Float3: return 3;
                case ShaderDataType.Float4: return 4;
                case ShaderDataType.Float3x3: return 3 * 3;
                case ShaderDataType.Float4x4: return 4 * 4;
                //caseShaderDataType.FloatArray:	 return 4;
                case ShaderDataType.Int: return 1;
                case ShaderDataType.Int2: return 2;
                case ShaderDataType.Int3: return 3;
                case ShaderDataType.Int4: return 4;
                case ShaderDataType.Int3x3: return 3 * 3;
                case ShaderDataType.Int4x4: return 4 * 4;
                //case ShaderDataType.IntArray:		return 4;
                //case ShaderDataType.Uint:			return 4;
                //case ShaderDataType.Uint2:		return 4;
                //case ShaderDataType.Uint3:		return 4;
                //case ShaderDataType.Uint4:		return 4;
                //case ShaderDataType.Uint3x3:		return 4;
                //case ShaderDataType.Uint4x4:		return 4;
                //case ShaderDataType.UintArray		return 4;
                case ShaderDataType.Bool: return 1;
                    //case ShaderDataType.Struct:		return 4;
            }
            return 0;
        }


    };

    public struct BufferLayout
    {
        private BufferElement[] m_Elements;
        private uint m_Stride;
        private uint m_Length;
        private uint m_BufferCount;
        public BufferLayout(BufferElement[] element)
        {
            m_Stride = 0;
            m_Length = 0;
            m_BufferCount = 0;
            m_Elements = element;
            CaculateOffsetAndStride();
        }

        private void CaculateOffsetAndStride()
        {

            m_Stride = 0;
            m_Length = 0;
            m_BufferCount = 0;

            int length = m_Elements.Length;
            uint offset = 0;

            for (int i = 0; i < length; i++)
            {
                m_Elements[i].Offset = offset;
                offset += m_Elements[i].Size;
                m_Stride += m_Elements[i].Size;
                m_Length++;
                m_BufferCount += m_Elements[i].GetCompontsCount();
            }
        }

        public uint GetLength()
        {
            return m_Length;
        }

        public BufferElement[] GetElements()
        {
            return m_Elements;
        }
    }


    public class VertexBuffer : Asset
    {
        public new AssetHandle Handle { get; internal set; }

        public VertexBuffer(ulong handle)
        {
            Handle = new AssetHandle();
        }

        public VertexBuffer(uint size)
        {

        }

        public VertexBuffer(byte[] vertices, uint size)
        {
            unsafe
            {
                fixed (byte* verticesPtr = vertices)
                {
                    InternalCalls.VertexBuffer_Create_Vertices_Size(out ulong uuid, (IntPtr)verticesPtr, size);
                    Handle = new AssetHandle(uuid);

                }
            }

        }

        public VertexBuffer(float[] vertices, uint size, BufferDataUsage usage)
        {
            unsafe
            {
                fixed (float* verticesPtr = vertices)
                {
                    InternalCalls.VertexBuffer_Create_Vertices_Size_Usage(out ulong uuid, (IntPtr)verticesPtr, size, usage);
                    Handle = new AssetHandle(uuid);

                }
            }
        }

        ~VertexBuffer()
        {
            InternalCalls.VertexBuffer_Destroy(Handle.UUID);
        }

        public void Destroy()
        {
            InternalCalls.VertexBuffer_Destroy(Handle.UUID);
        }

        public static VertexBuffer GetAsset(string path)
        {
            InternalCalls.AssetManger_GetAsset_Path(path, AssetType.VertexBuffer, out ulong handle);
            return new VertexBuffer(handle);
        }
        public static VertexBuffer GetAsset(ulong handle)
        {
            InternalCalls.AssetManger_GetAsset_Handle(handle, AssetType.VertexBuffer, out ulong outHandle);
            return new VertexBuffer(outHandle);
        }

        public void SetLayout(BufferLayout layout)
        {
            BufferElement[] bufferElements = layout.GetElements();
            uint size = (uint)bufferElements.Length;
            for (uint i = 0; i < size; i++)
            {
                InternalCalls.VertexBuffer_SetBufferElementToLayout(Handle.UUID,
                    size,
                    i,
                    bufferElements[i].Name,
                    bufferElements[i].Type,
                    bufferElements[i].Normilized
                    );
            }
        }
       
        public BufferLayout GetLayout()
        {
            InternalCalls.VertexBuffer_GetElementFromLayout(Handle.UUID,
                out uint size,
                0,
                out string bufferElementName,
                out ShaderDataType bufferElementType,
                out uint bufferElementSize,
                out bool bufferElementNormilized,
                out uint bufferElementOffset);
            BufferElement[] bufferElements = new BufferElement[size];
            for (uint i = 0; i < size; i++)
            {
                InternalCalls.VertexBuffer_GetElementFromLayout(Handle.UUID,
                out uint count,
                i,
                out string bE_Name,
                out ShaderDataType bE_Type,
                out uint bE_Size,
                out bool bE_Normilized,
                out uint bE_Offset);

                bufferElements[i].Name = bE_Name;
                bufferElements[i].Type = bE_Type;
                bufferElements[i].Size = bE_Size;
                bufferElements[i].Normilized = bE_Normilized;
                bufferElements[i].Offset = bE_Offset;
            }

            return new BufferLayout(bufferElements);
        }

        public void Bind()
        {
            InternalCalls.VertexBuffer_Bind(Handle.UUID);
        }

        public void UnBind()
        {
            InternalCalls.VertexBuffer_UnBind(Handle.UUID);
        }

        public void SetData(byte[] data, uint byteSize)
        {
            unsafe
            {
                fixed (byte* dataPointer = data)
                    InternalCalls.VertexBuffer_SetData(Handle.UUID, (IntPtr)dataPointer, byteSize);
            }
        }

        public uint GetByteSize()
        {
            InternalCalls.VertexBuffer_GetByteSize(Handle.UUID, out uint size);
            return size;

        }
    }

    public class IndexBuffer : Asset
    {
        public new AssetHandle Handle { get; internal set; }

        public IndexBuffer(uint[] indices, uint size, BufferDataUsage usage = BufferDataUsage.None)
        {
            unsafe
            {
                fixed (uint* indicesPtr = indices)
                {
                    InternalCalls.IndexBuffer_Create_Indices32_Size_Usage(out ulong uuid, (IntPtr)indicesPtr, size, usage);
                    Handle = new AssetHandle(uuid);
                }
            }
        }

        public IndexBuffer(UInt16[] indices, uint size, BufferDataUsage usage = BufferDataUsage.None)
        {
            unsafe
            {
                fixed (UInt16* indicesPtr = indices)
                {
                    InternalCalls.IndexBuffer_Create_Indices16_Size_Usage(out ulong uuid, (IntPtr)indicesPtr, size, usage);
                    Handle = new AssetHandle(uuid);
                }
            }
        }

        ~IndexBuffer()
        {
            InternalCalls.IndexBuffer_Destroy(Handle.UUID);
        }
        public void Bind()
        {
            InternalCalls.IndexBuffer_Bind(Handle.UUID);
        }

        public void UnBind()
        {
            InternalCalls.IndexBuffer_UnBind(Handle.UUID);
        }

        public void SetData(uint[] indices, uint count)
        {
            unsafe
            {
                fixed (uint* indicesPtr = indices)
                {
                    InternalCalls.IndexBuffer_SetData32(Handle.UUID, (IntPtr)indicesPtr, count);
                }
            }
        }

        public void SetData(UInt16[] indices, uint count)
        {
            unsafe
            {
                fixed (UInt16* indicesPtr = indices)
                {
                    InternalCalls.IndexBuffer_SetData16(Handle.UUID, (IntPtr)indicesPtr, count);
                }
            }
        }

        public uint GetCount()
        {
            InternalCalls.IndexBuffer_GetCount(Handle.UUID, out uint count);
            return count;

        }
    }
}
