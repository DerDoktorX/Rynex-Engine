#include "rypch.h"
#include "OpenGLTexture.h"

#include <Rynex/Core/Application.h>

#include <Platform/OpenGL/OpenGLBase.h>
#include <Platform/OpenGL/OpenGLFramebuffer.h>
#include <Platform/OpenGL/OpenGLBindlesTextureArray.h>

namespace Rynex{



	namespace Utils {
		
		template<typename T>
		constexpr void CheckSpecifaictionValuesAorB(T& value, T setValueA, T setValueB, bool onReplace, T dafaultValue = T::Default)
		{
			if (dafaultValue == value)
			{
				value = onReplace ? setValueA : setValueB;
				RY_CORE_WARN("{}: Is now a {} (otherwise {}), but it the out come can differz between APIs!", 
					magic_enum::enum_name<T>(dafaultValue), 
					magic_enum::enum_name<T>(onReplace ? setValueA : setValueB), 
					magic_enum::enum_name<T>(!onReplace ? setValueA : setValueB)
				);
			}
		}

		template<typename T>
		static void CheckSpecifaictionValues(T& value, T setValue, T dafaultValue = T::Default)
		{
			if (dafaultValue == value)
			{
				value = setValue;
				RY_CORE_WARN("{}: Is now a {}, but it the out come can differz between APIs!", 
					magic_enum::enum_name<T>(dafaultValue), 
					magic_enum::enum_name<T>(setValue)
				);
			}
		}

#if 0
		static void AddMemoryToTracker(uint64_t memory)
		{
			Application& app = Application::Get();
			Window& window = app.GetWindow();
			GraphicsContext* conext = window.GetGraphicsContext();
			OpenGLContext* OpenGLconext = static_cast<OpenGLContext*>(conext);
			OpenGLconext->AddCurentAllocMemory(memory);
		}

		static void RemoveMemoryToTracker(uint64_t memory)
		{
			Application& app = Application::Get();
			Window& window = app.GetWindow();
			GraphicsContext* conext = window.GetGraphicsContext();
			OpenGLContext* OpenGLconext = static_cast<OpenGLContext*>(conext);
			OpenGLconext->RemoveCurentAllocMemory(memory);
		}
#endif

