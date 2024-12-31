using System;
using System.Dynamic;

namespace Rynex
{
    public enum AssetType
    {
        None = 0,
        Scene,
        Shader,
        Texture,
        Texture2D,
        Value, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4,
        Entity, SceneCamera,
        Framebuffer,
        VertexBuffer, IndexBuffer, StorageBuffer, UniformBuffer,
        VertexArray, StorageArray,
        Mesh, Model
    }

    public struct AssetHandle
    {
        public ulong UUID;
        public AssetHandle(ulong uuid)
        {
            UUID = uuid;
        }

        public static explicit operator ulong(AssetHandle handle)
        {
            return handle.UUID;
        }

        

        // public static explicit operator AssetHandle(ulong uuid)
        // {
        //     return new AssetHandle(uuid);
        // }

    }

    public class Asset
    {
        protected internal Asset() { Handle = new AssetHandle(0uL); }

        internal Asset(AssetHandle assetHandle)
        {
            Handle = assetHandle;
        }

        public Asset(ulong uuid)
        {
            Handle = new AssetHandle(uuid);
        }

       

        public readonly AssetHandle Handle;
        
    }
}
