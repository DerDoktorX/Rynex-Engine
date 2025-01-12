#include "rypch.h"
#include "OpenGLFramebuffer.h"
#include <Platform/OpenGL/OpenGLTexture.h>
#include <Rynex/Renderer/API/Texture.h>
#include "Rynex/Core/Application.h"
#include <Platform/OpenGL/OpenGLThreadContext.h>

#include <glad/glad.h>

namespace Rynex {

	static const uint32_t s_MaxFrambufferSize = 8192;

	namespace Utils {

		static GLenum AtchemtType(TextureFormat attachmentType)
		{
			switch (attachmentType)
			{
			case TextureFormat::R8:
			case TextureFormat::RG8:
			case TextureFormat::RGB8:
			case TextureFormat::RGBA8:
			case TextureFormat::RGBA32F:
			case TextureFormat::RED_INTEGER:
				return GL_COLOR_ATTACHMENT0;

			case TextureFormat::DepthComp:
			case TextureFormat::DepthComp16:
			case TextureFormat::DepthComp24:
			case TextureFormat::DepthComp32:
			case TextureFormat::DepthComp32F:
				return GL_DEPTH_ATTACHMENT;

			case TextureFormat::Depth24Stencil8:
			case TextureFormat::Depth32FStencil8:
				return GL_DEPTH_STENCIL_ATTACHMENT;
			}
			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLDataFormat!");
			return GL_COLOR_ATTACHMENT0;
		}

		static GLenum FormatData(TextureFormat attachmentType)
		{
			switch (attachmentType)
			{
			case TextureFormat::R8:					
				return GL_RED;

			case TextureFormat::RG8:				
				return GL_RG;

			case TextureFormat::RGB8:				
				return GL_RGB;

			case TextureFormat::RGBA8:				
			case TextureFormat::RGBA32F:			
				return GL_RGBA;

			case TextureFormat::RED_INTEGER:		
				return GL_RED_INTEGER;

			case TextureFormat::DepthComp:
			case TextureFormat::DepthComp16:
			case TextureFormat::DepthComp24:
			case TextureFormat::DepthComp32:
			case TextureFormat::DepthComp32F:
				return GL_DEPTH_COMPONENT;

			case TextureFormat::Depth24Stencil8:
			case TextureFormat::Depth32FStencil8:
				return GL_DEPTH_STENCIL;
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLDataFormat!");
			return GL_RGBA;
		}

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		
		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static bool IsDeathTex(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::None:				
				break;
			case TextureFormat::R8:					return false;
			case TextureFormat::RG8:				return false;

			case TextureFormat::RGB8:				return false;
			case TextureFormat::RGB16F:				return false;
			
			case TextureFormat::RGBA8:				return false;
			case TextureFormat::RGBA16F:			return false;
			case TextureFormat::RGB32F:				return false;
			case TextureFormat::RGBA32F:			return false;

			case TextureFormat::RED_INTEGER:		return false;

			case TextureFormat::DepthComp:			return true;
			case TextureFormat::DepthComp16:		return true;
			case TextureFormat::DepthComp24:		return true;
			case TextureFormat::DepthComp32:		return true;
			case TextureFormat::DepthComp32F:		return true;

			case TextureFormat::Depth24Stencil8:	return true;
			case TextureFormat::Depth32FStencil8:	return true;
			default:
				break;
			}
			RY_CORE_ASSERT(false, "Not set or not known TexureFormat!");
		}

		static GLenum AccesType(Acces acces)
		{
			switch (acces)
			{

			case Acces::Read:		return GL_READ_ONLY;
			case Acces::Write:		return GL_WRITE_ONLY;
			case Acces::ReadWrite:	return GL_READ_WRITE;
			default:
				break;
			}
			RY_CORE_ASSERT(false, "Unknown AccesType!");
			return GL_READ_WRITE;
		}

		static void TexturDefaultTypesColor(FramebufferTextureSpecification* attachment)
		{
			TexFilter& filter = attachment->TextureFiltering;
			if (filter == TexFilter::Default)
				filter = TexFilter::Nearest;

			TexFrom& fromat = attachment->TextureFormat;
			if (fromat == TexFrom::Default)
				fromat = TexFrom::Depth24Stencil8;

			TextureWrappingMode& warpT = attachment->TextureWrapping.T;
			if (warpT == TexWarp::Default)
				warpT = TexWarp::Repeate;

			TextureWrappingMode& warpR = attachment->TextureWrapping.R;
			if (warpR == TexWarp::Default)
				warpR = TexWarp::None;

			TextureWrappingMode& warpS = attachment->TextureWrapping.S;
			if (warpS == TexWarp::Default)
				warpS = TexWarp::Repeate;
		}