		static uint32_t ImageChanelsBytes(TextureFormat interalformat)
		{
			switch (interalformat)
			{
			case TextureFormat::R8:					return 1 * 1;

			case TextureFormat::RG8:				return 1 * 2;

			case TextureFormat::S_RGB8:				return 1 * 3;
			case TextureFormat::RGB8:				return 1 * 3;
			case TextureFormat::RGB16F:				return 2 * 3;
			case TextureFormat::RGB32F:				return 4 * 3;


			case TextureFormat::S_RGBA8:			return 1 * 4;
			case TextureFormat::RGBA8:				return 1 * 4;
			case TextureFormat::RGBA16F:			return 2 * 4;
			case TextureFormat::RGBA32F:			return 4 * 4;

			case TextureFormat::RED_INTEGER:		return 4 * 1;

			case TextureFormat::Depth24Stencil8:	return 3 * 1;
			case TextureFormat::Depth32FStencil8:	return 4 * 1;

			// case TextureFormat::DepthComp:			return 1 * 1;
			case TextureFormat::DepthComp16:		return 1 * 2;
			case TextureFormat::DepthComp24:		return 1 * 3;
			case TextureFormat::DepthComp32:		return 1 * 3;
			case TextureFormat::DepthComp32F:		return 1 * 4;

			default:
			{
				RY_CORE_ASSERT(false, "Error  Utils::ImageCahnelsBytes whrong TextureFormat!");
				return 1 * 4;
			}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageCahnelsBytes!");
			return 1 * 4;
		}

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

				case TextureFormat::S_RGB8:
				case TextureFormat::RGB8:	
				case TextureFormat::RGB16F:
				case TextureFormat::RGB32F:
					return GL_RGB;


				case TextureFormat::S_RGBA8:

				case TextureFormat::RGBA8:				
				case TextureFormat::RGBA16F:			
				case TextureFormat::RGBA32F:			
					return GL_RGBA;

				case TextureFormat::RED_INTEGER:		
					return GL_RED_INTEGER;

				// case TextureFormat::DepthComp:			
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


				case TextureFormat::S_RGB8:				return GL_SRGB8;
				case TextureFormat::S_RGBA8:			return GL_SRGB8_ALPHA8;


				case TextureFormat::RED_INTEGER:		return GL_R32I;

				// case TextureFormat::DepthComp:			return GL_DEPTH_COMPONENT;
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

		static uint32_t GetIndexDimensionFromTextureTagetForTextureArray(TextureTarget target)
		{
			switch (target)
			{
			case TextureTarget::Texture1D_Array:
				return 1;
			case TextureTarget::Texture2D_Array:
				return 2;
			case TextureTarget::TextureCubeMap_Array:
				return 2;
			case TextureTarget::None:
			case TextureTarget::Default:
			case TextureTarget::Texture1D:
			case TextureTarget::Texture2D:
			case TextureTarget::Texture3D:
			case TextureTarget::TextureRectAngle:
			case TextureTarget::TextureBuffer:
			case TextureTarget::TextureCubeMap:
			default:
				RY_CORE_ASSERT(false, "Wrong Texture Target Fomrate! For Texture Array!");
				return MAXUINT32;
			}
		}

#if 0
		static uint64_t GetMemoryBytsFromTextur(const TextureSpecification& spec)
		{
			uint64_t sizePixel = Utils::ImageChanelsBytes(spec.Format);
			uint64_t midmapsSize = std::pow(2, spec.GenerateMips);
			uint64_t width = spec.Width / midmapsSize;
			uint64_t heigth = spec.Height / midmapsSize;

			RY_CORE_ASSERT(spec.Samples != 0);
			uint64_t samples = spec.Samples;

			uint64_t byteSize = width * heigth * sizePixel * samples;
			return byteSize;
		}
#endif

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
					return 4;
				}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLInternalFormat!");
			return 4;
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
#if 0
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
#endif


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
#if 0
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
#endif
		static GLenum TexTarget(TextureTarget target, bool multisampled)
		{
			switch (target)
			{
				case TextureTarget::Texture1D:		
				{
					
					RY_CORE_WARN_IF(!multisampled,"We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_1D;
				}
				case TextureTarget::Texture2D:			
				{
					if (multisampled)	return GL_TEXTURE_2D_MULTISAMPLE;
					else				return GL_TEXTURE_2D;
				}
				case TextureTarget::Texture3D:	
				{
					RY_CORE_WARN_IF(!multisampled,"We have no multisampled for a {}", magic_enum::enum_name(target));
						// ::Rynex::Log::Get().GetCoreLogger()->warn("We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_3D;
				}
				case TextureTarget::TextureRectAngle:
				{
					RY_CORE_WARN_IF(!multisampled, "We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_RECTANGLE;
				}
				case TextureTarget::TextureBuffer:		
				{
					RY_CORE_WARN_IF(!multisampled, "We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_BUFFER;
				}
				case TextureTarget::Texture1D_Array:
				{
					RY_CORE_WARN_IF(!multisampled, "We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_1D_ARRAY;
				}
				case TextureTarget::Texture2D_Array:
				{
					if (multisampled)	
						return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
					else				
						return GL_TEXTURE_2D_ARRAY;
				}
				case TextureTarget::TextureCubeMap_Array:
				{
					RY_CORE_WARN_IF(!multisampled, "We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_CUBE_MAP_ARRAY;
				}
				case TextureTarget::TextureCubeMap:		
				{
					RY_CORE_WARN_IF(!multisampled, "We have no multisampled for a {}", magic_enum::enum_name(target));
					return GL_TEXTURE_CUBE_MAP;
				}
				default:
				{
					RY_CORE_ASSERT(false, "Error SetTextureFiltering whrong TextureFilteringMode!");
					return GL_TEXTURE_2D;
				}
			}
		}
#if 0
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
			// case TextureFormat::DepthComp:
			// {
			// 	// glTexStorage2D(targetGL, 1, InternalFormat(format), width, height);
			// 	// return;
			// 	formatGL = GL_DEPTH_COMPONENT;
			// 	internalFormat = GL_DEPTH_COMPONENT;
			// 	type = GL_FLOAT;
			// 	break;
			// }
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
				uint32_t width = spec.Width, height = spec.Height;
				glTexImage2DMultisample(TexTarget(target, multisampled), samples, InternalFormat(format), width, height, GL_FALSE);
			}
			else
			{
				Texture2DValue(outID, samples, target, spec.GenerateMips, format, width, height, data);
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
			

			RY_GRAFIC_CREATE(*outID, OpenGLTexture);
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
#endif

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

	
#pragma region OpenglStorageTexture

#define RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION 1



	OpenGLTextureStorageModern::OpenGLTextureStorageModern(const TextureSpecification& spec)
		: m_Object(
#if RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION
			OpenGLTextureObject(
				OpenGLTextureObject::ObjectTypeOpenGL(GL_TEXTURE_2D, GL_RGBA8, GL_RGBA, 0u, 1u),
				OpenGLTextureObject::DimensionOpenGL(1u, 1u, 1u)
			)
#endif
		)
		, m_Sampler(
#if RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION
			OpenGLTextureSampler(
				OpenGLTextureSampler::FilterOpenGL(GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST),
				OpenGLTextureSampler::WarpOpenGL(GL_REPEAT, GL_REPEAT, GL_REPEAT),
				OpenGLTextureSampler::CompareOpenGL(GL_NONE, GL_LEQUAL),
				OpenGLTextureSampler::LodOpenGL(-1000.0f, 1000.0f, 0.0f, 1.0f),
				std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
			)
#endif

		)

		, m_Specification(spec)
		, m_RendererIDTex(0u)
		, m_RendererIDSam(0u)
		, m_BindlesActive(false)
		, m_BindlesHandle(0ull)
	{
		SetupeSpecficationVaribels();
		Invalidate();
		
	}

	OpenGLTextureStorageModern::OpenGLTextureStorageModern(const TextureSpecification& spec, void* dataPtr, uint32_t size)
		: m_Object(
#if RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION

			OpenGLTextureObject(
				OpenGLTextureObject::ObjectTypeOpenGL(GL_TEXTURE_2D, GL_RGBA8, GL_RGBA, 0u, 1u),
				OpenGLTextureObject::DimensionOpenGL(1u, 1u, 1u)
			)
		
#endif
		)
		, m_Sampler(
#if RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION
			OpenGLTextureSampler(
				OpenGLTextureSampler::FilterOpenGL(GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST),
				OpenGLTextureSampler::WarpOpenGL(GL_REPEAT, GL_REPEAT, GL_REPEAT),
				OpenGLTextureSampler::CompareOpenGL(GL_NONE, GL_LEQUAL),
				OpenGLTextureSampler::LodOpenGL(-1000.0f, 1000.0f, 0.0f, 1.0f),
				std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
			)
#endif
		)
		, m_Specification(spec)
		, m_RendererIDTex(0u)
		, m_RendererIDSam(0u)
		, m_BindlesActive(false)
		, m_BindlesHandle(0ull)
	{
		SetupeSpecficationVaribels();
		uint32_t byteSize = GetByteSize();

		RY_CORE_ASSERT(size <= byteSize, "not Vaild Byte Size, There are more data then in Texture Buffer fit in!");
		constexpr uint8_t dafoultPixel = 0x00;
		m_Data.resize(byteSize, dafoultPixel);
		uint8_t* dataBytePtr = reinterpret_cast<uint8_t*>(dataPtr);

		CopyTexData(dataBytePtr, size);
		
		Invalidate();
	}

	OpenGLTextureStorageModern::OpenGLTextureStorageModern(const TextureSpecification& spec, std::vector<unsigned char>&& data)
		: m_Object(
#if RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION
			OpenGLTextureObject(
				OpenGLTextureObject::ObjectTypeOpenGL(GL_TEXTURE_2D, GL_RGBA8, GL_RGBA, 0u, 1u),
				OpenGLTextureObject::DimensionOpenGL(1u, 1u, 1u)
			)
#endif
		)
		, m_Sampler(
#if RY_ENABLE_OPENGL_TEXTURE_STORAGE_MODERN_PRE_ARG_DEFNITION
			OpenGLTextureSampler(
				OpenGLTextureSampler::FilterOpenGL(GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST),
				OpenGLTextureSampler::WarpOpenGL(GL_REPEAT, GL_REPEAT, GL_REPEAT),
				OpenGLTextureSampler::CompareOpenGL(GL_NONE, GL_LEQUAL),
				OpenGLTextureSampler::LodOpenGL(-1000.0f, 1000.0f, 0.0f, 1.0f),
				std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
			)
#endif
		)
		, m_Specification(spec)
		, m_RendererIDTex(0u)
		, m_RendererIDSam(0u)
		, m_BindlesActive(false)
		, m_BindlesHandle(0ull)
		, m_Data(std::move(data))
	{
		SetupeSpecficationVaribels();
		Invalidate();
	}

	OpenGLTextureStorageModern::~OpenGLTextureStorageModern()
	{		
		OnDestroyCall();
		DestroyID();
	}

	void OpenGLTextureStorageModern::BindLessTex()
	{
		RY_CORE_ASSERT(m_BindlesActive, "Bindles Handle Is alrdy Aktve");
		RY_CORE_ASSERT(0u != m_RendererIDTex);

#if RY_ENABLE_BINDLES_TEXTURE
		RY_CORE_ASSERT(0u != m_BindlesHandle, "Bindles Handle Handle Invaild!");
		glMakeTextureHandleResidentARB(m_BindlesHandle);
		GL_CHECK_LOOP();
#endif
		m_BindlesActive = true;
	}

	void OpenGLTextureStorageModern::UnBindLessTex()
	{
		RY_CORE_ASSERT(m_BindlesActive, "Bindles Handle Is was not befor Aktve!");
		RY_CORE_ASSERT(0u != m_RendererIDTex);

#if RY_ENABLE_BINDLES_TEXTURE
		RY_CORE_ASSERT(0u != m_BindlesHandle, "Bindles Handle Handle Invaild!");
		glMakeTextureHandleNonResidentARB(m_BindlesHandle);
		GL_CHECK_LOOP();
#endif
		m_BindlesActive = false;
	}

	

	void OpenGLTextureStorageModern::SetData(void* dataPtr, uint32_t size)
	{
		uint8_t* dataBytePtr = reinterpret_cast<uint8_t*>(dataPtr);
		CopyTexData(dataBytePtr, size);
		uint32_t byteSize = GetByteSize();
		if (m_RendererIDTex == 0)
		{
			Invalidate();
		}
		else
		{
			m_Object.SetData(m_RendererIDTex, m_Data, byteSize);
			OnDataChangeCall();
		}
	}

	const std::vector<unsigned char> OpenGLTextureStorageModern::GetCurrentRenderData()
	{
		
		uint32_t pbb = Utils::ImageChanelsBytes(m_Specification.Format);
		m_Object.GetData(m_RendererIDTex, m_Data, pbb);
		return m_Data;
	}

	const std::vector<unsigned char> OpenGLTextureStorageModern::GetCurrentRenderData(TextureFormat format)
	{
		uint32_t glFomrat = Utils::FormatData(format);
		uint32_t pbb = Utils::ImageChanelsBytes(format);
		m_Object.GetData(m_RendererIDTex, m_Data, pbb, glFomrat);
		return m_Data;
	}

	void OpenGLTextureStorageModern::FreeCurrentData()
	{
		m_Data.clear();
	}

	bool OpenGLTextureStorageModern::IsTransferd()
	{
		return m_FanceObject.IsTransfered();
	}

	void OpenGLTextureStorageModern::Bind(uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);

#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindTextureSlot(slot, m_RendererIDTex);
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		RY_CORE_ASSERT(0u != m_RendererIDSam);
		OpenGLRenderCommand::BindSamplerSlot(slot, m_RendererIDSam);
#endif
#else
		RY_CORE_ASSERT(!m_BindlesActive, "Bindles Handle Is alrady Active!");
		glBindTextureUnit(slot, m_RendererIDTex);
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		glBindSampler(slot, m_RendererIDSam);
#endif
#endif
		GL_CHECK_LOOP();
	}

	void OpenGLTextureStorageModern::BindImage(Acces acces, uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);

		RY_CORE_ASSERT(!m_BindlesActive, "Bindles Handle Is alrady Active!");
		const auto& objectType = m_Object.GetObjectType();
		glBindImageTexture(slot, m_RendererIDTex, 0, GL_FALSE, 0, Utils::GetAccesType(acces), objectType.interleFormat);
	}

	void OpenGLTextureStorageModern::UnBindImage(Acces acces, uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);

		RY_CORE_ASSERT(!m_BindlesActive, "Bindles Handle Is alrady Active!");
		const auto& objectType = m_Object.GetObjectType();
		glBindImageTexture(slot, 0, 0, GL_FALSE, 0, Utils::GetAccesType(acces), objectType.interleFormat);
		GL_CHECK_LOOP();
	}

	void OpenGLTextureStorageModern::UnBind(uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);
		RY_CORE_ASSERT(!m_BindlesActive, "Bindles Handle Is alrady Active!");
#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindTextureSlot(slot, 0u);
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		RY_CORE_ASSERT(0u != m_RendererIDSam);
		OpenGLRenderCommand::BindSamplerSlot(slot, 0u);
#endif
#else
		glBindTextureUnit(slot, 0);
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		glBindSampler(slot, 0);
#endif
#endif
		GL_CHECK_LOOP();
	}

	void OpenGLTextureStorageModern::Resize1D(uint32_t width)
	{
		switch (m_Specification.Target)
		{
		case TextureTarget::Texture1D:
			break;
		case TextureTarget::Texture2D:
		case TextureTarget::Texture3D:
		{
			RY_CORE_TRACE("We handle Resize only in in withe the over dimension we don't tuch!");
			break;
		}
		case TextureTarget::TextureCubeMap:
		{
			RY_CORE_WARN("We handle this like a from a CubeMap like Resize2D and use the withe as in put for withe and Heigth!");
			m_Specification.Height = width;
			break;
		}

		case TextureTarget::TextureBuffer:
		case TextureTarget::TextureRectAngle:
		default:
			RY_CORE_ASSERT(false);
			return;
		}
		m_Specification.Width = width;

		SetupeSpecficationVaribels();


		m_Data.clear();
		Invalidate();
	}

	void OpenGLTextureStorageModern::Resize2D(uint32_t width, uint32_t height)
	{
		
		switch (m_Specification.Target)
		{
		case TextureTarget::Texture1D:
		{
			RY_CORE_ERROR("Texture1D has not enugh Dimensions! for Resize2D! abourt!");
			return;
		}
		case TextureTarget::Texture2D:
		case TextureTarget::TextureCubeMap:

		case TextureTarget::Texture3D:
		{
			RY_CORE_TRACE("We handle Resize only in in withe / heigth the over dimension we don't tuch!");
			break;
		}

		case TextureTarget::TextureBuffer:
		case TextureTarget::TextureRectAngle:
		default:
			RY_CORE_ASSERT(false);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		SetupeSpecficationVaribels();

		m_Data.clear();
		Invalidate();
	}

	void OpenGLTextureStorageModern::Resize3D(uint32_t width, uint32_t height, uint32_t depth)
	{
		const TexTar& targetSpec = m_Specification.Target;
		switch (targetSpec)
		{		
		case TextureTarget::Texture2D:
		case TextureTarget::Texture1D:
		{
			RY_CORE_ERROR("{} has not enugh Dimensions! for Resize3D! abourt!", magic_enum::enum_name(targetSpec));
			return;
		}
		case TextureTarget::TextureCubeMap:
		{
			RY_CORE_ERROR("TextureTarget::TextureCubeMap has enugh Dimensions but Dimensions depth need every time to be of 6! for Resize3D! abourt! use only Resize2D and Resize1D for Cubemaps!", magic_enum::enum_name(targetSpec));
			return;
		}
		case TextureTarget::Texture3D:
			break;

		case TextureTarget::TextureBuffer:
		case TextureTarget::TextureRectAngle:
		default:
			RY_CORE_ASSERT(false);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		m_Specification.Depth = depth;

		SetupeSpecficationVaribels();

		m_Data.clear();
		Invalidate();
	}

	void OpenGLTextureStorageModern::ClearImage()
	{
		m_Object.ClearImage(m_RendererIDTex, nullptr, GL_UNSIGNED_BYTE);
		OnDataChangeCall();
	}

	

	void OpenGLTextureStorageModern::ClearImage(int data)
	{
		glm::ivec4 value4 = glm::ivec4(data, 0.0f, 0.0f, 0.0f);
		ClearImage(value4);
	}

	

	void OpenGLTextureStorageModern::ClearDepth()
	{
		char dataBite[8] = { 
			0, 0, 0, 0, 
			0, 0, 0, 0,
		};
		GLenum type = 0;

		switch (m_Specification.Format)
		{
		case TextureFormat::DepthComp16:
		{
			type = GL_UNSIGNED_SHORT;
			GLushort clearValue = 1u;
			GLushort* dataPtr = reinterpret_cast<GLushort*>(dataBite);
			*dataPtr = clearValue;
			break;
		}
		case TextureFormat::DepthComp24:
		{
			type = GL_UNSIGNED_INT;
			GLuint clearValue = 1u;
			GLuint* dataPtr = reinterpret_cast<GLuint*>(dataBite);
			*dataPtr = clearValue;
			break;
		}
		case TextureFormat::DepthComp32:
		{
			type = GL_INT;
			int clearValue = 1;
			int* dataPtr = reinterpret_cast<int*>(dataBite);
			*dataPtr = clearValue;
			break;
		}
		case TextureFormat::DepthComp32F:
		{
			type = GL_FLOAT;
			float clearValue = 1.0f;
			float* dataPtr = reinterpret_cast<float*>(dataBite);
			*dataPtr = clearValue;
			break;
		}

		case TextureFormat::Depth24Stencil8:
		{
			type = GL_UNSIGNED_INT_24_8;
			GLuint clearValue = 1u;
			GLuint* dataPtr = reinterpret_cast<GLuint*>(dataBite);
			*dataPtr = clearValue;
			break;
		}
		case TextureFormat::Depth32FStencil8:
		{
			struct ClearLaout32FStencil8
			{
				float depth;
				GLuint stencil;
			};

			type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
			ClearLaout32FStencil8 clearValue = { 1.0f, 0 };
			ClearLaout32FStencil8* dataPtr = reinterpret_cast<ClearLaout32FStencil8*>(dataBite);
			*dataPtr = clearValue;
			break;
		}
		default:
			RY_CORE_ASSERT(false, "Not Vaild Depth Fromat");
			break;
		}
		

		m_Object.ClearImage(m_RendererIDTex, &dataBite, type);
	}

	



	void OpenGLTextureStorageModern::ClearImage(const glm::vec3& data)
	{
		glm::vec4 valu4 = glm::vec4(data, 0.0f);
		ClearImage(valu4);
	}

	void OpenGLTextureStorageModern::ClearImage(const glm::vec4& data)
	{
		m_Object.ClearImage(m_RendererIDTex, glm::value_ptr(data), GL_FLOAT);
		OnDataChangeCall();
	}

	void OpenGLTextureStorageModern::ClearImage(const glm::ivec4& data)
	{
		m_Object.ClearImage(m_RendererIDTex, glm::value_ptr(data), GL_INT);
		OnDataChangeCall();
	}

	void OpenGLTextureStorageModern::ClearImage(const glm::uvec4& data)
	{
		m_Object.ClearImage(m_RendererIDTex, glm::value_ptr(data), GL_UNSIGNED_INT);
		OnDataChangeCall();
	}



	void OpenGLTextureStorageModern::SetSpecfication(const TextureSpecification& spec, OpenGLFramebuffer* frambufferPtr)
	{
		
		if (spec == m_Specification)
			return;
		m_Specification = spec;
		m_ParentVec.SetLoopJump(frambufferPtr); 
		SetupeSpecficationVaribels(); 
		m_Data.clear();
		Invalidate();
	}



	void OpenGLTextureStorageModern::AddParent(OpenGLBindlesTextureArray* openGlBindlesTexPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		Ref<OpenGLBindlesTextureArray> openGLBindlesTextureArrayRef = Asset::GetRefInPlaceType(openGlBindlesTexPtr);
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (std::visit(
				[openGLBindlesTextureArrayRef](auto& openGLTextureParentWeek)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLBindlesTextureArray>>)
						return openGLTextureParentWeek.lock() == openGLBindlesTextureArrayRef;
					return false;
				}, openGLTextureParentVarientWeek)
			)
				return;
		}
		m_ParentVec.emplace_back(openGLBindlesTextureArrayRef);
#else
		m_ParentVec.Set(openGlBindlesTexPtr);
#endif
	}

