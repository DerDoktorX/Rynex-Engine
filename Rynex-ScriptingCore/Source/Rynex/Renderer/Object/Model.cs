using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rynex;

namespace Rynex
{
    public enum MeshMode
    {
        None= 0,
        Statitic,
        Dynamic
    }
    public class Model : Asset
    {
        public new AssetHandle Handle { get; internal set; }

        public Model(ulong handle)
        {
            Handle = new AssetHandle(handle);
        }
#if false
        ~Model()
        {
            Destroy();
        }
        public void Destroy()
        {

            InternalCalls.Model_Destroy(Handle.UUID);

        }
        #endif

        public static Model GetAsset(string path)
        {
            InternalCalls.AssetManger_GetAsset_Path(path, AssetType.Model, out ulong handle);
            return new Model(handle);
        }
        public static Model GetAsset(ulong handle)
        {
            Console.WriteLine($"C# -> {handle}");
            InternalCalls.AssetManger_GetAsset_Handle(handle, AssetType.Model, out ulong outhandle);
            Console.WriteLine($"C# -> {outhandle}");
            return new Model(outhandle);
        }

    }
}
