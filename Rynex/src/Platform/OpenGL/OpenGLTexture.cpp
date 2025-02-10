#include "rypch.h"
#include "OpenGLTexture.h"

#include <Platform/OpenGL/OpenGLFramebuffer.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Core/Application.h>
#include <Platform/OpenGL/OpenGLThreadContext.h>

#include <glad/glad.h>

namespace Rynex{

#define RY_CHECK_MULTYSAMPLE(samples) samples > 1

	namespace Utils {

		static GLenum CompareFunction(TextureCompareModes compareMode)
		{


			switch (compareMode)
			{
			

			case TextureCompareModes::Lequal:	return GL_LEQUAL;
			case TextureCompareModes::Always:	return GL_ALWAYS;
			case TextureCompareModes::Gequal:	return GL_GEQUAL;
			case TextureCompareModes::Less:		return GL_LESS;
			case TextureCompareModes::Greater:	return GL_GREATER;
			case TextureCompareModes::Equal:	return GL_EQUAL;
			case TextureCompareModes::Never:	return GL_NEVER;

			case TextureCompareModes::None:
			case TextureCompareModes::Default:
			default:
				RY_CORE_ASSERT(false);
				break;
			}
			return GL_NONE;
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
				case TextureFormat::RGBA16F:			
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

		static GLenum InternalFormat(TextureFormat interalformat)
		{
			switch (interalformat)
			{
				case TextureFormat::R8:					return GL_R8;

				case TextureFormat::RG8:				return GL_RG8;

				case TextureFormat::RGB8:				return GL_RGB8;
				case TextureFormat::RGB16F:				return GL_RGB16F;
				case TextureFormat::RGB32F:				return GL_RGB32F;

				case TextureFormat::RGBA8:				return GL_RGBA8;
				case TextureFormat::RGBA16F:			return GL_RGBA16F;
				case TextureFormat::RGBA32F:			return GL_RGBA32F;

				case TextureFormat::RED_INTEGER:		return GL_R32I;

				case TextureFormat::DepthComp:			return GL_DEPTH_COMPONENT;
				case TextureFormat::DepthComp16:		return GL_DEPTH_COMPONENT16;
				case TextureFormat::DepthComp24:		return GL_DEPTH_COMPONENT24;
				case TextureFormat::DepthComp32:		return GL_DEPTH_COMPONENT32;
				case TextureFormat::DepthComp32F:		return GL_DEPTH_COMPONENT32F;

				case TextureFormat::Depth24Stencil8:	return GL_DEPTH24_STENCIL8;
				case TextureFormat::Depth32FStencil8:	return GL_DEPTH32F_STENCIL8;
				default:
				{
					RY_CORE_ERROR("Error SetTextureWrappingMode whrong TextureWrappingMode!");
					return GL_RGBA8;
				}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLInternalFormat!");
			return GL_RGBA8;
		}

		

		static uint32_t ImageCahnels(TextureFormat interalformat)
		{
			switch (interalformat)
			{
				case TextureFormat::R8:					return 1;

				case TextureFormat::RG8:				return 2;

				case TextureFormat::RGB8:				return 3;
				case TextureFormat::RGB32F:				return 3;

				case TextureFormat::RGBA8:				return 4;
				case TextureFormat::RGBA32F:			return 4;

				case TextureFormat::RED_INTEGER:		return 1;

				case TextureFormat::Depth24Stencil8:	return 1;
				case TextureFormat::Depth32FStencil8:	return 1;
				default:
				{
					RY_CORE_ASSERT(false,"Error SetTextureWrappingMode whrong TextureWrappingMode!");
					return GL_RGBA8;
				}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLInternalFormat!");
			return GL_RGBA8;
		}

		static GLenum WrappingMode(TextureWrappingMode textureWrapping)
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
					RY_CORE_ASSERT(false, "Error SetTextureWrappingMode whrong TextureWrappingMode!");
					return GL_REPEAT;
				}
			}
			return GL_REPEAT;
		}