	void OpenGLTextureStorageModern::RemoveParent(OpenGLBindlesTextureArray* openGlBindlesTextureArrayPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		uint32_t index = 0u;
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (std::visit(
				[openGlBindlesTextureArrayPtr](auto& openGLTextureParentWeek)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLBindlesTextureArray>>)
					{
						Ref<OpenGLBindlesTextureArray> openGLBindlesTextureArray = openGLTextureParentWeek.lock();
						if (nullptr == openGLBindlesTextureArray)
							return nullptr == openGLBindlesTextureArray || openGlBindlesTextureArrayPtr == openGLBindlesTextureArray.get();
					}
					return false;
				}, openGLTextureParentVarientWeek)
				)
			{
				break;
			}
			index++;
		}
		if (index == m_ParentVec.size())
			return;

		m_ParentVec.erase(m_ParentVec.begin() + index);
#else
		m_ParentVec.Remove(openGlBindlesTextureArrayPtr);
#endif

	}

	void OpenGLTextureStorageModern::AddParent(OpenGLLinkedTextureArray* openGlLinkedTextureArrayPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		Ref<OpenGLLinkedTextureArray> openGLOpenGLLinkedTextureArrayRef = Asset::GetRefInPlaceType(openGlLinkedTextureArrayPtr);
		
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (std::visit(
				[openGLOpenGLLinkedTextureArrayRef](auto& openGLTextureParentWeek)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLLinkedTextureArray>>)
						return openGLTextureParentWeek.lock() == openGLOpenGLLinkedTextureArrayRef;
					return false;
				}, openGLTextureParentVarientWeek)
				)
			{
				return;
			}
		}
		m_ParentVec.emplace_back(openGLOpenGLLinkedTextureArrayRef);