		static void TexturDefaultTypesDepth(FramebufferTextureSpecification* attachment)
		{
			TexFilter& filter = attachment->TextureFiltering;
			if (filter == TexFilter::Default)
				filter = TexFilter::Nearest;

			TexFrom& fromat = attachment->TextureFormat;
			if (fromat == TexFrom::Default)
				fromat = TexFrom::Depth24Stencil8;

			TextureWrappingMode& warpT = attachment->TextureWrapping.T;
			if (warpT == TexWarp::Default)
				warpT = TexWarp::ClampEdge;

			TextureWrappingMode& warpR = attachment->TextureWrapping.R;
			if (warpR == TexWarp::Default)
				warpR = TexWarp::None;

			TextureWrappingMode& warpS = attachment->TextureWrapping.S;
			if (warpS == TexWarp::Default)
				warpS = TexWarp::ClampEdge;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{

		bool deathFound = false;
		
		uint32_t withe = m_Specification.Width, height = m_Specification.Height;
		m_Size = { withe , height };
	
		
		for (FramebufferTextureSpecification& attachment : m_Specification.Attachments)
		{
			
			if (!deathFound && Utils::IsDeathTex(attachment.TextureFormat))
			{
				Utils::TexturDefaultTypesDepth(&attachment);
				TextureSpecification spec = {
					withe, height,
					TextureTarget::Texture2D,
					attachment.TextureFormat,
					attachment.Samples,
					attachment.TextureFiltering,
					attachment.TextureWrapping
				};
				m_DepthAttachment = CreateRef<OpenGLFrameTexture>(spec);
				m_DepthAttachmentTex = m_DepthAttachment;
				deathFound = true;
			}
			else
			{
				Utils::TexturDefaultTypesColor(&attachment);
				TextureSpecification spec = {
					withe, height,
					TextureTarget::Texture2D,
					attachment.TextureFormat,
					attachment.Samples,
					attachment.TextureFiltering,
					attachment.TextureWrapping
				};
				
				Ref<OpenGLFrameTexture> tex = CreateRef<OpenGLFrameTexture>( spec);
				m_ColorAttachmentsTex.push_back(tex);
				if (attachment.Samples > 1)
					m_ColorAttachmentsMultySamples.push_back(tex);
				else
					m_ColorAttachments.push_back(tex);

			}	
			
		}
		
		Invalidate();
		
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		if(m_RendererID)
		{
			glDeleteFramebuffers(1,&m_RendererID);
			
		}

		for (Ref<OpenGLFrameTexture>tex : m_ColorAttachments)
			tex.reset();

		for (Ref<OpenGLFrameTexture>tex : m_ColorAttachmentsMultySamples)
			tex.reset();

	
		m_DepthAttachment.reset();
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, int value)
	{
		RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::ClearAttachment!");

		const TextureSpecification& spec = m_ColorAttachments[index]->GetSpecification();
		glClearTexImage(m_ColorAttachments[index]->GetRenderID(), 0,
			Utils::FormatData(spec.Format), GL_INT, &value);
	}

	const FramebufferSpecification& OpenGLFramebuffer::GetFramebufferSpecification() const
	{
		return m_Specification;
	}

	uint32_t OpenGLFramebuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::GetColorAttachmentRendererID!");
		return m_ColorAttachments[index]->GetRenderID();
	}

	uint32_t OpenGLFramebuffer::GetDeathAttachmentRendererID() const
	{
		return m_DepthAttachment->GetRenderID();
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::BindColorAttachment(uint32_t index, uint32_t slot) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::GetColorAttachmentRendererID!");
		glBindTextureUnit(slot, m_ColorAttachments[index]->GetRenderID());
	}

	void OpenGLFramebuffer::BindDeathAttachment(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_DepthAttachment->GetRenderID());
	}

