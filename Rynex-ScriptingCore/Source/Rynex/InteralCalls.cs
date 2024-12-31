using System;
using System.Numerics;
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
        internal extern static void TagComponent_SetTag(ulong uuid, string tag);

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

        #region SceneCamer

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetViewPortSize(ulong uuid, uint withe, uint heigth);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetOrthoGrafic(ulong uuid, float sizen, float nearClip, float farClip);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetPerspectiv(ulong uuid, float verticleFow, float nearClip, float farClip);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetOrthograficSize(ulong uuid, float size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetOrthograficNearClipe(ulong uuid, float size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetOrthograficFarClipe(ulong uuid, float size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CameraComponent_Camera_GetOrthographicSize(ulong uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CameraComponent_Camera_GetOrthographicNearClipe(ulong uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CameraComponent_Camera_GetOrthographicFarClipe(ulong uuid);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetPerspectivVerticleFOV(ulong uuid, float verticleVow);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetPerspectivNearClipe(ulong uuid, float nearClip);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetPerspectivFarClipe(ulong uuid, float farClipe);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CameraComponent_Camera_GetPerspectivVerticleFOV(ulong uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CameraComponent_Camera_GetPerspectivNearClipe(ulong uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CameraComponent_Camera_GetPerspectivFarClipe(ulong uuid);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static SceneCamera.ProjectionType CameraComponent_Camera_GetProjectionType(ulong uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetProjectionType(ulong uuid, SceneCamera.ProjectionType type);

        #endregion

        #region Camera

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Matrix4x4 CameraComponent_Camera_GetProjektion(ulong uuid);

#if false
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_Camera_SetProjektion(ulong uuid, ref Matrix4x4 matrix);
#endif

#endregion

#endregion

        #region ScriptComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptComponent_SetName(ulong uuid, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptComponent_GetName(ulong uuid, out string name);

        #endregion

#region MaterialComponent

#if false
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_SetShader(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_GetShader(ulong uuid, out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MaterialComponent_GetColor(ulong uuid, out Vector3 color);
#endif

#endregion

        #region Matrix4x4Component

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Matrix4x4Component_SetMatrix4x4(ulong uuid, ref Matrix4x4 matrix);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Matrix4x4 Matrix4x4Component_GetMatrix4x4(ulong uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Matrix4x4Component_SetGlobleMatrix4x4(ulong uuid, ref Matrix4x4 matrix);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Matrix4x4 Matrix4x4Component_GetGlobleMatrix4x4(ulong uuid);

        #endregion

        #region FrameBufferComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void FrameBufferComponent_SetFrameBuffer(ulong uuid, ref ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void FrameBufferComponent_GetFrameBuffer(ulong uuid, out ulong handle);

        #endregion

        #region MeshComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshComponent_SetModelR(ulong uuid, ulong handle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshComponent_GetModelR(ulong uuid, out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void MeshComponent_SetMeshMode(ulong uuid, MeshMode meshMode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static MeshMode MeshComponent_GetMeshMode(ulong uuid);

        #endregion

        #region AmbientLigthComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_SetColor(ulong uuid, ref Vector3 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_GetColor(ulong uuid, out Vector3 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AmbientLigthComponent_SetIntensitie(ulong uuid, ref float intensitie);
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
        internal extern static void PointLigthComponent_SetIntensitie(ulong uuid, ref float intensitie);
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
        internal extern static void SpotLigthComponent_SetIntensitie(ulong uuid, ref float intensitie);
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

        #region VertexArrayClass

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
        internal extern static void Framebuffer_Create_Spec(out ulong handle, IntPtr attachments, uint size, uint width, uint height, bool SwapChainTarget);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_Destroy(ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Framebuffer_GetFramebufferSpecification(ulong handle, out uint size, out uint width, out uint height, out bool SwapChainTarget);
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
        internal extern static void AssetManger_GetAsset_Handle(ulong handle, AssetType type, out ulong outhandle);

        #endregion
        
        #region Model
#if true
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Model_Create(out ulong handle);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Model_Destroy(ulong handle);
#endif
#endregion

        #region AplicationClass

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Vector2 Application_GetWindowSize();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Vector2 Application_GetMousePixelPosition();

        #endregion

        #region Event

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseOnViewPort();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsWindowResize();

        #endregion
    }
}
