using System;
using Rynex;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography;

namespace Rynex
{

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

        public readonly AssetHandle Handle;
    }
}
