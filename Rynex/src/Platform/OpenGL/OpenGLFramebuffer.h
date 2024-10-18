#pragma once
#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Renderer/API/Texture.h"

namespace Rynex {

	class OpenGLFramebuffer : public Framebuffer
	{
	public: 
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void BindColorAttachment(uint32_t index = 0, uint32_t slot = 0) const override;
		virtual void BindDeathAttachment(uint32_t index = 0) const override;

		virtual void BindColorAttachmentImage(Acces acces, uint32_t index = 0, uint32_t slot = 0) const override;
		virtual void BindDeathAttachmentImage(Acces acces, uint32_t slot = 0) const override;

		
		virtual void Resize(uint32_t withe, uint32_t heigth) override;
		virtual int ReadPixel(uint32_t attachmentsIndex, int x, int y) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual Ref<Texture> GetTexture(uint32_t index = 0) const  const override  { RY_CORE_ASSERT(index < m_Texures.size(), "Error: OpenGLFramebuffer::GetTexture!"); return m_Texures[index]; }
		virtual std::vector<Ref<Texture>> GetTextures() const  override { return m_Texures; }
		virtual const uint32_t GetTexturesSize() const override { return m_Texures.size(); };
		virtual const glm::uvec2& GetFrambufferSize() { return m_FrambufferSize; }

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::GetColorAttachmentRendererID!"); return m_ColorAttachments[index]; }
		virtual uint32_t GetDeathAttachmentRendererID() const override { return m_DepthAttachment; }
		// virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };
	

		virtual const FramebufferSpecification& GetFramebufferSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		
		glm::uvec2 m_FrambufferSize;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		std::vector<Ref<Texture>> m_Texures;
		std::vector<AssetHandle> m_TexuresAssets;
	};

}


