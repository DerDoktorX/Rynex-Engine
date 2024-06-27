#include "rypch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"


namespace Rynex{

	namespace Utils {

		static GLenum HazelImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
			}

			RY_CORE_ASSERT(false, "Error: Utils::HazelImageFormatToGLDataFormat!");
			return 0;
		}

		static GLenum HazelImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			}

			RY_CORE_ASSERT(false, "Error: Utils::HazelImageFormatToGLInternalFormat!");
			return 0;
		}

	}


	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification), 
		m_Width(m_Specification.Width), 
		m_Heigth(m_Specification.Height)
	{
		m_InternalFormate = Utils::HazelImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormate = Utils::HazelImageFormatToGLDataFormat(m_Specification.Format);

		RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Heigth);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t withe, uint32_t height)
		: m_Width(withe)
		, m_Heigth(height)
	{
		m_InternalFormate = Utils::HazelImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormate = Utils::HazelImageFormatToGLDataFormat(m_Specification.Format);

		RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Heigth);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) RY_CORE_ERROR("filePath->{0}", path.c_str());
		RY_CORE_ASSERT(data, "Faild to load Image!");
		
		
		if (data)
		{
			m_Width = width;
			m_Heigth = height;

			m_IsLoaded = true;

			GLenum internalFormate = 0, dataFormate = 0;
			if (channels == 4) {
				internalFormate = GL_RGBA8;
				dataFormate = GL_RGBA;
			}
			else if (channels == 3) {
				internalFormate = GL_RGB8;
				dataFormate = GL_RGB;
			}

			m_InternalFormate = internalFormate;
			m_DataFormate = dataFormate;

			RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Heigth);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Heigth, m_DataFormate, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		
		//err = fclose(m_stream);
		//int numclosed = _fcloseall();
		//printf("\n\t%i closed files, err state: %i\n", numclosed, err);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormate == GL_RGBA ? 4 : 3;
		RY_CORE_ASSERT(size == m_Width * m_Heigth * bpp, "Data must be entyer Texture!")
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Heigth, m_DataFormate, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}