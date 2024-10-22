#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/API/Texture.h"


namespace Rynex {
#if 1
	enum class RYNEX_API FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};
#endif
	enum class RYNEX_API FrameBufferImageSize : uint8_t {
		Nono = 0,
		MainViewPort,
		RelativToMainViewPort,
		StaticSize
	};

	

	struct RYNEX_API FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;

		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureSpecification(FramebufferTextureFormat format, TextureWrappingSpecification wrapping)
			: TextureFormat(format), TextureWrapping(wrapping) {}

		FramebufferTextureSpecification(FramebufferTextureFormat format, TextureFilteringMode filtering)
			: TextureFormat(format), TextureFiltering(filtering) {}

		FramebufferTextureSpecification(FramebufferTextureFormat format, TextureWrappingSpecification wrapping, TextureFilteringMode filtering)
			: TextureFormat(format), TextureWrapping(wrapping), TextureFiltering(filtering) {}
		

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		TextureWrappingSpecification TextureWrapping = TextureWrappingSpecification();
		TextureFilteringMode TextureFiltering = TextureFilteringMode::None;

		bool operator ==(FramebufferTextureSpecification framebufferTextureSpecification)
		{
			return (framebufferTextureSpecification.TextureFormat == TextureFormat) && 
				(framebufferTextureSpecification.TextureFiltering == TextureFiltering) &&
				(framebufferTextureSpecification.TextureWrapping == TextureWrapping);
		}
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}
		FramebufferAttachmentSpecification(std::vector<FramebufferTextureSpecification>& attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;

		FramebufferSpecification() = default;
		FramebufferSpecification(FramebufferAttachmentSpecification attachments)
			: Attachments(attachments) {}

		FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint32_t width,  uint32_t height)
			: Attachments(attachments), Width(width), Height(height) {}

		FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint32_t sample)
			: Attachments(attachments), Samples(sample) {}

		FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint32_t width, uint32_t height, uint32_t sample)
			: Attachments(attachments), Width(width), Height(height), Samples(sample) {}

		FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint32_t width, uint32_t height, uint32_t sample, bool swapChainTarget)
			: Attachments(attachments), Width(width), Height(height), Samples(sample), SwapChainTarget(swapChainTarget) {}
	
		FramebufferSpecification(FramebufferAttachmentSpecification attachments, bool swapChainTarget)
			: Attachments(attachments),  SwapChainTarget(swapChainTarget) {}

		FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint32_t width, uint32_t height, bool swapChainTarget)
			: Attachments(attachments), Width(width), Height(height), SwapChainTarget(swapChainTarget) {}
	};

	static std::string GetStringFromFramTexFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
			case FramebufferTextureFormat::None:			return "None";
			case FramebufferTextureFormat::RGBA8:			return "RGBA8";
			case FramebufferTextureFormat::RED_INTEGER:		return "RED_INTEGER";
			case FramebufferTextureFormat::Depth:			return "Depth";
			default:
				break;
		}
		RY_CORE_ASSERT(false, "Not Defined Format from FramebufferTextureFormat");
		return "None";
	}

	class RYNEX_API Framebuffer : public Asset
	{
	public:

		virtual ~Framebuffer() = default;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FramebufferSpecification& GetFramebufferSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDeathAttachmentRendererID() const = 0;

		virtual Ref<Texture> GetTexture(uint32_t index = 0) const = 0;
		virtual std::vector<Ref<Texture>> GetTextures() const = 0;
		virtual const uint32_t GetTexturesSize() const = 0;

		virtual void Resize(uint32_t withe, uint32_t heigth) = 0;
		virtual int ReadPixel(uint32_t attachmentsIndex, int x, int y) = 0;
		virtual const glm::uvec2& GetFrambufferSize() = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void BindColorAttachment(uint32_t index = 0, uint32_t slot = 0) const = 0;
		virtual void BindDeathAttachment(uint32_t slot = 0) const = 0;

		virtual void BindColorAttachmentImage(Acces acces, uint32_t index = 0, uint32_t slot = 0) const = 0;
		virtual void BindDeathAttachmentImage(Acces acces, uint32_t slot = 0) const = 0;

		static AssetType GetStaticType() { return AssetType::Framebuffer; }
		AssetType GetType() const { return GetStaticType(); }
	};
}


