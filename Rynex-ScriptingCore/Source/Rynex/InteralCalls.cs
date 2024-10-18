using System;
using System.Runtime.CompilerServices;


namespace Rynex
{

    public static class InternalCalls
    {



        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativLog(string text);


        // Exampels
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativLog_Vec3(ref Vector3 prameter, out Vector3 result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float NativLog_Vec3Dot(ref Vector3 prameter);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_CreateEntity(string name, out ulong entityID);

        // Entity

        // Transform
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong uuid, out Vector3 prameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong uuid, ref Vector3 prameter);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong uuid, out Vector3 prameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong uuid, ref Vector3 prameter);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong uuid, out Vector3 prameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong uuid, ref Vector3 prameter);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetChange(ulong uuid, out bool prameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetChange(ulong uuid, ref bool prameter);


        // Geometry

        // old
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetVertex(ulong uuid, IntPtr vertex, uint size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeomtryComponent_SetIndex(ulong uuid, IntPtr index, uint count);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeomtryComponent_SetPrimitv(ulong uuid, int iPrimitv);
        // new
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetBuffer(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_GetBuffer(ulong uuid, out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetGeometry(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_GetGeometry(ulong uuid, out ulong handle);




        // Texture
        // -Create
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Create_Withe_Heigth(out ulong handle, uint withe, uint heigth);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Create_Spec(out ulong handle, ref TextureSpecification spec);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Create_Spec_Data_ByteSize(out ulong handle, ref TextureSpecification spec, IntPtr data, uint byteSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Destroy(ulong handle);
        // -Funktions
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_GetSpecification(ulong handle, out TextureSpecification spec);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_GetWidth(ulong handle, out uint width);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_GetHeight(ulong handle, out uint height);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_GetRenderID(ulong handle, out uint renderID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_SetData(ulong handle, IntPtr data, uint byteSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Resize(ulong handle, uint height, uint width);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Bind(ulong handle, uint slot);



        // Shader
        // -Create
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_Create_Source_Name(out ulong handle, string source, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_Create_Source_Name_VertexSrc_FragmentSrc(out ulong handle, string name, string vertexSrc, string fragmentSrc);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_Destroy(ulong handle);
        // - Funktions
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_ReganrateShader(ulong handle, string source);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_Bind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_UnBind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_SetUniformValue(ulong handle, string name, IntPtr value, ShaderDataType type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_SetInt(ulong handle, string name, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Shader_SetIntArray(ulong handle, string name, int[] value, uint count);


        // VertexBuffer
        // -Create
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Create_Vertices_Size(out ulong handle, IntPtr vertices, uint size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Create_Vertices_Size_Usage(out ulong handle, IntPtr vertices, uint size, BufferDataUsage usage);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Destroy(ulong handle);
        // - Funktions
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Bind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_UnBind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_GetElementFromLayout(ulong handle,
            out uint elementCount,
            uint elementIndex,
            out string bufferElementName,
            out ShaderDataType bufferElementType,
            out uint bufferElementSize,
            out bool bufferElementNormilized,
            out uint bufferElementOffset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_SetBufferElementToLayout(ulong handle,
            uint elementCount,
            uint elementIndex,
            string bufferElementName,
            ShaderDataType bufferElementType,
            bool bufferElementNormilized);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_SetData(ulong handle, IntPtr data, uint size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_GetByteSize(ulong handle, out uint size);


        // IndexBuffer
        // -Create
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_Create_Indices32_Size_Usage(out ulong handle, IntPtr indices, uint size, BufferDataUsage usage);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_Create_Indices16_Size_Usage(out ulong handle, IntPtr indices, uint size, BufferDataUsage usage);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_Destroy(ulong handle);
        // - Funktions
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_Bind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_UnBind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_SetData32(ulong handle, IntPtr indices, uint count);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_SetData16(ulong handle, IntPtr indices, uint count);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_GetCount(ulong handle, out uint count);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void IndexBuffer_GetElementByte(ulong handle, out uint elementByte);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_Create(out ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_Destroy(ulong handle);
        // - Funktions
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_Bind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_UnBind(ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_AddVertexBuffer(ulong handle, ulong handleVertex);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_SetIndexBuffer(ulong handle, ulong handleIndex);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_SetPrimitv(ulong handle, VertexArray.Primitv primitv);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexArray_GetPrimitv(ulong handle, out VertexArray.Primitv primitv);

        // Componets
#if false

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType );
#endif


        // Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);
    }
}
