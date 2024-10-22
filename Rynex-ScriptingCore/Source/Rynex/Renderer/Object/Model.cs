using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rynex;

namespace Rynex
{
    public class Model : Asset
    {
        public new AssetHandle Handle { get; internal set; }

        public Model(ulong handle)
        {
            Handle = new AssetHandle();
        }

        ~Model()
        {
            Destroy();
        }
        public void Destroy()
        {
            InternalCalls.Model_Destroy(Handle.UUID);
        }

    }
}
