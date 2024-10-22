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


        #region Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType );
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_RemoveComponent(ulong entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_CreateChilde(ulong parent, string name, out ulong children);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_UpdateMatrixe(ulong parent);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_Dealete(ulong parent);

        #endregion



        #region TransformComponent

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

        #endregion

        #region GeometryComponent

        #region Old

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetVertex(ulong uuid, IntPtr vertex, uint size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeomtryComponent_SetIndex(ulong uuid, IntPtr index, uint count);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeomtryComponent_SetPrimitv(ulong uuid, int iPrimitv);

        #endregion

        #region New

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetBuffer(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_GetBuffer(ulong uuid, out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetGeometry(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_GetGeometry(ulong uuid, out ulong handle);

        #endregion

        #endregion

        #region TagComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_SetTag(ulong uuid, ref string tag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_GetTag(ulong uuid, out string tag);
        #endregion

        #region SpriteRendererComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColor(ulong uuid, ref Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColor(ulong uuid, out Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTexture(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetTexture(ulong uuid, out ulong handle);

        #endregion

        #region CameraComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetPrimary(ulong uuid, ref bool primary);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetPrimary(ulong uuid, out bool primary);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetFixedAspectRotaion(ulong uuid, ref bool fixedAspectRotaion);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_GetFixedAspectRotaion(ulong uuid, out bool fixedAspectRotaion);
        #endregion

        #region ScriptComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptComponent_SetName(ulong uuid, ref string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptComponent_GetName(ulong uuid, out string name);

        #endregion

        #region MaterialComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_SetShader(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_GetShader(ulong uuid, out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_GetColor(ulong uuid, out Vector3 color);

        #endregion

        #region Matrix4x4Component

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Matrix4x4Component_SetMatrix4x4(ulong uuid, ref Matrix4x4 matrix);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Matrix4x4Component_GetMatrix4x4(ulong uuid, out Matrix4x4 matrix);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Matrix4x4Component_SetGlobleMatrix4x4(ulong uuid, ref Matrix4x4 matrix);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Matrix4x4Component_GetGlobleMatrix4x4(ulong uuid, out Matrix4x4 matrix);

        #endregion

        #region FrameBufferComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void FrameBufferComponent_SetFrameBuffer(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void FrameBufferComponent_GetFrameBuffer(ulong uuid, out ulong handle);

        #endregion

        #region MeshComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshComponent_SetModelR(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshComponent_GetModelR(ulong uuid, out ulong handle);

        #endregion

        #region AmbientLigthComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_GetColor(ulong uuid, out Vector3 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_SeIntensitie(ulong uuid, ref float intensitie);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_GetIntensitie(ulong uuid, out float intensitie);

        #endregion

        #region DrirektionleLigthComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DrirektionleLigthComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DrirektionleLigthComponent_GetColor(ulong uuid, out Vector3 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DrirektionleLigthComponent_SetIntensitie(ulong uuid, ref float intensitie);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void DrirektionleLigthComponent_GetIntensitie(ulong uuid, out float intensitie);

        #endregion

        #region PointLigthComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointLigthComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointLigthComponent_GetColor(ulong uuid, out Vector3 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointLigthComponent_SeIntensitie(ulong uuid, ref float intensitie);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointLigthComponent_GetIntensitie(ulong uuid, out float intensitie);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointLigthComponent_SetDistence(ulong uuid, ref float distence);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PointLigthComponent_GetDistence(ulong uuid, out float distence);

        #endregion

        #region SpotLigthComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_GetColor(ulong uuid, out Vector3 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_SeIntensitie(ulong uuid, ref float intensitie);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_GetIntensitie(ulong uuid, out float intensitie);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_SetDistence(ulong uuid, ref float distence);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_GetDistence(ulong uuid, out float distence);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_SetOuter(ulong uuid, ref float outer);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_GetOuter(ulong uuid, out float outer);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_SetInner(ulong uuid, ref float inner);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpotLigthComponent_GetInner(ulong uuid, out float inner);

        #endregion

        #region ParticelComponente
        #endregion

        #region PostProcessingComponent
        #endregion



        #region TextureClass

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Create_Withe_Heigth(out ulong handle, uint withe, uint heigth);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Create_Spec(out ulong handle, ref TextureSpecification spec);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Create_Spec_Data_ByteSize(out ulong handle, ref TextureSpecification spec, IntPtr data, uint byteSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Texture_Destroy(ulong handle);
        
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

        #endregion

        #region ShaderClass

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

        #endregion

        #region VertexBufferClass

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Create_Vertices_Size(out ulong handle, IntPtr vertices, uint size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Create_Vertices_Size_Usage(out ulong handle, IntPtr vertices, uint size, BufferDataUsage usage);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VertexBuffer_Destroy(ulong handle);

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

        #endregion

        #region IndexBufferClass

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

        #endregion

        # region VertexArrayClass

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

        #endregion

        #region SceneClass

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_CreateEntity(string entityName, out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Scene_GetEntityByName(string entityName, out ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Scene_GetEntityByEntityID(ulong handle);
        

        #endregion

        #region FramebufferClass

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_Create_Spec(out ulong handle, IntPtr attachments, uint size, uint width, uint height, uint Samples, bool SwapChainTarget);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_Destroy(ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_GetFramebufferSpecification(ulong handle, out uint size, out uint width, out uint height, out uint Samples, out bool SwapChainTarget);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_GetFramebufferTextureSpecification(ulong handle, uint index, out FramebufferTextureSpecification attachment);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_Resize(ulong handle, uint withe, uint heigth);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_ReadPixel(ulong handle, uint attachmentsIndex, int x, int y, out int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_GetFrambufferSize(ulong handle, out Vector2 size);

        #endregion

        #region AssetManagerClass

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AssetManger_GetAsset_Path(string path, AssetType type, out ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AssetManger_GetAsset_Handle(ulong handle, AssetType type);

        #endregion

        #region Model
#if false
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Model_Create(out ulong handle);
#endif

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Model_Destroy(ulong handle);

        #endregion


        #region Event

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        #endregion
    }
}
