using System;
using Rynex;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Policy;
using System.Runtime.CompilerServices;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Rynex
{

    public enum ImageFormat
	{
		None = 0,
		R8,

		RGB8,
		RGBA8,

		RGB32F,
		RGBA32F,


		RED_INTEGER,
		DEPTH24STENCIL8,
		Depth = DEPTH24STENCIL8,
	};

	public enum TextureTarget
    {
        Nono = 0,
        Texture1D, Texture2D, Texture3D,
        TextureRectAngle,
        TextureBuffer,
        TextureCubeMap,

        ImageTexture,
        FrameBufferTexture
    };

    public enum TextureFilteringMode
	{
		None = 0,
		Linear,
		Nearest,
	};

	public enum TextureWrappingMode
    {
        None = 0,
        Repeate,
        MirrorRepeate,
        ClampEdge,
        ClampBorder,
        MirrorClampEdge
    };

    public struct TextureWrappingSpecification
    {
        public TextureWrappingMode S;
        public TextureWrappingMode T;
        public TextureWrappingMode R;

        public TextureWrappingSpecification(TextureWrappingMode s = TextureWrappingMode.None, TextureWrappingMode t = TextureWrappingMode.None, TextureWrappingMode r = TextureWrappingMode.None)
        {
            S = s;
            T = t;
            R = r;
        }


    }

    public struct TextureSpecification
    {
        public uint Width, Height;
        public ImageFormat Format;
        public uint Samples;
        public bool GenerateMips;
        public TextureWrappingSpecification WrappingSpec;
        public TextureFilteringMode FilteringMode;
        public TextureTarget Target;

        public TextureSpecification(
            ImageFormat image = ImageFormat.RGBA8, 
            TextureTarget target = TextureTarget.Nono, 
            uint width = 1, uint height = 1, 
            TextureWrappingSpecification wrappingSpec = new TextureWrappingSpecification(), 
            TextureFilteringMode filteringMode = TextureFilteringMode.None, 
            uint sample = 1, 
            bool generateMips = true)
        {
            Width = width;
            Height = height;
            Format = image;
            Samples = sample;
            GenerateMips = generateMips;
            WrappingSpec = wrappingSpec;
            FilteringMode = filteringMode;
            Target = target;
        }

        public void GetInteralCalls(out uint withe, out uint heigth, out int format,out uint samples,out bool generateMips, out int wrappingSpec_S, out int wrappingSpec_T,out int wrappingSpec_R,out int filteringMode,out int target)
        {
            withe = Width;
            heigth = Height;
            format = (int)Format;
            samples = Samples;
            generateMips = GenerateMips;
            wrappingSpec_S = (int)WrappingSpec.S;
            wrappingSpec_T = (int)WrappingSpec.T;
            wrappingSpec_R = (int)WrappingSpec.R;
            filteringMode = (int)FilteringMode;
            target = (int)Target;
        }
        
    }

    public class Texture : Asset
    {
        public new AssetHandle Handle { get; internal set; }
        public Texture(uint withe, uint heigth)
        {
            InternalCalls.Texture_Create_Withe_Heigth( out ulong uuid, withe, heigth);
            Handle = new AssetHandle(uuid);
        }
        public Texture(TextureSpecification spec)
        {
            InternalCalls.Texture_Create_Spec(out ulong uuid, ref spec);
            Handle = new AssetHandle(uuid);
        }

        public Texture(TextureSpecification spec, byte[] data, uint countSize)
        {
            unsafe
            {
                fixed (byte* dataPointer = data)
                {
                    InternalCalls.Texture_Create_Spec_Data_ByteSize(out ulong uuid, ref spec, (IntPtr)dataPointer, countSize);
                    Handle = new AssetHandle(uuid);
                }
            }
            
        }

        public Texture(TextureSpecification spec, int[] data, uint countSize)
        {
            unsafe
            {
                fixed (int* dataPointer = data)
                {
                    InternalCalls.Texture_Create_Spec_Data_ByteSize(out ulong uuid, ref spec, (IntPtr)dataPointer, countSize);
                    Handle = new AssetHandle(uuid);
                }
            }

        }

        public Texture(TextureSpecification spec, uint[] data, uint countSize)
        {
            unsafe
            {
                fixed (uint* dataPointer = data)
                {
                    InternalCalls.Texture_Create_Spec_Data_ByteSize(out ulong uuid, ref spec, (IntPtr)dataPointer, countSize);
                    Handle = new AssetHandle(uuid);
                }
            }

        }

        ~Texture()
        {
            Destroy();
            Console.WriteLine($"C# -> Destruktur Aktiv from Texture {(uint)Handle.UUID}");
        }
        public void Destroy()
        {
            InternalCalls.Texture_Destroy(Handle.UUID);
            Console.WriteLine($"C# -> Destroy Aktiv from Texture {(uint)Handle.UUID}");
        }

        public TextureSpecification GetSpecification()
         {
            InternalCalls.Texture_GetSpecification( Handle.UUID, out TextureSpecification spec);
            return spec;
         }

        public uint GetWidth()
        {
            InternalCalls.Texture_GetWidth(Handle.UUID, out uint width);
            return width;
        }

        public uint GetHeight()
        {
            InternalCalls.Texture_GetHeight(Handle.UUID, out uint height);
            return height;
        }

        public uint GetRenderID()
        {
            InternalCalls.Texture_GetHeight(Handle.UUID, out uint renderID);
            return renderID;
        }

        public void SetData(byte[] data, uint countSize)
        {
            unsafe
            {
                fixed (byte* dataPointer = data)
                    InternalCalls.Texture_SetData(Handle.UUID, (IntPtr)dataPointer, countSize);
            }
        }
        public void SetData(int[] data, uint countSize)
        {
            unsafe
            {
                fixed (int* dataPointer = data)
                    InternalCalls.Texture_SetData(Handle.UUID, (IntPtr)dataPointer, countSize);
            }
        }

        public void SetData(uint[] data, uint countSize)
        {
            unsafe
            {
                fixed (uint* dataPointer = data)
                    InternalCalls.Texture_SetData(Handle.UUID, (IntPtr)dataPointer, countSize);
            }
        }

        public void Resize(uint height, uint width)
        {
            InternalCalls.Texture_Resize(Handle.UUID, height, width);
        }

        public void Bind(uint slot)
        {
            InternalCalls.Texture_Bind(Handle.UUID, slot);
        }
    }
}