#else
		m_ParentVec.Set(openGlLinkedTextureArrayPtr);
#endif

	}

	void OpenGLTextureStorageModern::RemoveParent(OpenGLLinkedTextureArray* openGlLinkedTextureArrayPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		uint32_t index = 0u;
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (std::visit(
				[openGlLinkedTextureArrayPtr](auto& openGLTextureParentWeek)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLLinkedTextureArray>>)
					{
						Ref<OpenGLLinkedTextureArray> linkedTextureArray = openGLTextureParentWeek.lock();
						if (nullptr == linkedTextureArray)
							return nullptr == linkedTextureArray || openGlLinkedTextureArrayPtr == linkedTextureArray.get();
					}
					return false;
				}, openGLTextureParentVarientWeek)
				)
			{
				break;
			}
			index++;
		}
		if (index == m_ParentVec.size())
			return;

		m_ParentVec.erase(m_ParentVec.begin() + index);
#else
		m_ParentVec.Remove(openGlLinkedTextureArrayPtr);
#endif
	}

	void OpenGLTextureStorageModern::AddParent(OpenGLFramebuffer* openGlFramebufferPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		Ref<OpenGLFramebuffer> openGLOpenGLFramebufferRef = Asset::GetRefInPlaceType(openGlFramebufferPtr);

		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (std::visit(
				[openGLOpenGLFramebufferRef](auto& openGLTextureParentWeek)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLFramebuffer>>)
					{
						Ref<OpenGLFramebuffer> openGlFramebufferRef = openGLTextureParentWeek.lock();
						return openGLOpenGLFramebufferRef == openGLTextureParentWeek.lock();
					}
					return false;
				}, openGLTextureParentVarientWeek)
				)
			{
				return;
			}
		}
		m_ParentVec.emplace_back(openGLOpenGLFramebufferRef);
#else
		m_ParentVec.Set(openGlFramebufferPtr);
#endif

	}

	void OpenGLTextureStorageModern::RemoveParent(OpenGLFramebuffer* openGlFramebufferPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		uint32_t index = 0u;
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (std::visit(
				[openGlFramebufferPtr](auto& openGLTextureParentWeek)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLFramebuffer>>)
					{
						Ref<OpenGLFramebuffer> linkedTextureArray = openGLTextureParentWeek.lock();
						if (nullptr == linkedTextureArray)
							return nullptr == linkedTextureArray || openGlFramebufferPtr == linkedTextureArray.get();
					}
					return false;
				}, openGLTextureParentVarientWeek)
				)
			{
				break;
			}
			index++;
		}
		if (index == m_ParentVec.size())
			return;

		m_ParentVec.erase(m_ParentVec.begin() + index);
#else
		m_ParentVec.Remove(openGlFramebufferPtr);
