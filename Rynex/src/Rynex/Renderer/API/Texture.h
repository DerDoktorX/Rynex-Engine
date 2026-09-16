#pragma once
#include <Rynex/Asset/Base/Asset.h>


// #define RY_TEXTURE_STORE_ARRAY

namespace Rynex {
	
	enum class TextureFormat : uint8_t
	{
		None = 0,
		Default = 1,

		R8,
		RG8,
		
		S_RGB8,
		S_RGBA8,
		
		RGB8,
		RGBA8,

		RGB16F,
		RGBA16F,

		RGB32F,
		RGBA32F,


		RED_INTEGER,

		// DepthComp,
		DepthComp16,
		DepthComp24,
		DepthComp32,
		DepthComp32F,

		Depth24Stencil8,
		Depth32FStencil8,
		
		
	};


	enum class TextureTarget : uint8_t
	{
		None = 0,
		Default = 1,

		Texture1D, Texture2D, Texture3D,
		TextureRectAngle, 
		TextureBuffer,
		TextureCubeMap,

		Texture1D_Array,
		Texture2D_Array,
		TextureCubeMap_Array,

	};

	enum class TextureFilteringMode : uint8_t
	{
		None = 0,
		Default = 1,

		Linear,
		Nearest,
		LinearMidMapLinear,
		LinearMidMapNearest,    // LinearMidMapNearest
	};

	enum class TextureCompareModes : uint8_t
	{
		None = 0,
		Default = 1,

		Lequal,
		Always,
		Gequal,
		Less,
		Greater,
		Equal,
		Never
	};

	enum class TextureWrappingMode : uint8_t
	{
		None = 0,
		Default = 1,

		Repeat,
		MirrorRepeat,  // MirrorRepeat
		ClampEdge,
		ClampBorder,
		MirrorClampEdge
	};

	enum class Access : uint8_t
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
	using TexComp = TextureCompareModes;


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

		TextureWrappingMode& operator[](const int index)
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

		bool operator ==(const TextureWrappingSpecification textureWrappingSpecification) const
		{
			return (textureWrappingSpecification.R == R) && (textureWrappingSpecification.S == S) && (textureWrappingSpecification.T == T);
		}

