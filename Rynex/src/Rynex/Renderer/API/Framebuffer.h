#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/API/Texture.h"


namespace Rynex {
#if 1
	
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

		FramebufferTextureSpecification(TextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples)
			: TextureFormat(format), Samples(samples){}

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples, TextureWrappingSpecification wrapping)
			: TextureFormat(format), Samples(samples), TextureWrapping(wrapping) {}


		FramebufferTextureSpecification(TextureFormat format, uint32_t samples, TextureWrappingSpecification wrapping, TextureFilteringMode filtering)
			: TextureFormat(format), Samples(samples), TextureWrapping(wrapping), TextureFiltering(filtering) {}
		

		TextureFormat TextureFormat = TextureFormat::RGBA8;
		uint32_t Samples = 1;
		TextureWrappingSpecification TextureWrapping = {
			TextureWrappingMode::ClampEdge,
			TextureWrappingMode::ClampEdge,
			TextureWrappingMode::ClampEdge
		};
		TextureFilteringMode TextureFiltering = TextureFilteringMode::Linear;

		bool operator ==(FramebufferTextureSpecification& framebufferTextureSpecification)
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

		std::vector<FramebufferTextureSpecification>::iterator begin()
		{
			return Attachments.begin();
		}

		std::vector<FramebufferTextureSpecification>::iterator end()
		{
			return Attachments.end();
		}

		std::vector<FramebufferTextureSpecification>::const_iterator begin() const
		{
			return Attachments.begin();
		}

		std::vector<FramebufferTextureSpecification>::const_iterator end() const
		{
			return Attachments.end();
		}

		FramebufferTextureSpecification& operator[](uint32_t index)
		{
			RY_CORE_ASSERT(index > Attachments.size());
			return Attachments[index];
		}

		const FramebufferTextureSpecification& operator[](uint32_t index) const
		{
			RY_CORE_ASSERT(index > Attachments.size());
			return Attachments[index];
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 1, Height = 1;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};

	static std::string GetStringFromFramTexFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::None:			return "None";
			case TextureFormat::RGBA8:			return "RGBA8";
			case TextureFormat::RED_INTEGER:	return "RED_INTEGER";
			// case TextureFormat::Depth:			return "Depth";
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

		virtual const Ref<Texture>& GetAttachmentTexture(uint32_t index = 0) const = 0;
		virtual const std::vector<Ref<Texture>>& GetAttachmentsTextures() const = 0;
		virtual const uint32_t GetAttachmentTexturesSize() const = 0;

		virtual const Ref<Texture>& GetDepthTexture() const = 0;
		virtual void InitAsync() = 0;

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