#endif
	}

	uint32_t OpenGLTextureStorageModern::GetByteSize() const
	{
		const auto& dimension = m_Object.GetDimension();
		uint32_t pixelCount = dimension.withe;
		pixelCount *= dimension.heigth;
		pixelCount *= dimension.depth;

		const uint32_t bytePerPixel = Utils::ImageChanelsBytes(m_Specification.Format);
		const uint32_t byteSize = pixelCount * bytePerPixel;
		return byteSize;
	}

	void OpenGLTextureStorageModern::SetupeSpecficationVaribels()
	{
		Utils::CheckSpecifaictionValuesAorB(m_Specification.FilteringMode, 
			TextureFilteringMode::LinearMidmapLinear, TextureFilteringMode::Linear,
			m_Specification.GenerateMips != 0);
		Utils::CheckSpecifaictionValues(m_Specification.Target, TextureTarget::Texture2D);

		Utils::CheckSpecifaictionValues(m_Specification.Format, TextureFormat::RGBA8);

		Utils::CheckSpecifaictionValues(m_Specification.WrappingSpec.T, TextureWrappingMode::Repeate);
		Utils::CheckSpecifaictionValues(m_Specification.WrappingSpec.R, TextureWrappingMode::Repeate);
		Utils::CheckSpecifaictionValues(m_Specification.WrappingSpec.S, TextureWrappingMode::Repeate);

		m_Object.SetObjectType({
			Utils::TexTarget(m_Specification.Target, m_Specification.Samples > 1),
			Utils::FormatData(m_Specification.Format),
			Utils::InternalFormat(m_Specification.Format),
			m_Specification.GenerateMips,
			m_Specification.Samples
		});
		m_Object.SetDimension({
			m_Specification.Width,
			m_Specification.Height,
			m_Specification.Depth
		});

		m_Sampler.SetFilter({
			Utils::FilteringMode(m_Specification.FilteringMode),
			Utils::FilteringMode(m_Specification.FilteringMode)
		});
		m_Sampler.SetWarp({
			Utils::WrappingMode(m_Specification.WrappingSpec.S),
			Utils::WrappingMode(m_Specification.WrappingSpec.R),
			Utils::WrappingMode(m_Specification.WrappingSpec.T)
		});
		
		bool comapreUse = m_Specification.Compare == TexComp::None;
		m_Sampler.SetCompare(
			OpenGLTextureSampler::CompareOpenGL(
				comapreUse ? GL_NONE : GL_COMPARE_REF_TO_TEXTURE,
				comapreUse ? GL_LEQUAL : Utils::CompareFunction(m_Specification.Compare)
			)
		);
		constexpr std::array<float, 4> borderColorArray = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_Sampler.SetBorderColor(borderColorArray);
	}

	void OpenGLTextureStorageModern::CopyTexData(uint8_t* data, uint32_t size, uint32_t offset)
	{
		uint32_t byteSize = size + offset;
		RY_CORE_ASSERT(byteSize <= m_Data.size(), "Overfolwe!");
		if (byteSize < m_Data.size() && 0u != byteSize && 0u != offset)
		{
			RY_CORE_WARN("On Textures is it not so Comenen too use a offset!");
		}
		else if (byteSize < m_Data.size() && 0u != byteSize && 0u == offset)
		{
			RY_CORE_WARN("On Textures is it not so Comenen to use texture Data not for the complet Image!");
		}

		uint8_t* dataPtr = m_Data.data() + offset;
		std::memcpy(dataPtr, data, size);
	}

	void OpenGLTextureStorageModern::CreateID(uint32_t target)
	{
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererIDTex);
		RY_OPENGL_TEXTURE_ID_SCOPE_LOCK();
		glCreateTextures(target, 1, &m_RendererIDTex);

#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		RY_CORE_ASSERT(0u == m_RendererIDSam);
		glDeleteSamplers(1, &m_RendererIDSam);
		GL_CHECK();
#endif

	}

	void OpenGLTextureStorageModern::DestroyID()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());

		if (m_BindlesActive)
			UnBindLessTex();

		RY_OPENGL_TEXTURE_ID_SCOPE_LOCK();

#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		if (0u != m_RendererIDSam)
		{
			glDeleteSamplers(1, &m_RendererIDSam);
			m_RendererIDSam = 0u;
			GL_CHECK();
		}
#endif	
		if (0u != m_RendererIDTex)
		{
			glDeleteTextures(1, &m_RendererIDTex);
			m_RendererIDTex = 0u;
			GL_CHECK();
		}
		m_BindlesActive = 0ull;
	}

	void OpenGLTextureStorageModern::Invalidate()
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLTextureStorageModern::Invalidate, this));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLTextureStorageModern::Invalidate);
#endif
		

		const OpenGLTextureObject::ObjectTypeOpenGL& type = m_Object.GetObjectType();
		CreateID(type.target);


		if (m_Data.empty())
		{
			m_Object.Create(m_RendererIDTex);

			uint32_t byteSize = GetByteSize();
			m_Data.resize(byteSize);
		}
		else
		{
			m_Object.Create(m_RendererIDTex, m_Data, GetByteSize());
			if (0 < type.levelMidMaps)
			{
				glGenerateMipmap(m_RendererIDTex);
			}
		}
#if RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER
			const uint32_t& samplerRenderID = m_RendererIDTex;
#else
			const uint32_t& samplerRenderID = m_RendererIDSam;
#endif

		if(1u < type.samples)
		{
			m_Sampler.CreateSampler(samplerRenderID);
		}
		else
		{

			m_Sampler.Create(samplerRenderID);
		}



		OnSpecifcationChangeCall();
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		m_BindlesHandle = glGetTextureHandleARB(m_RendererIDTex);
#endif
		m_BindlesActive = false;

	}

	void OpenGLTextureStorageModern::SetupFance()
	{ 
#if 0
		m_FanceObject.SetupFence();
#else
		if (Asset::CurrentOnMainThread())
			return;

		m_FanceObject.SetupFence();
		uint32_t byteSize = m_Data.size();
		int64_t pastTime = m_FanceObject.StoppThreadUntlieLoded();
		RY_CORE_INFO("Data OpenGLArrayBuffer Transfered! off {} bytesComplet. Time waiting {} Nanosec", byteSize, pastTime);
		m_FanceObject.DestroyID();
		ClearLocaleDataStore();
#endif
	}

	void OpenGLTextureStorageModern::ClearLocaleDataStore()
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
	}

	
	

	uint32_t OpenGLTextureStorageModern::GetOpenGLTextureTarget() const
	{
		const OpenGLTextureObject::ObjectTypeOpenGL& objectType = m_Object.GetObjectType();
		uint32_t target = objectType.target;
		return target;
	}

	uint32_t OpenGLTextureStorageModern::GetOpenGLTextureDataFormate() const
	{
		const OpenGLTextureObject::ObjectTypeOpenGL& objectType = m_Object.GetObjectType();
		uint32_t formate = objectType.formate;
		return formate;
	}

	bool OpenGLTextureStorageModern::CopyFromTextureToTexture(const Ref<OpenGLTextureStorageModern>& textureSrc)
	{
		glm::uvec3 size = {
			textureSrc->m_Specification.Width,
			textureSrc->m_Specification.Height,
			textureSrc->m_Specification.Depth,
		};
		return CopyFromTextureToTexture(textureSrc, size);
	}

	bool OpenGLTextureStorageModern::CopyFromTextureToTexture(const Ref<OpenGLTextureStorageModern>& textureSrc, const glm::uvec3& size)
	{
		glm::uvec3 offsetDst = {
			0u,
			0u,
			0u,
		};

		glm::uvec3 offsetSrc = {
			0u,
			0u,
			0u,
		};
		return CopyFromTextureToTexture(textureSrc, size, offsetSrc, offsetDst);
	}

	bool OpenGLTextureStorageModern::CopyFromTextureToTexture(const Ref<OpenGLTextureStorageModern>& textureSrc, const glm::uvec3& size, const glm::uvec3& offsetSrc, const glm::uvec3& offsetDst)
	{
		bool result = m_Object.CopyFromTextureToTexture(
			textureSrc->m_RendererIDTex, m_RendererIDTex
			, textureSrc->m_Object
			, size, offsetSrc, offsetDst
		);
	
		RY_CORE_ASSERT(result, "Texture Copy Faild!");
		return result;
	}


	void OpenGLTextureStorageModern::ChangedDataFromParent(OpenGLFramebuffer* openGlFramebufferPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		for (const auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (
				std::visit([this, openGlFramebufferPtr](auto& openGLTextureParentWeek)
					{
						if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLFramebuffer>>)
						{
							Ref<OpenGLFramebuffer> openGLBindlesTextureArrayRef = openGLTextureParentWeek.lock();

							return openGLBindlesTextureArrayRef.get() == openGlFramebufferPtr;
						}
						return false;
					}
					, openGLTextureParentVarientWeek)
				)
			{
				OnDataChangeCall();
				return;
			}
		}
