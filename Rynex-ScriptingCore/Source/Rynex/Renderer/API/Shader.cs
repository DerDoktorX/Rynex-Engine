using System;

namespace Rynex
{
    public class Shader : Asset
    {
        public enum Type
        {
            None = 0,
            Fragment = (1 << 0),
            Vertex = (1 << 1),
            Compute = (1 << 2),
            Geometry = (1 << 3),
            TeselationControl = (1 << 4),
            TeselationEvelution = (1 << 5)
        }
        public new AssetHandle Handle { get; internal set; }

        public Shader(ulong handle)
        {
            Handle = new AssetHandle(handle);
        }

        public Shader(string source, string name)
        {
            InternalCalls.Shader_Create_Source_Name(out ulong uuid, source, name);
            Handle = new AssetHandle(uuid);
        }

        public Shader(string name, string vertexSrc, string fragmentSrc)
        {
            InternalCalls.Shader_Create_Source_Name_VertexSrc_FragmentSrc(out ulong uuid, name, vertexSrc, fragmentSrc);
            Handle = new AssetHandle(uuid);
        }

        ~Shader()
        {
            Destroy();
        }
        public void Destroy()
        {
            InternalCalls.Shader_Destroy(Handle.UUID);
        }

        public void ReganrateShader(string source)
        {
            InternalCalls.Shader_ReganrateShader(Handle.UUID, source);
        }

        public void Bind()
        {
            InternalCalls.Shader_Bind(Handle.UUID);
        }

        public void UnBind()
        {
            InternalCalls.Shader_UnBind(Handle.UUID);
        }

        void AddShader(string shader, Type shaderType)
        {
            // InternalCalls.Shader_AddShader(Handle.UUID, shader, shaderType);
        }

        void SetUniformValue(string name, byte[] value, ShaderDataType type)
        {
            unsafe
            {
                fixed (byte* valuePtr = value)
                    InternalCalls.Shader_SetUniformValue(Handle.UUID, name, (IntPtr)valuePtr, type);

            }
        }

        void SetInt(string name, int value)
        {
            InternalCalls.Shader_SetInt(Handle.UUID, name, value);
        }

        void SetIntArray(string name, int[] value, uint count)
        {
            InternalCalls.Shader_SetIntArray(Handle.UUID, name, value, count);
        }

        void SetFloat3(string name, Vector3 value)
        {

        }

        void SetFloat4(string name, Vector4 value)
        {

        }

        void SetMat4(string name, Matrix4x4 value)
        {

        }
    }
}
