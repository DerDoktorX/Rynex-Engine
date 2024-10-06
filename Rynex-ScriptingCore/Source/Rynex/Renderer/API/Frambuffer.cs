using Rynex;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Rynex
{

    public enum FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
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

        public FramebufferTextureSpecification(
            FramebufferTextureFormat format = FramebufferTextureFormat.None,
            TextureWrappingSpecification wrapping = new TextureWrappingSpecification(),
            TextureFilteringMode filtering = TextureFilteringMode.None)
        {
            TextureFormat = format;
            TextureWrapping = wrapping;
            TextureFiltering = filtering;
        }
    }

    public struct FramebufferAttachmentSpecification
    {
        public FramebufferAttachmentSpecification[] Attachments;
    }

    struct FramebufferSpecification
    {
        
        public uint Width;
        public uint Height;
        public FramebufferAttachmentSpecification Attachments;
        public uint Samples;
        public bool SwapChainTarget;

        public FramebufferSpecification(
            FramebufferAttachmentSpecification attachments = new FramebufferAttachmentSpecification(),
            uint width = 0,
            uint height = 0,
            uint sample = 1,
            bool swapChainTarget = false)
        {
            Attachments = attachments;
            Width = width;
            Height = height;
            Samples = sample;
            SwapChainTarget = swapChainTarget;
        }
        
    }

    public class Frambuffer : Asset
    {
#if true
        public Frambuffer()
        {

        }
#endif
        public void ClearAttachment(uint attachmentIndex, int value)
        {

        }

        public FramebufferAttachmentSpecification GetFramebufferSpecification()
        {
            return new FramebufferAttachmentSpecification();
        }

        public uint GetColorAttachmentRendererID(uint index = 0)
        {
            return 0;
        }

        public uint GetDeathAttachmentRendererID()
        {
            return 0;
        }



    }

}