		bool operator !=(const TextureWrappingSpecification textureWrappingSpecification) const
		{
			return (textureWrappingSpecification.R != R) || (textureWrappingSpecification.S != S) || (textureWrappingSpecification.T != T);
		}

	};

	struct TextureSpecification
	{
		uint32_t Width, Height, Depth = 1u;
		TextureTarget Target = TextureTarget::Texture2D;
		TextureFormat Format = TextureFormat::RGBA8;
		
		uint32_t Samples = 1u;
		TextureFilteringMode FilteringMode = TextureFilteringMode::Nearest;
		TextureWrappingSpecification WrappingSpec{
			TextureWrappingMode::Repeat,
			TextureWrappingMode::Repeat,
		};
		TextureCompareModes Compare = TextureCompareModes::None;
		uint32_t GenerateMips = 0u;


		bool operator==(const TextureSpecification& spec) const
		{
			return Width == spec.Width
				&& Height == spec.Height
				&& Target == spec.Target
				&& Format == spec.Format
				&& Samples == spec.Samples
				&& FilteringMode == spec.FilteringMode
				&& WrappingSpec == spec.WrappingSpec
				&& Compare == spec.Compare
				&& GenerateMips == spec.GenerateMips;
		}

		bool operator!=(const TextureSpecification& spec) const
		{
			return Width != spec.Width
				|| Height != spec.Height
				|| Target != spec.Target
				|| Format != spec.Format
				|| Samples != spec.Samples
				|| FilteringMode != spec.FilteringMode
				|| WrappingSpec != spec.WrappingSpec
				|| Compare != spec.Compare
				|| GenerateMips != spec.GenerateMips;
		}

		bool IsEqualWitheOutTextureTarget(const TextureSpecification& spec)const
		{
			return Width == spec.Width
				&& Height == spec.Height
				&& Format == spec.Format
				&& Samples == spec.Samples
				&& FilteringMode == spec.FilteringMode
				&& WrappingSpec == spec.WrappingSpec
				&& Compare == spec.Compare
				&& GenerateMips == spec.GenerateMips;
		}
	};

	


	class RYNEX_API Texture : public Asset
	{
	public:
		static Ref<Texture> Create(TextureSpecification spec, void* dataPtr, uint32_t size);
		static Ref<Texture> CreateAsync(TextureSpecification spec, std::vector<unsigned char>&& data);
		static Ref<Texture> Create(TextureSpecification spec);
		static Ref<Texture> Default();
		
		static Ref<Texture> White();
		static void Shutdown();
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;

		virtual uint32_t GetRenderID() const = 0;


		virtual void SetData(void* dataPtr, uint32_t size) = 0;
		virtual void FreeCurrentData() = 0;
		virtual const std::vector<unsigned char> GetCurrentRenderData() = 0;
		virtual const std::vector<unsigned char> GetCurrentRenderData(TextureFormat format) = 0;
		virtual const uint32_t GetCurrentRenderDataSize() const = 0;
		
		virtual bool IsBindLessTexActiv() const = 0;

		virtual bool IsTransfer() = 0;//IsTransfer

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void BindImage(Access access,uint32_t slot = 0) const = 0;
		virtual void UnBind(uint32_t slot = 0) const = 0;
		virtual void UnBindImage(Access access, uint32_t slot = 0) const = 0;
		
		virtual void BindLessTex() = 0;
		virtual void UnBindLessTex() = 0;
		virtual uint64_t GetBindlessHandle() const = 0;// GetBindlessHandle

		virtual void Resize1D(uint32_t width) = 0;
		virtual void Resize2D(uint32_t width, uint32_t height) = 0;
		virtual void Resize3D(uint32_t width, uint32_t height, uint32_t depth) = 0;
		static void BindTex(uint32_t renderID, uint32_t slot = 0);

		virtual bool operator==(const Texture& other) const = 0;

		static AssetType GetStaticType() { return AssetType::Texture; }
		AssetType GetType() const override { return GetStaticType(); }
	};
	

	class RYNEX_API LinkedTextureArray : public Asset
	{
	public:
		static Ref<LinkedTextureArray> Create(TextureSpecification spec);
		virtual ~LinkedTextureArray() {};

		virtual const TextureSpecification& GetValidTextureSpecification() const = 0;

		virtual uint32_t GetValidTextureWidth() const = 0;
		virtual uint32_t GetValidTextureHeight() const = 0;
		virtual uint32_t GetValidTextureDepth() const = 0;
		virtual uint32_t GetRenderID() const = 0;

		virtual bool IsTransfer() = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void BindImage(Access access, uint32_t slot = 0) const = 0;
		virtual void UnBind(uint32_t slot = 0) const = 0;
		virtual void UnBindImage(Access access, uint32_t slot = 0) const = 0;


		virtual void SetTextureToArray(int index, const Ref<Texture>& texture, bool instantlyGPUploade = false) = 0;
		virtual void SetTextureToArray(int index, std::nullptr_t, bool instantlyGPUploade = false) = 0;

		virtual Ref<Texture> GetTextureToArray(int index) = 0;
		virtual void ResizeTextureArray(int size, bool instantlyGPUploade = false) = 0;

		virtual const std::vector<Weak<Texture>>& GetTextureVec() const = 0;
		virtual uint32_t GetTextureCount() const = 0;
		virtual void ClearTextures() = 0;

		virtual bool IsDataReadyOnGPU() const = 0;
		virtual void UpdateDataGPU() = 0;

		virtual void ResizeDimensions1D(uint32_t width) = 0;
		virtual void ResizeDimensions2D(uint32_t width, uint32_t height) = 0;
		virtual void ResizeDimensions3D(uint32_t width, uint32_t height, uint32_t depth) = 0;
		
		virtual const TextureSpecification& GetSpecification() const = 0;
		virtual bool operator==(const LinkedTextureArray& other) const = 0;

		static AssetType GetStaticType() { return AssetType::LinkedTextureArray; }
		AssetType GetType() const override { return GetStaticType(); }
	};


}
