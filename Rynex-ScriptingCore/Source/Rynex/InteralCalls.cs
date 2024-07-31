using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;


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


        // Entity

        // Transform
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TransformComponent_GetTranslation(ulong uuid, out Vector3 prameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TransformComponent_SetTranslation(ulong uuid, ref Vector3 prameter);


        // Geometry
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeometryComponent_SetVertex(ulong uuid, IntPtr vertex, uint size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeomtryComponent_SetIndex(ulong uuid, IntPtr index, uint count);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void GeomtryComponent_SetPrimitv(ulong uuid, int iPrimitv);
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