#else

		if(m_ParentVec.Has(openGlFramebufferPtr))
			OnDataChangeCall();
		else
#endif
		RY_CORE_ERROR("OpenGLTextureStorageModern dident find his parent!");
	}

	void OpenGLTextureStorageModern::ChangedSpecifcationFromParent(OpenGLFramebuffer* openGlFramebufferPtr)
	{
#ifndef RY_USE_PRENT_LINKE
		Ref<OpenGLFramebuffer> openGLBindlesTextureArrayRef = Asset::GetRefInPlaceType(openGlFramebufferPtr);

		for (const auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			if (
				std::visit([this, openGLBindlesTextureArrayRef](auto& openGLTextureParentWeek)
					{
						if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLFramebuffer>>)
						{
							return openGLBindlesTextureArrayRef == openGLTextureParentWeek.lock();
						}
						return false;
					}
					, openGLTextureParentVarientWeek)
				)
			{
				OnSpecifcationChangeCall();
				return;
			}
		}
#else

		if (m_ParentVec.Has(openGlFramebufferPtr))
			OnSpecifcationChangeCall();
		else
#endif
			RY_CORE_ERROR("OpenGLTextureStorageModern dident find his parent!");

	}

	void OpenGLTextureStorageModern::OnSpecifcationChangeCall()
	{
#ifndef RY_USE_PRENT_LINKE

		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			std::visit([this](auto& openGLTextureParentWeek) {
				if (auto openGLTextureParentRef = openGLTextureParentWeek.lock())
				{
					openGLTextureParentRef->OnChildeSpecifcationChange(this);
				}
			}, openGLTextureParentVarientWeek);
		}
#else

		m_ParentVec.ForEche([this](auto& openGLTextureParentWeek) {
				if (auto openGLTextureParentRef = openGLTextureParentWeek.lock())
				{
					if constexpr (!std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLFramebuffer>>)
					{
						openGLTextureParentRef->OnChildeSpecifcationChange(this);
					}
				}
		});
#endif

	}

	void OpenGLTextureStorageModern::OnDataChangeCall()
	{
#ifndef RY_USE_PRENT_LINKE
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			std::visit([this](auto& openGLTextureParentWeek)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLLinkedTextureArray>>)
				{
					if (Ref<OpenGLLinkedTextureArray> openGLLinkedTextureArray = openGLTextureParentWeek.lock())
						openGLLinkedTextureArray->OnChildeDataChange(this);
				}
				
			}, openGLTextureParentVarientWeek);
		}
#else

		m_ParentVec.ForEche([this](auto& openGLTextureParentWeek) {
			if (auto openGLTextureParentRef = openGLTextureParentWeek.lock())
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLLinkedTextureArray>>)
				{
					if (Ref<OpenGLLinkedTextureArray> openGLLinkedTextureArray = openGLTextureParentWeek.lock())
						openGLLinkedTextureArray->OnChildeDataChange(this);
				}
			}
		});
#endif
	}

	void OpenGLTextureStorageModern::OnDestroyCall()
	{
#ifndef RY_USE_PRENT_LINKE
		for (auto& openGLTextureParentVarientWeek : m_ParentVec)
		{
			std::visit([this](auto& openGLTextureParentWeek)
			{
				if (auto openGLLinkedTextureArray = openGLTextureParentWeek.lock())
				{
					openGLLinkedTextureArray->OnChildeDestroy(this);
				}
			}, openGLTextureParentVarientWeek);
		}
		m_ParentVec.clear();
#else
		m_ParentVec.ForEche([this](auto& openGLTextureParentWeek) {
			if constexpr (std::is_same_v<std::decay_t<decltype(openGLTextureParentWeek)>, Weak<OpenGLLinkedTextureArray>>)
			{
				if (Ref<OpenGLLinkedTextureArray> openGLLinkedTextureArray = openGLTextureParentWeek.lock())
					openGLLinkedTextureArray->OnChildeDestroy(this);
			}
		});
		m_ParentVec.Clear();
#endif

	}

#pragma endregion


#pragma region OpenGLLinkedTextureArray