	void OpenGLFramebuffer::BindColorAttachmentImage(Acces acces, uint32_t index, uint32_t slot) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::BindColorAttachmentImage!");
		m_ColorAttachments[index]->BindImage(acces, slot);
	}

	void OpenGLFramebuffer::BindDeathAttachmentImage(Acces acces, uint32_t slot) const
	{
		m_DepthAttachment->BindImage(acces, slot);
	}



	void OpenGLFramebuffer::Invalidate()
	{
		if(OpenGLThreadContext::IsActive())
		{
			uint32_t withe = m_Size.x, height = m_Size.y;
			if (m_RendererID)
			{
				glDeleteFramebuffers(1, &m_RendererID);
			}

			glCreateFramebuffers(1, &m_RendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

			uint32_t texSize = m_ColorAttachments.size();
			if (texSize)
			{
				std::vector<GLuint> texIDs(texSize);
				GLenum targetTex = GL_TEXTURE_2D;
				glCreateTextures(targetTex, texSize, texIDs.data());
				uint32_t i = 0;
				for (Ref<OpenGLFrameTexture> tex : m_ColorAttachments)
				{
					uint32_t id = texIDs[i];
					const TextureSpecification& attachment = tex->GetSpecification();

					tex->Invalidate({
						withe, height,
						attachment.Target,
						attachment.Format,
						attachment.Samples,
						attachment.FilteringMode,
						attachment.WrappingSpec
						}, id);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, targetTex, id, 0);
					i++;
				}
			}

			uint32_t multySamleTexSize = m_ColorAttachmentsMultySamples.size();
			if (multySamleTexSize)
			{
				std::vector<GLuint> multySamleTexIDs(multySamleTexSize);
				GLenum targetMultySamleTex = GL_TEXTURE_2D_MULTISAMPLE;
				glCreateTextures(targetMultySamleTex, multySamleTexSize, multySamleTexIDs.data());

				uint32_t i = 0;

				for (Ref<OpenGLFrameTexture> tex : m_ColorAttachmentsMultySamples)
				{
					uint32_t id = multySamleTexIDs[i];

					const TextureSpecification& attachment = tex->GetSpecification();
					tex->Invalidate({
						withe, height,
						attachment.Target,
						attachment.Format,
						attachment.Samples,
						attachment.FilteringMode,
						attachment.WrappingSpec
						}, id);
					glFramebufferTexture2D(GL_FRAMEBUFFER, Utils::AtchemtType(attachment.Format) + i, targetMultySamleTex, id, 0);
					i++;
				}
			}

			uint32_t colorAttachment = multySamleTexSize + texSize;
			if (m_DepthAttachment)
			{
				const TextureSpecification& attachment = m_DepthAttachment->GetSpecification();
				GLenum target = Utils::TextureTarget(attachment.Samples > 1);
				uint32_t id = 0;

				glCreateTextures(target, 1, &id);

				m_DepthAttachment->Invalidate({
					withe, height,
					attachment.Target,
					attachment.Format,
					attachment.Samples,
					attachment.FilteringMode,
					attachment.WrappingSpec
					}, id);


				glFramebufferTexture2D(GL_FRAMEBUFFER, Utils::AtchemtType(attachment.Format), target, id, 0);
			}


			if (colorAttachment)
			{
				RY_CORE_ASSERT(colorAttachment <= 10, "Error: OpenGLFramebuffer::Invalidate!");
				GLenum buffers[10] = {
					GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
					GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
				};
				glDrawBuffers(colorAttachment, buffers);
			}
			else if (colorAttachment == 0)
			{
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}

			RY_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer is not Complet!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{

			Application::Get().SubmiteToMainThreedQueue([this]() {
				Invalidate();
				});
		}
	}

	const Ref<Texture>& OpenGLFramebuffer::GetAttachmentTexture(uint32_t index) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::GetAttachmentTexture!");

		return (m_ColorAttachments[index]);
	}

	const std::vector<Ref<Texture>>& OpenGLFramebuffer::GetAttachmentsTextures() const
	{
		return m_ColorAttachmentsTex;
	}

	const uint32_t OpenGLFramebuffer::GetAttachmentTexturesSize() const
	{
		return m_ColorAttachments.size();
	}

	void OpenGLFramebuffer::InitAsync()
	{
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrambufferSize || height > s_MaxFrambufferSize)
		{
			RY_CORE_WARN("Faild Resize frambueffer to {0}, {1}", width, height);
			return;
		}
		m_Size = { width , height };
		m_Specification.Width = m_Size.x;
		m_Specification.Height = m_Size.y;
		RY_CORE_TRACE("Resize frambueffer to {0}, {1}", m_Size.x, m_Size.y);
		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentsIndex, int x, int y)
	{
		RY_CORE_ASSERT(attachmentsIndex < m_ColorAttachments.size() && x <= m_Size.x && y <= m_Size.y, "Error OpenGLFramebuffer::ReadPixxel!");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentsIndex);
		int pixeldata;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixeldata);

		return pixeldata;
	}

	const glm::uvec2& OpenGLFramebuffer::GetFrambufferSize()
	{
		return m_Size;
	}

}
