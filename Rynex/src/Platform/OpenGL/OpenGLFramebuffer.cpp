#include "rypch.h"
#include "OpenGLFramebuffer.h"

#include "Rynex/Asset/Base/AssetManager.h"

#include <glad/glad.h>


namespace Rynex {

	static const uint32_t s_MaxFrambufferSize = 8192;

	namespace Utils {

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static GLenum RynexFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			RY_CORE_ASSERT(false, "Not defint Format: RynexFBTextureFormatToGL!");
			return 0;
		}

		static GLenum RynexFBTextureFormatDepthToGLBindImgae(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}

			RY_CORE_ASSERT(false, "Not defint Format: RynexFBTextureFormatToGL!");
			return 0;
		}

		static GLenum GetAccesType(Acces acces)
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

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto specs : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(specs.TextureFormat))
			{
				m_ColorAttachmentSpecifications.emplace_back(specs);
			}
			else
			{
				m_DepthAttachmentSpecification = specs;
			}
		}

		Invalidate();
		
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		RY_PROFILE_FUNCTION();
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
#if 0
		for (auto& tex : m_TextureAttachments)
			tex.reset();
		m_TextureAttachments.clear();
#endif
		
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
			
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;
		
		size_t iT = 0;
		uint32_t indexAsset = 0;
		// Atachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());

			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			
			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{

				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						
					m_Texures.push_back(Texture::CreateFrame(Handle, -1));
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					m_Texures.push_back(Texture::CreateFrame(Handle, -1));
					break;
				}
			}
		}


		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				m_Texures.push_back(Texture::CreateFrame(Handle, -1));
				break;
			}

		}


		if (m_ColorAttachments.size() > 1)
		{
			RY_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Error: OpenGLFramebuffer::Invalidate!");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		RY_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer is not Complet!");
		
		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		

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
		glBindTextureUnit(slot, m_ColorAttachments[index]);
	}

	void OpenGLFramebuffer::BindDeathAttachment(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_DepthAttachment);
	}

	void OpenGLFramebuffer::BindColorAttachmentImage(Acces acces, uint32_t index, uint32_t slot) const
	{
		glBindImageTexture(slot, m_ColorAttachments[index], 0, GL_FALSE, 0, Utils::GetAccesType(acces), Utils::RynexFBTextureFormatToGL(m_ColorAttachmentSpecifications[index].TextureFormat));
	}

	void OpenGLFramebuffer::BindDeathAttachmentImage(Acces acces, uint32_t slot) const
	{
		glBindImageTexture(slot, m_DepthAttachment, 1, GL_FALSE, 0, Utils::GetAccesType(acces), Utils::RynexFBTextureFormatDepthToGLBindImgae(m_DepthAttachmentSpecification.TextureFormat));
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrambufferSize || height > s_MaxFrambufferSize)
		{
			RY_CORE_WARN("Faild Resize frambueffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentsIndex, int x, int y)
	{
		RY_CORE_ASSERT(attachmentsIndex < m_ColorAttachments.size(), "Error OpenGLFramebuffer::ReadPixxel!");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentsIndex);
		int pixeldata;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixeldata);

		return pixeldata;
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		RY_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Error: OpenGLFramebuffer::ClearAttachment!");

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			Utils::RynexFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
#if 0
		m_TextureAttachments[attachmentIndex].reset();
#endif
	}
#if 0
	Ref<Texture> OpenGLFramebuffer::GetTexture(uint32_t index)
	{
		return m_TextureAttachments[index];
	}
#endif
	


}