		static GLenum WrappingDimension(int index)
		{
			switch (index)
			{
				case 0:	return GL_TEXTURE_WRAP_S;
				case 1:	return GL_TEXTURE_WRAP_T;
				case 2:	return GL_TEXTURE_WRAP_R;
				default: 
				{
					RY_CORE_ASSERT(false, "Error WrappingDimension whrong index! not mor than 2!");
					return GL_TEXTURE_WRAP_R;
				}
			}
		}

		static GLenum FilteringMode(TextureFilteringMode filteringMode)
		{
			switch (filteringMode)
			{
				case TextureFilteringMode::Linear:	return GL_LINEAR;
				case TextureFilteringMode::Nearest:	return GL_NEAREST;
				case TextureFilteringMode::LinearMidmapLinear:	return GL_NEAREST_MIPMAP_LINEAR;
				case TextureFilteringMode::LinearMidmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
				default:
				{
					RY_CORE_ASSERT(false, "Error SetTextureFiltering whrong TextureFilteringMode!");
					return GL_LINEAR;
				}
			}
		}

		static GLenum FilteringDimensionality(int index)
		{
			switch (index)
			{
				case 0:	return GL_TEXTURE_MIN_FILTER;
				case 1:	return GL_TEXTURE_MAG_FILTER;
				default:
				{
					RY_CORE_ASSERT(false, "Error FilteringDimensionality whrong index! not mor than 1!");
					return GL_TEXTURE_MAG_FILTER;
				}
			}
		}
		
