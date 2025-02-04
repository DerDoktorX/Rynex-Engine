using System;

namespace Rynex
{

    public enum FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        Depth24Stencil8,

        // Defaults
        Depth = Depth24Stencil8
    }

    public enum FrameBufferImageSize
    {
        Nono = 0,
        MainViewPort,
        RelativToMainViewPort,
        StaticSize
    }

    public struct FramebufferTextureSpecification
    {

        public FramebufferTextureFormat TextureFormat;
        public TextureWrappingSpecification TextureWrapping;
        public TextureFilteringMode TextureFiltering;
        public uint Samples;
        public FramebufferTextureSpecification(
            FramebufferTextureFormat format = FramebufferTextureFormat.None,
            TextureWrappingSpecification wrapping = new TextureWrappingSpecification(),
            TextureFilteringMode filtering = TextureFilteringMode.None,
            uint samples = 1)
        {
            TextureFormat = format;
            TextureWrapping = wrapping;
            TextureFiltering = filtering;
            Samples = samples;
        }
    }

    public struct FramebufferAttachmentSpecification
    {

        public FramebufferTextureSpecification[] Attachments;



        public FramebufferAttachmentSpecification(FramebufferTextureSpecification[] attachments)
        {
            Attachments = attachments;
        }
    }

    public struct FramebufferSpecification
    {

        public uint Width;
        public uint Height;
        public FramebufferAttachmentSpecification Attachments;
        public bool SwapChainTarget;

        public FramebufferSpecification(
            FramebufferAttachmentSpecification attachments = new FramebufferAttachmentSpecification(),
            uint width = 0,
            uint height = 0,

            bool swapChainTarget = false)
        {
            Attachments = attachments;
            Width = width;
            Height = height;
            SwapChainTarget = swapChainTarget;
        }

        public FramebufferSpecification(
            FramebufferTextureSpecification[] attachments,
            uint width = 0,
            uint height = 0,
            uint sample = 1,
            bool swapChainTarget = false)
        {
            Attachments = new FramebufferAttachmentSpecification(attachments);
            Width = width;
            Height = height;
            SwapChainTarget = swapChainTarget;
        }

    }

    public class Framebuffer : Asset
    {
        public new AssetHandle Handle { get; internal set; }
        public Framebuffer(ulong handle)
        {
            Handle = new AssetHandle();
        }
        public Framebuffer(FramebufferSpecification spec)
        {

            FramebufferTextureSpecification[] attachments = spec.Attachments.Attachments;
            uint size = (uint)attachments.Length;
            unsafe
            {
                fixed (FramebufferTextureSpecification* attachmentsPtr = attachments)
                {
                    InternalCalls.Framebuffer_Create_Spec(out ulong uuid, (IntPtr)attachmentsPtr, size, spec.Width, spec.Height, spec.SwapChainTarget);
                    Handle = new AssetHandle(uuid);
                }
            }
        }

        public Framebuffer(FramebufferTextureSpecification[] attachments,
            uint width = 0,
            uint height = 0,
            bool swapChainTarget = false)
        {
            uint size = (uint)attachments.Length;
            unsafe
            {
                fixed (FramebufferTextureSpecification* attachmentsPtr = attachments)
                {
                    InternalCalls.Framebuffer_Create_Spec(out ulong uuid, (IntPtr)attachmentsPtr, size, width, height, swapChainTarget);
                    Handle = new AssetHandle(uuid);
                }
            }
        }
        ~Framebuffer()
        {
            Destroy();
        }

        public void Destroy()
        {
            InternalCalls.Framebuffer_Destroy(Handle.UUID);
        }

        public static Framebuffer GetAsset(string path)
        {
            InternalCalls.AssetManger_GetAsset_Path(path, AssetType.Framebuffer, out ulong handle);
            return new Framebuffer(handle);
        }
        public static Framebuffer GetAsset(ulong handle)
        {
            InternalCalls.AssetManger_GetAsset_Handle(handle, AssetType.Framebuffer, out ulong outHandle);
            return new Framebuffer(outHandle);
        }

        public FramebufferSpecification GetFramebufferSpecification()
        {


            InternalCalls.Framebuffer_GetFramebufferSpecification(Handle.UUID, out uint size, out uint width, out uint height, out bool swapChainTarget);

            FramebufferTextureSpecification[] attachments = new FramebufferTextureSpecification[size];
            for (uint i = 0; i < size; i++)
            {
                InternalCalls.Framebuffer_GetFramebufferTextureSpecification(Handle.UUID, i, out attachments[i]);
            }
            FramebufferSpecification spec = new FramebufferSpecification(
                new FramebufferAttachmentSpecification(attachments), width, height, swapChainTarget);
            return spec;
        }

        public void Resize(uint withe, uint heigth)
        {
            InternalCalls.Framebuffer_Resize(Handle.UUID, heigth, withe);
        }

        public Vector2 GetFrambufferSize()
        {
            InternalCalls.Framebuffer_GetFrambufferSize(Handle.UUID, out Vector2 value);
            return value;
        }

        public int ReadPixel(uint attachmentsIndex, int x, int y)
        {
            InternalCalls.Framebuffer_ReadPixel(Handle.UUID, attachmentsIndex, x, y, out int value);
            return value;
        }

    }

}
