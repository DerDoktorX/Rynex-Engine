using System;

namespace Rynex
{
    public class VertexArray : Asset
    {
        public new AssetHandle Handle { get; internal set; }

        public enum Primitv
        {
            Nono = 0,
            Traingle, TraingleStrips, TraingleFan,
            Line, LineLoop, LineStrips,
            Points,
            Patches,
        }

        public VertexArray(ulong handle)
        {
            Handle = new AssetHandle();
        }

        public VertexArray()
        {
            InternalCalls.VertexArray_Create(out ulong uuid);
            Handle = new AssetHandle(uuid);
        }

        ~VertexArray()
        {
            Destroy();
        }

        public void Destroy()
        {
            InternalCalls.VertexArray_Destroy(Handle.UUID);
        }

        public static VertexArray GetAsset(string path)
        {
            InternalCalls.AssetManger_GetAsset_Path(path, AssetType.VertexArray, out ulong handle);
            return new VertexArray(handle);
        }
        public static VertexArray GetAsset(ulong handle)
        {
            InternalCalls.AssetManger_GetAsset_Handle(handle, AssetType.VertexArray);
            return new VertexArray(handle);
        }
        public void Bind()
        {
            InternalCalls.VertexArray_Bind(Handle.UUID);
        }

        public void UnBind()
        {
            InternalCalls.VertexArray_UnBind(Handle.UUID);
        }

        public void AddVertexBuffer(VertexBuffer vertexBuffer)
        {
            InternalCalls.VertexArray_AddVertexBuffer(Handle.UUID, vertexBuffer.Handle.UUID);
        }

        public void SetIndexBuffer(IndexBuffer indexBuffer)
        {
            InternalCalls.VertexArray_SetIndexBuffer(Handle.UUID, indexBuffer.Handle.UUID);
        }

        public void SetPrimitv(Primitv primitv)
        {
            InternalCalls.VertexArray_SetPrimitv(Handle.UUID, primitv);
        }

        public Primitv GetPrimitv()
        {
            InternalCalls.VertexArray_GetPrimitv(Handle.UUID, out Primitv primitv);
            return primitv;
        }
    }
}
