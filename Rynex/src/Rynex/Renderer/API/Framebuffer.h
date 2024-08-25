#pragma once

#include "Rynex/Renderer/API/Texture.h"

namespace Rynex {
#if 1
	enum class FramebufferTextureFormat
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
	enum class FrameBufferImageSize : uint8_t {
		Nono = 0,
		MainViewPort,
		RelativToMainViewPort,
		StaticSize,
	};

	

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
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

	class Framebuffer
	{
	public:

		virtual ~Framebuffer() = default;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FramebufferSpecification& GetFramebufferSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDeathAttachmentRendererID() const = 0;

#if 0
		virtual Ref<Texture2D> GetTexture(uint32_t index = 0) = 0;
#endif
		virtual void Resize(uint32_t withe, uint32_t heigth) = 0;
		virtual int ReadPixel(uint32_t attachmentsIndex, int x, int y) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		
	};
}


