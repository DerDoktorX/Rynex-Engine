using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Rynex;

namespace Rynex
{
    // Curent Scene
    public static class CurentScene
    {
        public static Entity AddEntity(string name)
        {
            InternalCalls.Scene_CreateEntity(name, out ulong entityID);
            if (entityID != 0)
                return new Entity(entityID);
            return null;
        }

        public static Entity GetEntityByName(string name)
        {
            if (InternalCalls.Scene_GetEntityByName(name, out ulong entityID))
                return new Entity(entityID);
            return null;
        }

        public static Entity GetEntityByEntityID(ulong entityID)
        {
            if(InternalCalls.Scene_GetEntityByEntityID(entityID))
                return new Entity(entityID);
            return null;
        }
    }

    
}
