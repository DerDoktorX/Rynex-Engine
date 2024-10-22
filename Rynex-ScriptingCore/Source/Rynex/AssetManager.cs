using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Rynex
{
    public static class AssetManager
    {
#if false
        public static T GetAsset<T>(string path, AssetType type) where T : Asset
        {
            InternalCalls.AssetManger_GetAsset_Path(path, type, out ulong handle);
            T asset = new T(handle);
            return asset;
        }

        public static T GetAsset<T>(string handle, AssetType type) where T : Asset
        {
            InternalCalls.AssetManger_GetAsset_Handle(handle, type);
            return T (handle);
        }
#endif
    }
}