		static GLenum TexTarget(TextureTarget target, bool multisampled)
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
					RY_CORE_ASSERT(false, "Error SetTextureFiltering whrong TextureFilteringMode!");
					return GL_TEXTURE_2D;
				}
			}
		}

		static void TextureWrapping(TextureSpecification spec, uint32_t renderID)
		{
			TextureWrappingMode warping;
			TextureTarget taget = spec.Target;
			bool multysample = RY_CHECK_MULTYSAMPLE(spec.Samples);
			for (int i = 0; i < 3; i++)
			{	
				warping = spec.WrappingSpec[i];
				if ((warping != TextureWrappingMode::None) && (i < 3 || i >= 0))
					glTextureParameteri(renderID, WrappingDimension(i), WrappingMode(warping));
			}
		}

		static void TextureFiltering(TextureSpecification spec, uint32_t renderID)
		{
			TextureFilteringMode filering = spec.FilteringMode;
			TextureCompareModes compareModes = spec.Compare;
			TextureTarget taget = spec.Target;
			if(compareModes == TextureCompareModes::Lequal)
			{
				glTextureParameteri(renderID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTextureParameteri(renderID, GL_TEXTURE_COMPARE_FUNC, CompareFunction(compareModes));
			}

			if (filering == TextureFilteringMode::None) 
				return;

			for (int i = 0; i < 2; i++)
			{
				glTextureParameteri(renderID, FilteringDimensionality(i), FilteringMode(filering));
			}
		}
		
		static void Texture2DValue(uint32_t id, int samples, TextureTarget target, int midmap, TextureFormat format, uint32_t width, uint32_t height, void* data = nullptr)
		{
			GLenum type;
			GLenum formatGL;
			GLenum internalFormat;
			GLenum targetGL = GL_TEXTURE_2D;
			switch (format)
			{
			
			case TextureFormat::RG8:
			{
				type = GL_UNSIGNED_BYTE;
				formatGL = GL_RGB;
				internalFormat = GL_RG8;
				break;
			}
			case TextureFormat::RGB8:
			{
				type = GL_UNSIGNED_BYTE;
				formatGL = GL_RGB;
				internalFormat = GL_RGB8;
				break;
			}
			case TextureFormat::RGBA8:
			{
				type = GL_UNSIGNED_BYTE;
				formatGL = GL_RGBA;
				internalFormat = GL_RGBA8;
				break;
			}
			
			
			case TextureFormat::RGBA32F:
			{
				type = GL_UNSIGNED_BYTE;
				formatGL = GL_RGBA;
				internalFormat = GL_RGBA32F;
				break;
			}

			case TextureFormat::RED_INTEGER:
			{
				type = GL_UNSIGNED_BYTE;
				formatGL = GL_RED_INTEGER;
				internalFormat = GL_R32I;
				break;
			}
			case TextureFormat::Depth24Stencil8:
			{
				
				// 
				type = GL_FLOAT;
				formatGL = GL_DEPTH_STENCIL;
				internalFormat = GL_DEPTH24_STENCIL8;
				glTexStorage2D(targetGL, 1, internalFormat, width, height);
				return;
			}
			case TextureFormat::Depth32FStencil8:
			{
				// glTexStorage2D(targetGL, 1, InternalFormat(format), width, height);
				// return;
				formatGL = GL_DEPTH_STENCIL;
				internalFormat = GL_DEPTH32F_STENCIL8;
				type = GL_FLOAT;
				break;
			}
			case TextureFormat::DepthComp:
			{
				// glTexStorage2D(targetGL, 1, InternalFormat(format), width, height);
				// return;
				formatGL = GL_DEPTH_COMPONENT;
				internalFormat = GL_DEPTH_COMPONENT;
				type = GL_FLOAT;
				break;
			}
			case TextureFormat::DepthComp16:
			{
				// glTexStorage2D(targetGL, 1, InternalFormat(format), width, height);
				// return;
				formatGL = GL_DEPTH_COMPONENT;
				internalFormat = GL_DEPTH_COMPONENT16;
				type = GL_FLOAT;
				break;
			}
			case TextureFormat::DepthComp24:
			{
				// glTexStorage2D(targetGL, 1, InternalFormat(format), width, height);
				// return;
				formatGL = GL_DEPTH_COMPONENT;
				internalFormat = GL_DEPTH_COMPONENT24;
				type = GL_FLOAT;
				break;
			}
			case TextureFormat::DepthComp32:
			{
				// glTexStorage2D(targetGL, 1, InternalFormat(format), width, height);
				// return;
				formatGL = GL_DEPTH_COMPONENT;
				internalFormat = GL_DEPTH_COMPONENT32;
				type = GL_FLOAT;
				break;
			}
			case TextureFormat::DepthComp32F:
			{
				// glTexStorage2D(TexTarget(target, samples > 1), 1, InternalFormat(format), width, height);
				// return;
				formatGL = GL_DEPTH_COMPONENT;
				internalFormat = GL_DEPTH_COMPONENT32F;
				type = GL_FLOAT;
				break;
			}

			default:
				RY_CORE_ASSERT(false);
				return;
			}
			
			GLint midmapLevel = midmap;
			GLenum border = 0;
			
			glTexImage2D(targetGL, midmapLevel, internalFormat, width, height, 0, formatGL, type, data);
		}

		static void CreateTexture2D(TextureSpecification spec, uint32_t outID, void* data = nullptr)
		{
			int samples = spec.Samples;
			bool multisampled = RY_CHECK_MULTYSAMPLE(samples);
			uint32_t width = spec.Width, height = spec.Height;
			TextureFormat format = spec.Format;
			TextureTarget target = spec.Target;
			if (multisampled)
			{
				glTexImage2DMultisample(TexTarget(target, multisampled), samples, InternalFormat(format), width, height, GL_FALSE);
			}
			else
			{
				Texture2DValue(outID, samples, target, 0, format, width, height, data);
			}
		}

		static bool CreateTexture(TextureSpecification spec, uint32_t* outID, void* data = nullptr)
		{
			int samples = spec.Samples;
			bool multisampled = RY_CHECK_MULTYSAMPLE(samples);
			uint32_t width = spec.Width, height = spec.Height;
			TextureFormat format = spec.Format;
			TextureTarget textureTarget = spec.Target;
			GLenum target = TexTarget(textureTarget, multisampled);
			glCreateTextures(target, 1, outID);
			glBindTexture(target, *outID);
			switch (textureTarget)
			{
				// case TextureTarget::None:	return false;
				// case TextureTarget::Texture1D: break;
				case TextureTarget::Texture2D:
				{
					CreateTexture2D(spec, *outID, data);
					return true;
				}
				// case TextureTarget::Texture3D:
				// case TextureTarget::TextureRectAngle:
				// case TextureTarget::TextureBuffer:
				// case TextureTarget::TextureCubeMap:
				// case TextureTarget::ImageTexture:
				// case TextureTarget::FrameBufferTexture:
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



#pragma region Texture


	OpenGLTexture::OpenGLTexture()
	{
	}

	OpenGLTexture::OpenGLTexture(const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{
		
			Invalidate(nullptr);
	

	}

	
#if 0

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
#endif

	OpenGLTexture::OpenGLTexture(const TextureSpecification& spec, void* data, uint32_t size)
		: m_Specification(spec)
		, m_Width(spec.Width)
		, m_Height(spec.Height)
	{

		m_Data.resize(size);
		std::memcpy(m_Data.data(), data, m_Data.size());
		
		if (m_Data.size() == m_Width * m_Height * Utils::ImageCahnels(m_Specification.Format))
		{
			Invalidate(m_Data.data());
		}
		else
		{
			RY_CORE_ASSERT(false);
			Invalidate(nullptr);
		}
	
		
	}

	

	OpenGLTexture::OpenGLTexture(const TextureSpecification& spec, std::vector<unsigned char>&& data)
		: m_Specification(spec)
		, m_Width(spec.Width)
		, m_Height(spec.Height)
		, m_Data(std::move(data))
	{
		uint32_t bpp = Utils::ImageCahnels(m_Specification.Format);
		RY_CORE_ASSERT(m_Data.size() != 0 && m_Width * m_Height * bpp == m_Data.size());
		Invalidate(m_Data.data());
	}

	OpenGLTexture::~OpenGLTexture()
	{
		RY_PROFILE_FUNCTION();
		FreeCurrentData();
		if(m_RendererID)
		{
			glDeleteTextures(1, &m_RendererID);
		}
		
	}

	void OpenGLTexture::InitAsync()
	{
		
	}

	void OpenGLTexture::SetData(void* data, uint32_t size)
	{
		RY_PROFILE_FUNCTION();
		uint32_t bpp = Utils::ImageCahnels(m_Specification.Format);
		RY_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entyer Texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormate, GL_UNSIGNED_BYTE, data);
		RY_CORE_INFO("OpenGLTexture::SetData Set Image Data");
	}

	const std::vector<unsigned char> OpenGLTexture::GetCurrentRenderData()
	{
		FreeCurrentData();

		uint32_t bpp = Utils::ImageCahnels(m_Specification.Format);
		m_Data.resize(m_Width * m_Height * bpp);
		glGetTextureImage(m_RendererID, 0, m_DataFormate, GL_UNSIGNED_BYTE, m_Data.size(), m_Data.data());
		return m_Data;
	}

	void OpenGLTexture::FreeCurrentData()
	{
		if (m_Data.size())
		{
			m_Data.clear();
			m_Data.shrink_to_fit();
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
		Invalidate(nullptr);
	}

	void OpenGLTexture::BindTex(uint32_t renderID, uint32_t slot)
	{
		glBindTextureUnit(slot, renderID);
	}

	void OpenGLTexture::Invalidate(void* data)
	{
		if (OpenGLThreadContext::IsActive())
		{
			TexFilter& filter = m_Specification.FilteringMode;
			if (filter == TexFilter::Default && !m_Specification.GenerateMips)
				filter = m_Specification.GenerateMips ? TexFilter::LinearMidmapLinear : TexFilter::Linear;

			TexFrom& fromat = m_Specification.Format;
			if (fromat == TexFrom::Default)
				fromat = TexFrom::RGBA8;

			TextureWrappingMode& warpT = m_Specification.WrappingSpec.T;
			if (warpT == TexWarp::Default)
				warpT = TexWarp::Repeate;

			TextureWrappingMode& warpR = m_Specification.WrappingSpec.R;
			if (warpR == TexWarp::Default)
				warpR = TexWarp::Repeate;

			TextureWrappingMode& warpS = m_Specification.WrappingSpec.S;
			if (warpS == TexWarp::Default)
				warpS = TexWarp::Repeate;

			if (m_RendererID)
			{
				glDeleteTextures(1, &m_RendererID);
				m_RendererID = 0;
			}

			// Create Texure
			if (Utils::CreateTexture(m_Specification, &m_RendererID, data))
			{
				m_DataFormate = Utils::FormatData(m_Specification.Format);
				m_InternalFormate = Utils::InternalFormat(m_Specification.Format);
				if (!(m_Specification.Samples > 1))
				{
					
					Utils::TextureFiltering(m_Specification, m_RendererID);
					Utils::TextureWrapping(m_Specification, m_RendererID);

				}
				// RY_CORE_INFO("OpenGLTexture2D Texture Create Finished: {0}", m_RendererID);
				// glBindTexture(Utils::TexTarget(m_Specification.Target, m_Specification.Samples > 1), 0);

			}
			else
			{
				RY_CORE_ASSERT(false, "OpenGLTexture2D Faild to Create Texture!");
				glDeleteTextures(1, &m_RendererID);
			}
		}
		else
		{
			Application::Get().SubmiteToMainThreedQueue([this, data]() {
				Invalidate(data);
				});
		}
	}

	

#if 0
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
		case FramebufferTextureFormat::Depth24Stencil8:
			texSpec.Format = ImageFormat::Depth24Stencil8;
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
#endif

#pragma endregion


#pragma region FrameTextures

#if 0
	OpenGLFrameTexture::OpenGLFrameTexture( const TextureSpecification& specification)
		: m_Specification(specification)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
	{

		
		
	}

	OpenGLFrameTexture::~OpenGLFrameTexture()
	{
		FreeCurrentData();
		if(!m_RendererID)
			glDeleteTextures(1, &m_RendererID);

	}
#endif
	OpenGLFrameTexture::OpenGLFrameTexture(const TextureSpecification& specification)
		: OpenGLTexture()
	{
		m_Specification = specification;
		m_Width = m_Specification.Width;
		m_Height = m_Specification.Height;


	}

	OpenGLFrameTexture::~OpenGLFrameTexture()
	{
	}

	void OpenGLFrameTexture::Resize(uint32_t width, uint32_t height)
	{
		RY_CORE_ERROR("You Cant Resize a single Image From Frambuffer you need to Resize the Frambuffer and this Texture will Resize too!");
	}

	void OpenGLFrameTexture::Invalidate(const TextureSpecification& specification, uint32_t redererID)
	{
		m_Specification = specification;
		m_Width = m_Specification.Width;
		m_Height = m_Specification.Height;
		Invalidate(redererID);
	}

	void OpenGLFrameTexture::Invalidate(uint32_t redererID)
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());

		m_RendererID = redererID;
		bool multySample = m_Specification.Samples > 1;
		m_DataFormate = Utils::FormatData(m_Specification.Format);
		m_InternalFormate = Utils::InternalFormat(m_Specification.Format);
		GLenum target = Utils::TexTarget(m_Specification.Target, multySample);


		glBindTexture(target, m_RendererID);
		RY_CORE_INFO("Resize TextureFrame to {0}, {1}", m_Width, m_Height);
		switch (m_Specification.Target)
		{
		case TextureTarget::Texture2D:
		{
			Utils::CreateTexture2D(m_Specification, m_RendererID, nullptr);
			//if(m_Specification.Format != TextureFormat::Depth24Stencil8 )
			//	glTexImage2D(target, 0, m_InternalFormate, m_Width, m_Height, 0, m_DataFormate, GL_UNSIGNED_BYTE, nullptr);
			//else
			//	glTexStorage2D(GL_TEXTURE_2D, 1, m_InternalFormate, m_Width, m_Height);
			break;
		}
		// case TextureTarget::Texture1D: 
		// case TextureTarget::Texture3D:
		// case TextureTarget::TextureRectAngle:
		// case TextureTarget::TextureBuffer:
		// case TextureTarget::TextureCubeMap:
		// case TextureTarget::ImageTexture:
		// case TextureTarget::FrameBufferTexture:
		default:
			break;
		}
			
		if (!multySample)
		{
			Utils::TextureFiltering(m_Specification, m_RendererID);
			Utils::TextureWrapping(m_Specification, m_RendererID);

			// glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			// glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			// glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			// glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}	
	}

	void OpenGLFrameTexture::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteTextures(1, &m_RendererID);
			m_RendererID = 0;
		}
		glCreateTextures(Utils::TexTarget(m_Specification.Target, m_Specification.Samples > 1), 1, &m_RendererID);
		if(m_RendererID)
			Invalidate(m_RendererID);
	}

#pragma endregion
	
}