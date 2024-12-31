#pragma once
#include <Rynex/Asset/Base/Asset.h>

#include <Rynex/Renderer/RendererAPI.h>


namespace Rynex {
	
	enum class TextureFormat
	{
		None = 0,
		Default = 1,

		R8,
		RG8,
		RGB8,
		RGBA8,

		RGB16F,
		RGBA16F,

		RGB32F,
		RGBA32F,


		RED_INTEGER,

		DepthComp,
		DepthComp16,
		DepthComp24,
		DepthComp32,
		DepthComp32F,

		Depth24Stencil8,
		Depth32FStencil8,
		
		
	};

	enum class TextureTarget
	{
		None = 0,
		Default = 1,

		Texture1D, Texture2D, Texture3D,
		TextureRectAngle, 
		TextureBuffer,
		TextureCubeMap,

		ImageTexture,
		FrameBufferTexture
	};

	enum class TextureFilteringMode
	{
		None = 0,
		Default = 1,

		Linear,
		Nearest,
		LinearMidmapLinear,
		LinearMidmapNearest,
	};

	enum class TextureWrappingMode
	{
		None = 0,
		Default = 1,

		Repeate,
		MirrorRepeate,
		ClampEdge,
		ClampBorder,
		MirrorClampEdge
	};

	enum class Acces
	{
		None = 0,
		Default = 1,

		Read,
		Write,
		ReadWrite
	};

	using TexFrom = TextureFormat;
	using TexWarp = TextureWrappingMode;
	using TexFilter = TextureFilteringMode;
	using TexTar = TextureTarget;

	struct TextureWrappingSpecification
	{
		TextureWrappingMode S = TextureWrappingMode::Default;
		TextureWrappingMode T = TextureWrappingMode::Default;
		TextureWrappingMode R = TextureWrappingMode::Default;

		TextureWrappingMode operator[](int index) const
		{
			switch (index)
			{
				case 0:	return S;
				case 1:	return T;
				case 2:	return R;
				default: 
					RY_CORE_ASSERT(false);
					return TextureWrappingMode::Default;
			}
		}

		TextureWrappingMode& operator[](int index)
		{
			switch (index)
			{
			case 0:	return S;
			case 1:	return T;
			case 2:	return R;
			default: 
				RY_CORE_ASSERT(false);
				return T;
			}
		}

		bool operator ==(TextureWrappingSpecification textureWrappingSpecification) const
		{
			return (textureWrappingSpecification.R == R) && (textureWrappingSpecification.S == S) && (textureWrappingSpecification.T == T);
		}

	};

	struct TextureSpecification
	{
		uint32_t Width, Height;
		TextureTarget Target = TextureTarget::Texture2D;
		TextureFormat Format = TextureFormat::RGBA8;
		uint32_t Samples = 1;
		TextureFilteringMode FilteringMode = TextureFilteringMode::Nearest;
		TextureWrappingSpecification WrappingSpec = {
			TextureWrappingMode::Repeate,
			TextureWrappingMode::Repeate,
			// TextureWrappingMode::Repeate
		};
		bool GenerateMips = true;
	};

	struct FramebufferTextureSpecification;

	class RYNEX_API Texture : public Asset
	{
	public:
		// static Ref<Texture> CreateFrame(AssetHandle assetHandle, int index);
		// static Ref<Texture> Create(uint32_t withe, uint32_t height);
		// static Ref<Texture> Create(const std::string& path);
		static Ref<Texture> Create(TextureSpecification spec, void* data, uint32_t size);
		static Ref<Texture> CreateAsync(TextureSpecification spec, std::vector<unsigned char>&& data);
		static Ref<Texture> Create(TextureSpecification spec);
		static Ref<Texture> Default();
		static void Shutdown();
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRenderID() const = 0;


		virtual void InitAsync() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void FreeCurrentData() = 0;
		virtual const std::vector<unsigned char> GetCurrentRenderData() = 0;
		virtual const uint32_t GetCurrentRenderDataSize() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void BindImage(Acces acces,uint32_t slot = 0) const = 0;
		
		virtual bool operator==(const Texture& other)const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		static void BindTex(uint32_t renderID, uint32_t slot = 0);

		static AssetType GetStaticType() { return AssetType::Texture; }
		AssetType GetType() const { return GetStaticType(); }
	};
	
}