#if 1
	OpenGLLinkedTextureArray::OpenGLLinkedTextureArray(const TextureSpecification& spec)
		: m_LinkedTexturesVec()
		, m_ChangedTextureInidicesVec()
		, m_Data()

		, m_FanceObject()
		, m_Object()
		, m_Sampler()

		, m_Specification(spec)
		, m_TextureIndexDimension(5u)
		, m_RendererIDSam(0u)
		, m_RendererIDTex(0u)
		, m_DataUpdatedGPU(true)
	{
		SetupeSpecficationVaribels();
		Invalidate();
	}

	OpenGLLinkedTextureArray::~OpenGLLinkedTextureArray()
	{
		DestroyID();
	}

	void OpenGLLinkedTextureArray::SetupeSpecficationVaribels()
	{
		Utils::CheckSpecifaictionValuesAorB(m_Specification.FilteringMode,
			TextureFilteringMode::LinearMidmapLinear, TextureFilteringMode::Linear,
			m_Specification.GenerateMips != 0);
		Utils::CheckSpecifaictionValues(m_Specification.Target, TextureTarget::Texture2D_Array);
		Utils::CheckSpecifaictionValues(m_Specification.Target, TextureTarget::TextureCubeMap_Array, TextureTarget::TextureCubeMap);
		Utils::CheckSpecifaictionValues(m_Specification.Target, TextureTarget::Texture2D_Array, TextureTarget::Texture2D);
		Utils::CheckSpecifaictionValues(m_Specification.Target, TextureTarget::Texture1D_Array, TextureTarget::Texture1D);
		m_TextureIndexDimension = Utils::GetIndexDimensionFromTextureTagetForTextureArray(m_Specification.Target);

		Utils::CheckSpecifaictionValues(m_Specification.Format, TextureFormat::RGBA8);

		Utils::CheckSpecifaictionValues(m_Specification.WrappingSpec.T, TextureWrappingMode::Repeate);
		Utils::CheckSpecifaictionValues(m_Specification.WrappingSpec.R, TextureWrappingMode::Repeate);
		Utils::CheckSpecifaictionValues(m_Specification.WrappingSpec.S, TextureWrappingMode::Repeate);

		m_Object.SetObjectType({
			Utils::TexTarget(m_Specification.Target, m_Specification.Samples > 1),
			Utils::FormatData(m_Specification.Format),
			Utils::InternalFormat(m_Specification.Format),
			m_Specification.GenerateMips,
			m_Specification.Samples
			});
		m_Object.SetDimension({
			m_Specification.Width,
			m_Specification.Height,
			m_Specification.Depth
			});

		m_Sampler.SetFilter({
			Utils::FilteringMode(m_Specification.FilteringMode),
			Utils::FilteringMode(m_Specification.FilteringMode)
			});
		m_Sampler.SetWarp({
			Utils::WrappingMode(m_Specification.WrappingSpec.S),
			Utils::WrappingMode(m_Specification.WrappingSpec.R),
			Utils::WrappingMode(m_Specification.WrappingSpec.T)
			});

		bool comapreUse = m_Specification.Compare == TexComp::None;
		m_Sampler.SetCompare(
			OpenGLTextureSampler::CompareOpenGL(
				comapreUse ? GL_NONE : GL_COMPARE_REF_TO_TEXTURE,
				comapreUse ? GL_LEQUAL : Utils::CompareFunction(m_Specification.Compare)
			)
		);
		constexpr std::array<float, 4> borderColorArray = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_Sampler.SetBorderColor(borderColorArray);
	}


	void OpenGLLinkedTextureArray::OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex)
	{

		m_DataUpdatedGPU = false;
		int index = 0;
		int count = 0;
		for (Weak<Texture>& textureWeak : m_LinkedTexturesVec)
		{
			if (Ref<Texture> texture = textureWeak.lock())
			{
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
				if (texutureStorage.get() == ptrTex)
				{
					ChangeTextureIndex(index);
					count++;
				}
			}
			index++;
		}
		if (count == 0)
			RY_CORE_ERROR("OpenGLLinkedTextureArray dident find his children!");


		RY_CORE_WARN("This Texture is maby no longer vaild");
	}

	void OpenGLLinkedTextureArray::OnChildeDataChange(OpenGLTextureStorageModern* ptrTex)
	{
		int index = 0;
		int count = 0;
		for (Weak<Texture>& textureWeak : m_LinkedTexturesVec)
		{
			if (Ref<Texture> texture = textureWeak.lock())
			{
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
				if(texutureStorage.get() == ptrTex)
				{
					ChangeTextureIndex(index);
					count++;
				}
			}
			index++;

		}
		if(count == 0)
			RY_CORE_ERROR("OpenGLLinkedTextureArray dident find his children!");
	}

	void OpenGLLinkedTextureArray::OnChildeDestroy(OpenGLTextureStorageModern* ptrTex)
	{
		int index = 0;
		int count = 0;

		for (Weak<Texture>& textureWeak : m_LinkedTexturesVec)
		{
			if (Ref<Texture> texture = textureWeak.lock())
			{
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
				if (texutureStorage.get() == ptrTex)
				{
					RemovedTextureIndexFromChangeVec(index);
					count++;
				}
			}
			else
			{
				RemovedTextureIndexFromChangeVec(index);
				count++;
			}
			index++;

		}

		if (count == 0)
			RY_CORE_ERROR("OpenGLLinkedTextureArray dident find his children!");

	}

	bool OpenGLLinkedTextureArray::HasTextureMoreThenOnce(const Ref<Texture>& texture)
	{
		uint32_t index = 0;
		uint32_t count = 0;
		for (Weak<Texture>& textureWeak : m_LinkedTexturesVec)
		{
			if (Ref<Texture> textureRef = textureWeak.lock())
			{
				if (textureRef == texture)
					count++;
			}
			index++;
		}
		return 1u < count;
	}

	uint32_t OpenGLLinkedTextureArray::GetPerTextureByteSize() const
	{
		uint32_t pbb = Utils::ImageChanelsBytes(m_Specification.Format);
		uint32_t byteSize = m_Specification.Width * m_Specification.Height * m_Specification.Depth * pbb;
		return byteSize;
	}
	

	void OpenGLLinkedTextureArray::ChangeTextureIndex(int index)
	{
		using It = std::vector<int>::iterator;
		It end = m_ChangedTextureInidicesVec.end();
		It pos = std::lower_bound(m_ChangedTextureInidicesVec.begin(), end, index);
		if (pos != end && *pos._Ptr == index)
			return;
		m_ChangedTextureInidicesVec.insert(pos, index);
		m_DataUpdatedGPU = false;
	}

	void OpenGLLinkedTextureArray::RemovedTextureIndexFromChangeVec(int index)
	{
		using It = std::vector<int>::iterator;
		It end = m_ChangedTextureInidicesVec.end();
		It pos = std::lower_bound(m_ChangedTextureInidicesVec.begin(), end, index);
		if (pos == end || *pos._Ptr != index)
			return;
		m_ChangedTextureInidicesVec.erase(pos);
	}

	bool OpenGLLinkedTextureArray::IsTransferd()
	{
		return m_FanceObject.IsTransfered();
	}

	void OpenGLLinkedTextureArray::Bind(uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);
		OpenGLRenderCommand::BindTextureSlot(slot, m_RendererIDTex);
	}

	void OpenGLLinkedTextureArray::BindImage(Acces access, uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);
		const auto& objectType = m_Object.GetObjectType();
		constexpr GLint texLevel = 0;
		constexpr GLboolean layered = GL_FALSE;
		constexpr GLint texLayered = 0;
		const GLenum accessGL = Utils::GetAccesType(access);
		const GLenum interalFomrat = objectType.interleFormat;
		glBindImageTexture(slot, m_RendererIDTex, texLevel, layered, texLayered, accessGL, interalFomrat);

	}

	void OpenGLLinkedTextureArray::UnBindImage(Acces acces, uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);
		const auto& objectType = m_Object.GetObjectType();
		constexpr uint32_t renderID = 0u;
		glBindImageTexture(slot, renderID, 0, GL_FALSE, 0, Utils::GetAccesType(acces), objectType.interleFormat);
	}

	void OpenGLLinkedTextureArray::UnBind(uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != m_RendererIDTex);
		OpenGLRenderCommand::BindTextureSlot(slot, 0u);
	}


	void OpenGLLinkedTextureArray::SetTextureToArray(int index, const Ref<Texture>& texture, bool instandGPUploade)
	{
		RY_CORE_ASSERT(index < m_LinkedTexturesVec.size());
		const TextureSpecification& spec = texture->GetSpecification();
		bool specNotEqual = !m_Specification.IsEqualWitheOutTexturTarget(spec);
		bool specTarget1DtEqual = (m_Specification.Target == TextureTarget::Texture1D_Array && spec.Target == TextureTarget::Texture1D);
		bool specTarget2DEqual = (m_Specification.Target == TextureTarget::Texture2D_Array && spec.Target == TextureTarget::Texture2D);
		bool specTargetCubeMapEqual = (m_Specification.Target == TextureTarget::TextureCubeMap_Array && spec.Target == TextureTarget::TextureCubeMap);
		bool notVaildTargetSet = !(specTarget1DtEqual || specTarget2DEqual || specTargetCubeMapEqual);
		if (notVaildTargetSet || specNotEqual)
		{
			RY_CORE_ERROR("Specifction are not equal!");
		}
		Weak<Texture>& textureIndexWeak = m_LinkedTexturesVec.at(index);
		if (Ref<Texture>& textureIndex = textureIndexWeak.lock())
		{
			if (textureIndex != texture)
			{
				if (!HasTextureMoreThenOnce(textureIndex))
				{
					Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(textureIndex);
					texutureStorage->RemoveParent(this);
				}
			}
			else
			{
				return;
			}
		}
		m_LinkedTexturesVec.at(index) = texture;
		if (instandGPUploade)
			UpdateDataGPU();
		else
			ChangeTextureIndex(index);
		Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		texutureStorage->AddParent(this);
	}

	void OpenGLLinkedTextureArray::SetTextureToArray(int index, std::nullptr_t, bool instandGPUploade)
	{
		RY_CORE_ASSERT(index < m_LinkedTexturesVec.size());

		Weak<Texture>& textureIndexWeak = m_LinkedTexturesVec.at(index);
		if (Ref<Texture>& textureIndex = textureIndexWeak.lock())
		{
			if (!HasTextureMoreThenOnce(textureIndex))
			{
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(textureIndex);
				texutureStorage->RemoveParent(this);
			}
		}
		m_LinkedTexturesVec.at(index).reset();
		if (instandGPUploade)
			UpdateDataGPU();
		else
			RemovedTextureIndexFromChangeVec(index);
	}

	const Ref<Texture>& OpenGLLinkedTextureArray::GetTextureToArray(int index)
	{
		return m_LinkedTexturesVec.at(index).lock();
	}

	void OpenGLLinkedTextureArray::ResizeTextureArray(int size, bool instandGPUploade)
	{
		m_LinkedTexturesVec.resize(size);

		if (instandGPUploade)
			Invalidate();
	}

	const std::vector<Weak<Texture>>& OpenGLLinkedTextureArray::GetTextureVec() const
	{
		return m_LinkedTexturesVec;
	}

	uint32_t OpenGLLinkedTextureArray::GetTextureCount() const
	{
		return m_LinkedTexturesVec.size();
	}

	void OpenGLLinkedTextureArray::ClearTextures()
	{
		for (Weak<Texture>& textureWeak : m_LinkedTexturesVec)
		{
			if (Ref<Texture> texture = textureWeak.lock())
			{
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
				texutureStorage->RemoveParent(this);
			}
		}
		m_LinkedTexturesVec.clear();
	}


	void OpenGLLinkedTextureArray::CreateID(uint32_t target)
	{	
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererIDTex);

		RY_OPENGL_TEXTURE_ID_SCOPE_LOCK();
		GL_CHECK();
		glCreateTextures(target, 1u, &m_RendererIDTex);
		GL_CHECK();

		RY_CORE_ASSERT(0u != m_RendererIDTex, "Not Vaild Handle");

