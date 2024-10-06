#include "rypch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>
#include "Rynex/Renderer/API/Framebuffer.h"


namespace Rynex{

	namespace Utils {

#define RY_CHECK_MULTYSAMPLE(samples) samples > 1

		static GLenum ImageFormatToGLDataFormat(ImageFormat attachmentType)
		{
			switch (attachmentType)
			{
				case ImageFormat::R8:				return GL_RED;
				case ImageFormat::RG8:				return GL_RG;
				case ImageFormat::RGB8:				return GL_RGB;
				case ImageFormat::RGBA8:			return GL_RGBA;
				case ImageFormat::RGBA32F:			return GL_RGBA;
				case ImageFormat::RED_INTEGER:		return GL_RED_INTEGER;
				case ImageFormat::DEPTH24STENCIL8:	return GL_DEPTH_STENCIL_ATTACHMENT;
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLDataFormat!");
			return GL_RGBA;
		}

		static GLenum ImageFormatToGLInternalFormat(ImageFormat interalformat)
		{
			switch (interalformat)
			{
				case ImageFormat::R8:				return GL_R8;
				case ImageFormat::RG8:				return GL_RG8;
				case ImageFormat::RGB8:				return GL_RGB8;
				case ImageFormat::RGBA8:			return GL_RGBA8;
				case ImageFormat::RGBA32F:			return GL_RGBA32F;
				case ImageFormat::RED_INTEGER:		return GL_R32I;
				case ImageFormat::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
				default:
				{
					RY_CORE_ERROR("Error SetTextureWrappingMode whrong TextureWrappingMode!");
					return GL_RGBA8;
				}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLInternalFormat!");
			return GL_RGBA8;
		}

		static uint32_t ImageFormatCahnelsCount(ImageFormat interalformat)
		{
			switch (interalformat)
			{
				case ImageFormat::R8:				return 1;
				case ImageFormat::RG8:				return 2;
				case ImageFormat::RGB8:				return 3;
				case ImageFormat::RGB32F:			return 3;
				case ImageFormat::RGBA8:			return 4;
				case ImageFormat::RGBA32F:			return 4;
				case ImageFormat::RED_INTEGER:		return 1;
				case ImageFormat::DEPTH24STENCIL8:	return 1;
				default:
				{
					RY_CORE_ERROR("Error SetTextureWrappingMode whrong TextureWrappingMode!");
					return GL_RGBA8;
				}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLInternalFormat!");
			return GL_RGBA8;
		}

		static GLenum SetTextureWrappingMode(TextureWrappingMode textureWrapping)
		{
			switch (textureWrapping)
			{
				case TextureWrappingMode::ClampBorder:		return GL_CLAMP_TO_BORDER;
				case TextureWrappingMode::ClampEdge:		return GL_CLAMP_TO_EDGE;
				case TextureWrappingMode::MirrorClampEdge:  return GL_MIRROR_CLAMP_TO_EDGE;
				case TextureWrappingMode::MirrorRepeate:	return GL_MIRRORED_REPEAT;
				case TextureWrappingMode::Repeate:			return GL_REPEAT;
				default:
				{
					RY_CORE_ERROR("Error SetTextureWrappingMode whrong TextureWrappingMode!");
					return GL_REPEAT;
				}
			}
			return GL_REPEAT;
		}

		static GLenum SetTextureWrappingDimension(int index)
		{
			switch (index)
			{
				case 0:	return GL_TEXTURE_WRAP_S;
				case 1:	return GL_TEXTURE_WRAP_T;
				case 2:	return GL_TEXTURE_WRAP_R;
				default: 
				{
					RY_CORE_ASSERT(false, "Error SetTextureWrappingDimension whrong index! not mor than 2!");
					return GL_TEXTURE_WRAP_R;
				}
			}
		}

		static GLenum SetTextureFilteringMode(TextureFilteringMode filteringMode)
		{
			switch (filteringMode)
			{
				case TextureFilteringMode::Linear:	return GL_LINEAR;
				case TextureFilteringMode::Nearest:	return GL_NEAREST;
				default:
				{
					RY_CORE_ERROR("Error SetTextureFiltering whrong TextureFilteringMode!");
					return GL_LINEAR;
				}
			}
		}

		static GLenum SetTexturFilteringDimensionality(int index)
		{
			switch (index)
			{
				case 0:	return GL_TEXTURE_MIN_FILTER;
				case 1:	return GL_TEXTURE_MAG_FILTER;
				default:
				{
					RY_CORE_ASSERT(false, "Error SetTexturFilteringDimensionality whrong index! not mor than 1!");
					return GL_TEXTURE_MAG_FILTER;
				}
			}
		}
		
		static GLenum SetTextureTarget(TextureTarget target, bool multisampled)
		{
			switch (target)
			{
				case TextureTarget::Texture1D:			return GL_TEXTURE_1D;
				case TextureTarget::Texture2D:			
				{
					if (multisampled)	return GL_TEXTURE_2D_MULTISAMPLE;
					else				return GL_TEXTURE_2D;
				}
				case TextureTarget::Texture3D:			return GL_TEXTURE_3D;
				case TextureTarget::TextureRectAngle:	return GL_TEXTURE_RECTANGLE;
				case TextureTarget::TextureBuffer:		return GL_TEXTURE_BUFFER;
				case TextureTarget::TextureCubeMap:		return GL_TEXTURE_CUBE_MAP;
				default:
				{
					RY_CORE_ERROR("Error SetTextureFiltering whrong TextureFilteringMode!");
					return GL_TEXTURE_2D;
				}
			}
		}

		static void CreateTextureWrapping(TextureSpecification spec, uint32_t renderID)
		{
			TextureWrappingMode warping;
			TextureTarget taget = spec.Target;
			bool multysample = RY_CHECK_MULTYSAMPLE(spec.Samples);
			for (int i = 0; i < 3; i++)
			{	
				warping = spec.WrappingSpec[i];
				if ((warping != TextureWrappingMode::None) && (i < 3 || i >= 0))
					glTextureParameteri(renderID, SetTextureWrappingDimension(i), SetTextureWrappingMode(warping));
			}
		}

		static void CreateTextureFiltering(TextureSpecification spec, uint32_t renderID)
		{
			TextureFilteringMode filering = spec.FilteringMode;
			TextureTarget taget = spec.Target;
			if (filering == TextureFilteringMode::None) return;
			for (int i = 0; i < 2; i++)
			{
				glTextureParameteri(renderID, SetTexturFilteringDimensionality(i), SetTextureFilteringMode(filering));
			}
		}

		static void CreateTexture2D(TextureSpecification spec, uint32_t outID, void* data = nullptr)
		{
				int samples = spec.Samples;
				bool multisampled = RY_CHECK_MULTYSAMPLE(samples);
				uint32_t width = spec.Width, height = spec.Height;
				ImageFormat format = spec.Format;
				TextureTarget target = spec.Target;
				if (multisampled)
					glTextureStorage2DMultisample(outID, samples, ImageFormatToGLInternalFormat(format), width, height, GL_FALSE);
				else
					glTextureStorage2D(outID, format == ImageFormat::Depth ? 0 : 1, ImageFormatToGLInternalFormat(format), width, height);
		}

		static bool CreateTexture(TextureSpecification spec, uint32_t* outID, void* data = nullptr)
		{
			int samples = spec.Samples;
			bool multisampled = RY_CHECK_MULTYSAMPLE(samples);
			uint32_t width = spec.Width, height = spec.Height;
			ImageFormat format = spec.Format;
			TextureTarget textureTarget = spec.Target;
			glCreateTextures(SetTextureTarget(textureTarget, multisampled), 1, outID);
			switch (textureTarget)
			{
				case TextureTarget::Nono:	return false;
				case TextureTarget::Texture1D:
				case TextureTarget::Texture2D:
				{
					CreateTexture2D(spec, *outID, data);
					return true;
				}
				case TextureTarget::Texture3D:
				case TextureTarget::TextureRectAngle:
				case TextureTarget::TextureBuffer:
				case TextureTarget::TextureCubeMap:
				case TextureTarget::ImageTexture:
				case TextureTarget::FrameBufferTexture:
				default:
					return false;
			}
			return false;
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




	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification)
		: m_Specification(specification), 
		m_Width(m_Specification.Width), 
		m_Height(m_Specification.Height)
	{
#if 0
		specification.FilteringMode
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
#else
		Invalidate();
#endif

	}

	

	OpenGLTexture::OpenGLTexture(uint32_t withe, uint32_t height)
		: m_Width(withe)
		, m_Height(height)
	{
		RY_PROFILE_FUNCTION();

		m_DataFormate = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
		m_InternalFormate = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
		RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

		m_Specification.Width = m_Width;
		m_Specification.Height = m_Height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture::OpenGLTexture(const std::string& path)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ASSERT(false,"OpenGLTexture::OpenGLTexture-> use string path!");
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) RY_CORE_ERROR("filePath->{0}", path.c_str());
		RY_CORE_ASSERT(data, "Faild to load Image!");
		
		
		if (data)
		{
			m_Width = width;
			m_Height = height;

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

			m_Specification.Width = m_Width;
			m_Specification.Height = m_Height;

			RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormate, GL_UNSIGNED_BYTE, data);

			
		}

		stbi_image_free(data);
		//err = fclose(m_stream);
		//int numclosed = _fcloseall();
		//printf("\n\t%i closed files, err state: %i\n", numclosed, err);
	}

	OpenGLTexture::OpenGLTexture(void* data, int width, int height, int channels)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ASSERT(data,"Error OpenGLTexture2D::OpenGLTexture2D no data confirmed!");
		if (data)
		{
			m_Width = width;
			m_Height = height;

			GLenum internalFormate = 0, dataFormate = 0;
			if (channels == 4) {
				internalFormate = GL_RGBA8;
				dataFormate = GL_RGBA;
			}
			else if (channels == 3) {
				internalFormate = GL_RGB8;
				dataFormate = GL_RGB;
			}
			else if (channels == 2) {
				internalFormate = GL_RG8;
				dataFormate = GL_RG;
			}
			else if (channels == 1) {
				internalFormate = GL_R8;
				dataFormate = GL_RED;
			}
			m_Specification.Width = m_Width;
			m_Specification.Height = m_Height;
			m_InternalFormate = internalFormate;
			m_DataFormate = dataFormate;

			RY_CORE_ASSERT(m_InternalFormate & m_DataFormate, "format not seportet?");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormate, GL_UNSIGNED_BYTE, data);

		}
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& spec, void* data, uint32_t size)
		: m_Specification(spec)
		, m_Width(m_Specification.Width)
		, m_Height(m_Specification.Height)
	{

#if 0
		RY_PROFILE_FUNCTION();
		m_DataFormate = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
		m_InternalFormate = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormate, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
		{
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormate, GL_UNSIGNED_BYTE, data);
		}
#endif
		Invalidate();
		SetData(data, size);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		RY_PROFILE_FUNCTION();
		FreeCurrentData();
		glDeleteTextures(1, &m_RendererID);
		
	}

	void OpenGLTexture::SetData(void* data, uint32_t size)
	{
		RY_PROFILE_FUNCTION();
		uint32_t bpp = Utils::ImageFormatCahnelsCount(m_Specification.Format);
		RY_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entyer Texture!")
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormate, GL_UNSIGNED_BYTE, data);
		RY_CORE_INFO("OpenGLTexture::SetData Set Image Data");
	}

	const std::vector<unsigned char> OpenGLTexture::GetCurrentRenderData()
	{
		FreeCurrentData();

		uint32_t bpp = Utils::ImageFormatCahnelsCount(m_Specification.Format);
		m_Data.resize(m_Width * m_Height * bpp);
		glGetTextureImage(m_RendererID, 0, m_DataFormate, GL_UNSIGNED_BYTE, m_Data.size(), m_Data.data());
		return m_Data;
	}

	void OpenGLTexture::FreeCurrentData()
	{
		if (m_Data.size())
		{
			m_Data.clear();
		}
	}

	void OpenGLTexture::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	

	void OpenGLTexture::BindImage( Acces acces, uint32_t slot) const
	{
		glBindImageTexture(slot, m_RendererID, 0, GL_FALSE, 0, Utils::GetAccesType(acces), m_InternalFormate);
	}

	void OpenGLTexture::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		m_Width = width;
		m_Height = height;
		Invalidate();
	}

	void OpenGLTexture::BindTex(uint32_t renderID, uint32_t slot)
	{
		glBindTextureUnit(slot, renderID);
	}

	void OpenGLTexture::Invalidate(void* data)
	{
		if (m_RendererID)
		{
			glDeleteTextures(1, &m_RendererID);
		}

		// Create Texure
		if(Utils::CreateTexture(m_Specification, &m_RendererID, data))
		{
			m_DataFormate = Utils::ImageFormatToGLDataFormat(m_Specification.Format);
			m_InternalFormate = Utils::ImageFormatToGLInternalFormat(m_Specification.Format);
			Utils::CreateTextureFiltering(m_Specification, m_RendererID);
			Utils::CreateTextureWrapping(m_Specification, m_RendererID);
			RY_CORE_INFO("OpenGLTexture2D Texture Create Finished: {0}", m_RendererID);
		}
		else
		{
			RY_CORE_ERROR("OpenGLTexture2D Faild to Create Texture!");
			glDeleteTextures(1, &m_RendererID);
		}
		FreeCurrentData();
	}


	OpenGLTextureLinking::OpenGLTextureLinking(AssetHandle asseHandle, int index)
		: m_AssetHandle(asseHandle)
		, m_Index(index)
	{
		RY_CORE_INFO("OpenGLTextureLinking Create Finished: {0}!", index);
	}

	OpenGLTextureLinking::~OpenGLTextureLinking()
	{
	}

	const TextureSpecification& OpenGLTextureLinking::GetSpecification() const
	{
#if 0
		FramebufferTextureFormat texFB = m_FramBuffer->GetFramebufferSpecification().Attachments.Attachments[m_Index != -1 ? m_Index : m_Index + 1].TextureFormat;
		TextureSpecification texSpec = TextureSpecification();
		switch (texFB)
		{
		case FramebufferTextureFormat::None:
			texSpec.Format = ImageFormat::None;
			break;
		case FramebufferTextureFormat::RGBA8:
			texSpec.Format = ImageFormat::RGBA8;
			break;
		case FramebufferTextureFormat::RED_INTEGER:
			texSpec.Format = ImageFormat::RED_INTEGER;
			break;
		case FramebufferTextureFormat::DEPTH24STENCIL8:
			texSpec.Format = ImageFormat::DEPTH24STENCIL8;
			break;
		default:
			break;
		}
		return texFB;
#endif
		RY_CORE_ASSERT(false, "not Implemntet!");
		return TextureSpecification();
	}

	uint32_t OpenGLTextureLinking::GetWidth() const
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
		return 0;
	}

	uint32_t OpenGLTextureLinking::GetHeight() const
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
		return 0;
	}

	uint32_t OpenGLTextureLinking::GetRenderID() const
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
		return 0;
	}

	void OpenGLTextureLinking::Bind(uint32_t slot) const
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
	}

	void OpenGLTextureLinking::BindImage(Acces acces, uint32_t slot) const
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
	}

	void OpenGLTextureLinking::SetData(void* data, uint32_t size)
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
	}

	const std::vector<unsigned char> OpenGLTextureLinking::GetCurrentRenderData()
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
		return std::vector<unsigned char>();
	}

	const uint32_t OpenGLTextureLinking::GetCurrentRenderDataSize() const
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
		return 0;
	}

	void OpenGLTextureLinking::FreeCurrentData()
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
	}

	void OpenGLTextureLinking::Resize(uint32_t width, uint32_t height)
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
	}

	void OpenGLTextureLinking::BindTex(uint32_t renderID, uint32_t slot)
	{
		RY_CORE_ASSERT(false, "not Implemntet!");
	}
}