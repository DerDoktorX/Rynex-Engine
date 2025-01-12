#pragma once
#include <Rynex/Renderer/API/Framebuffer.h>

namespace Rynex {
	class OpenGLFrameTexture;
	class Texture;

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		~OpenGLFramebuffer();

		virtual void ClearAttachment(uint32_t index, int value) override;

		virtual const FramebufferSpecification& GetFramebufferSpecification() const override;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;
		virtual uint32_t GetDeathAttachmentRendererID() const override;

		virtual const Ref<Texture>& GetAttachmentTexture(uint32_t index = 0) const override;
		virtual const std::vector<Ref<Texture>>& GetAttachmentsTextures() const override;
		virtual const uint32_t GetAttachmentTexturesSize() const override;

		virtual const Ref<Texture>& GetDepthTexture() const override { return m_DepthAttachmentTex; };
		virtual void InitAsync() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t index, int x, int y) override;
		virtual const glm::uvec2& GetFrambufferSize() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void BindColorAttachment(uint32_t index = 0, uint32_t slot = 0) const override;
		virtual void BindDeathAttachment(uint32_t slot = 0) const override;

		virtual void BindColorAttachmentImage(Acces acces, uint32_t index = 0, uint32_t slot = 0) const override;
		virtual void BindDeathAttachmentImage(Acces acces, uint32_t slot = 0) const override;
	private:

		void Invalidate();
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;
		glm::uvec2 m_Size;
		Ref<OpenGLFrameTexture> m_DepthAttachment = nullptr;
		std::vector<Ref<OpenGLFrameTexture>> m_ColorAttachments;
		std::vector<Ref<OpenGLFrameTexture>> m_ColorAttachmentsMultySamples;
		std::vector<Ref<Texture>> m_ColorAttachmentsTex;
		Ref<Texture> m_DepthAttachmentTex;
	};
}

