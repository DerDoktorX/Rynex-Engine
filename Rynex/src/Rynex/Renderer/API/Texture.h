#pragma once
#include "Rynex/Asset/Base/Asset.h"

#include "Rynex/Renderer/RendererAPI.h"
#include "Rynex/Asset/Base/Buffer.h"

namespace Rynex {
	

	enum class RYNEX_API ImageFormat
	{
		None = 0,
		R8,
		RG8,
		RGB8,
		RGBA8,

		RGB32F,
		RGBA32F,


		RED_INTEGER,
		DEPTH24STENCIL8,
		Depth = DEPTH24STENCIL8,
	};

	enum class RYNEX_API TextureTarget
	{
		Nono = 0,
		Texture1D, Texture2D, Texture3D,
		TextureRectAngle, 
		TextureBuffer,
		TextureCubeMap,

		ImageTexture,
		FrameBufferTexture
	};

	enum class RYNEX_API TextureFilteringMode
	{
		None = 0,
		Linear,
		Nearest,
	};

	enum class RYNEX_API TextureWrappingMode
	{
		None = 0,
		Repeate,
		MirrorRepeate,
		ClampEdge,
		ClampBorder,
		MirrorClampEdge
	};

	enum class Acces
	{
		None = 0,
		Read,
		Write,
		ReadWrite
	};

	struct RYNEX_API TextureWrappingSpecification
	{
		TextureWrappingMode S = TextureWrappingMode::None;
		TextureWrappingMode T = TextureWrappingMode::None;
		TextureWrappingMode R = TextureWrappingMode::None;

		TextureWrappingSpecification() = default;

		TextureWrappingSpecification(TextureWrappingMode s, TextureWrappingMode t, TextureWrappingMode r)
			: S(s), T(t), R(r) {}

		TextureWrappingSpecification(TextureWrappingMode s, TextureWrappingMode t)
			: S(s), T(t) {}

		TextureWrappingSpecification(TextureWrappingMode s)
			: S(s) {}

		TextureWrappingMode operator[](int index) const
		{
			switch (index)
			{
				case 0:	return S;
				case 1:	return T;
				case 2:	return R;
				default: return TextureWrappingMode::None;
			}
			
		}

		TextureWrappingMode& operator[](int index)
		{
			switch (index)
			{
				case 0:	return S;
				case 1:	return T;
				case 2:	return R;
			}
			
		}

		bool operator ==(TextureWrappingSpecification textureWrappingSpecification)
		{
			return (textureWrappingSpecification.S == S) && (textureWrappingSpecification.T == T) && (textureWrappingSpecification.R == R);
		}

	};

	struct RYNEX_API TextureSpecification
	{
		uint32_t Width = 1, Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		uint32_t Samples = 1;
		bool GenerateMips = true;
		TextureWrappingSpecification WrappingSpec = TextureWrappingSpecification();
		TextureFilteringMode FilteringMode = TextureFilteringMode::None;
		TextureTarget Target = TextureTarget::Nono;

		TextureSpecification() = default;

		TextureSpecification(ImageFormat image, TextureTarget target)
			: Format(image), Target(target) {}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height)
			: Format(image), Target(target), Width(width), Height(height){}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height, TextureFilteringMode filteringMode)
			: Format(image), Target(target), Width(width), Height(height), FilteringMode(filteringMode) {}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height, TextureWrappingSpecification wrappingSpec, TextureFilteringMode filteringMode)
			: Format(image), Target(target), Width(width), Height(height), WrappingSpec(wrappingSpec), FilteringMode(filteringMode) {}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height, TextureWrappingSpecification wrappingSpec, TextureFilteringMode filteringMode, uint32_t sample)
			: Format(image), Target(target), Width(width), Height(height), WrappingSpec(wrappingSpec), FilteringMode(filteringMode), Samples(sample) {}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height, TextureWrappingSpecification wrappingSpec, TextureFilteringMode filteringMode, uint32_t sample, bool generateMips)
			: Format(image), Target(target), Width(width), Height(height), WrappingSpec(wrappingSpec), FilteringMode(filteringMode), GenerateMips(generateMips), Samples(sample) {}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height, TextureFilteringMode filteringMode, uint32_t sample)
			: Format(image), Target(target), Width(width), Height(height), FilteringMode(filteringMode), Samples(sample) {}

		TextureSpecification(ImageFormat image, TextureTarget target, uint32_t width, uint32_t height, TextureFilteringMode filteringMode, uint32_t sample, bool generateMips)
			: Format(image), Target(target), Width(width), Height(height), FilteringMode(filteringMode), GenerateMips(generateMips), Samples(sample) {}

	};

	struct RYNEX_API FramebufferTextureSpecification;

	class RYNEX_API Texture : public Asset
	{
	public:
		static Ref<Texture> CreateFrame(AssetHandle assetHandle, int index);
		static Ref<Texture> Create(uint32_t withe, uint32_t height);
		static Ref<Texture> Create(const std::string& path);
		static Ref<Texture> Create(TextureSpecification spec, void* data, uint32_t size);
		static Ref<Texture> Create(TextureSpecification spec);
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRenderID() const = 0;

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