#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		RY_CORE_ASSERT(0u == m_RendererIDSam);

		GL_CHECK();
		glCreateSamplers(1u, &m_RendererIDSam);
		GL_CHECK();

		RY_CORE_ASSERT(0u != m_RendererIDSam, "Not Vaild Handle");
#endif	
	}

	
	void OpenGLLinkedTextureArray::DestroyID()
	{
		if (0u != m_RendererIDTex)
		{
			RY_OPENGL_TEXTURE_ID_SCOPE_LOCK();

			GL_CHECK();
			glDeleteTextures(1u, &m_RendererIDTex);
			m_RendererIDTex = 0u;
			GL_CHECK();
		}
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		if (0u == m_RendererIDSam)
		{
			GL_CHECK();
			glDeleteSamplers(1u, &m_RendererIDSam);
			m_RendererIDSam = 0u;
			GL_CHECK();
		}
		RY_CORE_ASSERT(0u == m_RendererIDSam);
#endif

		RY_CORE_ASSERT(0u == m_RendererIDTex);

	}

	void OpenGLLinkedTextureArray::Invalidate()
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLLinkedTextureArray::Invalidate, this));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLLinkedTextureArray::Invalidate);
#endif
		if (m_LinkedTexturesVec.empty())
			return;

		using Dimension = OpenGLTextureObject::DimensionOpenGL;
		using Type = OpenGLTextureObject::ObjectTypeOpenGL;

		Dimension dimension(
			m_Specification.Width,
			m_Specification.Height,
			m_Specification.Depth
		);
		dimension[m_TextureIndexDimension] *= m_LinkedTexturesVec.size();

		const Type& type = m_Object.GetObjectType();

		m_Object.SetDimension(dimension);

		CreateID(type.target);

		m_Object.Create(m_RendererIDTex);
#if !(RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER)
		const GLuint& samplerID = m_RendererIDSam;
#else
		const GLuint& samplerID = m_RendererIDTex;
#endif
		if (1u < type.samples)
		{
			m_Sampler.CreateSampler(samplerID);
		}
		else
		{
			m_Sampler.Create(samplerID);
		}
		uint32_t index = 0u;
		for(Weak<Texture>& textureWeak : m_LinkedTexturesVec)
		{
			if (Ref<Texture> texture = textureWeak.lock())
			{
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
				glm::uvec3 dstOffset = {
					0u, 0u, m_Specification.Depth * index,
				};
				glm::uvec3 srcOffset = {
					0u, 0u, 0u,
				};

				Dimension dimension(
					m_Specification.Width,
					m_Specification.Height,
					m_Specification.Depth
				);
#if 0
				m_Object.CopyFromTextureToTexture(
					m_RendererIDTex, texutureStorage->m_RendererIDTex
					, texutureStorage->m_Object
					, dimension, srcOffset, dstOffset
				);
#else
				m_Object.CopyFromTextureToTexture(
					texutureStorage->m_RendererIDTex, m_RendererIDTex
					, texutureStorage->m_Object,
					dimension, dstOffset, srcOffset
				);
#endif
#if 0
				std::vector<uint8_t> dataTex = texutureStorage->GetCurrentRenderData();
				m_Object.SetData(m_RendererIDTex, dataTex, dataTex.size(), index);
#endif
			}
			index++;
		}
		
		m_ChangedTextureInidicesVec.clear();
		m_DataUpdatedGPU = true;
		SetupFance();
	}

	void OpenGLLinkedTextureArray::SetupFance()
	{
		if (Asset::CurrentOnMainThread())
			return;

		m_FanceObject.SetupFence();
		uint32_t byteSize = m_Data.size();
		int64_t pastTime = m_FanceObject.StoppThreadUntlieLoded();
		RY_CORE_INFO("Data OpenGLArrayBuffer Transfered! off {} bytesComplet. Time waiting {} Nanosec, thread {}", byteSize, pastTime, std::this_thread::get_id());
		m_FanceObject.DestroyID();
		ClearLocaleDataStore();

	}

	void OpenGLLinkedTextureArray::ClearLocaleDataStore()
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
	}
	

	bool OpenGLLinkedTextureArray::IsDataRaydyOnGPU() const
	{
		return m_ChangedTextureInidicesVec.empty();
	}

	void OpenGLLinkedTextureArray::UpdateDataGPU()
	{
		if(0u == m_RendererIDTex)
		{
			Invalidate();
			return;
		}
		glm::uvec3 textureSize = {
			m_Specification.Width,
			m_Specification.Height,
			m_Specification.Depth,
		};
		glm::uvec3 dstOffset = {
			0u, 0u, 0u,
		};
		glm::uvec3 srcOffset = {
			0u, 0u, 0u,
		};

		for (int indexChanges : m_ChangedTextureInidicesVec)
		{
			if(Ref<Texture> texture = m_LinkedTexturesVec.at(indexChanges).lock())
			{
				dstOffset.z = textureSize.z * indexChanges;
				Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
				m_Object.CopyFromTextureToTexture(
					texutureStorage->m_RendererIDTex, m_RendererIDTex
					, texutureStorage->m_Object, 
					textureSize, dstOffset, srcOffset
				);
			}
			else
			{
				RY_CORE_ASSERT(false, "We expext to have every nullptr Removed, mabey somting Async chnaged!");
			}
		}
		m_DataUpdatedGPU = true;
		m_ChangedTextureInidicesVec.clear();

		SetupFance();
	}
	void OpenGLLinkedTextureArray::ResizeDimensions1D(uint32_t width)
	{
		RY_CORE_NOT_IMPL();

		m_Specification.Width = width;

		Invalidate();
	}

	void OpenGLLinkedTextureArray::ResizeDimensions2D(uint32_t width, uint32_t height)
	{
		RY_CORE_NOT_IMPL();

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();

	}
	void OpenGLLinkedTextureArray::ResizeDimensions3D(uint32_t width, uint32_t height, uint32_t depth)
	{
		RY_CORE_NOT_IMPL();
	}
#endif
#pragma endregion
	
	

}