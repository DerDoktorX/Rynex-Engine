#include "rypch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"


namespace Rynex{

	namespace Utils {

		static GLenum ImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLDataFormat!");
			return 0;
		}

		static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB8:  return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLInternalFormat!");
			return 0;
		}


	}


	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification), 
		m_Width(m_Specification.Width), 
		m_Heigth(m_Specification.Height)
	{
		RY_PROFILE_FUNCTION();
		m_InternalFormate = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormate = Utils::ImageFormatToGLDataFormat(m_Specification.Format);

		RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Heigth);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, uint32_t rendererID)
		: m_Specification(specification),
		m_Width(m_Specification.Width),
		m_Heigth(m_Specification.Height),
		m_RendererID(rendererID)
	{
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t withe, uint32_t height)
		: m_Width(withe)
		, m_Heigth(height)
	{
		RY_PROFILE_FUNCTION();
		m_InternalFormate = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormate = Utils::ImageFormatToGLDataFormat(m_Specification.Format);

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
		RY_PROFILE_FUNCTION();
		RY_CORE_ERROR("OpenGLTexture2D::OpenGLTexture2D-> use string path!");
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

			
		}

		stbi_image_free(data);
		//err = fclose(m_stream);
		//int numclosed = _fcloseall();
		//printf("\n\t%i closed files, err state: %i\n", numclosed, err);
	}

	OpenGLTexture2D::OpenGLTexture2D(void* data, int width, int height, int channels)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_INFO("OpenGLTexture2D::OpenGLTexture2D -> use data");
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

		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec, void* data)
		: m_Specification(spec)
		, m_Width(m_Specification.Width)
		, m_Heigth(m_Specification.Height)
		, m_IsLoaded(true)
	{
		RY_PROFILE_FUNCTION();
		m_DataFormate = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
		m_InternalFormate = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Heigth);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
		{
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Heigth, m_DataFormate, GL_UNSIGNED_BYTE, data);
		}
		
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		RY_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		RY_PROFILE_FUNCTION();
		uint32_t bpp = m_DataFormate == GL_RGBA ? 4 : 3;
		RY_CORE_ASSERT(size == m_Width * m_Heigth * bpp, "Data must be entyer Texture!")
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Heigth, m_DataFormate, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		RY_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::BindTex(uint32_t renderID, uint32_t slot)
	{
		RY_PROFILE_FUNCTION();
		glBindTextureUnit(slot, renderID);
	}
}