#pragma once

#include "Rynex/Renderer/Framebuffer.h"


namespace Rynex {

	class OpenGLFramebuffer : public Framebuffer
	{
	public: 
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;
		
		virtual void Resize(uint32_t withe, uint32_t heigth) override;
		virtual int ReadPixel(uint32_t attachmentsIndex, int x, int y) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::GetColorAttachmentRendererID!"); return m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetFramebufferSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}


