#include "rypch.h"
#include "OpenGLTextureStorage.h"


#include <Platform/OpenGL/OpenGLBase.h>


#include <glad/glad.h>



namespace Rynex {

	namespace Utils {
		static uint32_t ImageChanelsBytes(GLenum interalformat)
		{
			switch (interalformat)
			{
			case GL_R8:					return 1 * 1;

			case GL_RG8:				return 1 * 2;

			case GL_SRGB8:				return 1 * 3;
			case GL_RGB8:				return 1 * 3;
			case GL_RGB16F:				return 2 * 3;
			case GL_RGB32F:				return 4 * 3;


			case GL_SRGB8_ALPHA8:		return 1 * 4;
			case GL_RGBA8:				return 1 * 4;
			case GL_RGBA16F:			return 2 * 4;
			case GL_RGBA32F:			return 4 * 4;

			case GL_RED_INTEGER:		return 4 * 1;

			case GL_DEPTH24_STENCIL8:	return 3 * 1;
			case GL_DEPTH32F_STENCIL8:	return 4 * 1;

			case GL_DEPTH_COMPONENT16:		return 1 * 2;
			case GL_DEPTH_COMPONENT24:		return 1 * 3;
			case GL_DEPTH_COMPONENT32:		return 1 * 3;
			case GL_DEPTH_COMPONENT32F:		return 1 * 4;

			default:
			{
				RY_CORE_ASSERT(false, "Error  Utils::ImageCahnelsBytes whrong TextureFormat!");
				return 1 * 4;
			}
			}

			RY_CORE_ASSERT(false, "Error: Utils::ImageCahnelsBytes!");
			return 1 * 4;
		}

	}

#pragma region OpenGLTextureModern
#if 1
#pragma region OpenGLTextureObjectType


	OpenGLTextureObject::ObjectTypeOpenGL::ObjectTypeOpenGL()
		: target(0xFFFFFFFF), formate(0xFFFFFFFF), interleFormat(0xFFFFFFFF), levelMidMaps(0xFFFFFFFF), samples(0xFFFFFFFF)
	{
	}

	OpenGLTextureObject::ObjectTypeOpenGL::ObjectTypeOpenGL(uint32_t target)
		: target(target), formate(GL_RGBA8), interleFormat(GL_RGBA), levelMidMaps(1), samples(1)
	{
	}

	OpenGLTextureObject::ObjectTypeOpenGL::ObjectTypeOpenGL(uint32_t target, uint32_t fromat)
		: target(target), formate(fromat), interleFormat(GL_RGBA), levelMidMaps(1), samples(1)
	{
	}

	OpenGLTextureObject::ObjectTypeOpenGL::ObjectTypeOpenGL(uint32_t target, uint32_t fromat, uint32_t interleFormat)
		: target(target), formate(fromat), interleFormat(interleFormat), levelMidMaps(1), samples(1)
	{
	}

	OpenGLTextureObject::ObjectTypeOpenGL::ObjectTypeOpenGL(uint32_t target, uint32_t fromat, uint32_t interleFormat, uint32_t levelMidMaps)
		: target(target), formate(fromat), interleFormat(interleFormat), levelMidMaps(levelMidMaps), samples(1)
	{
	}

	OpenGLTextureObject::ObjectTypeOpenGL::ObjectTypeOpenGL(uint32_t target, uint32_t fromat, uint32_t interleFormat, uint32_t levelMidMaps, uint32_t samples)
		: target(target), formate(fromat), interleFormat(interleFormat), levelMidMaps(levelMidMaps), samples(samples)
	{
	}








#pragma endregion



	OpenGLTextureObject::OpenGLTextureObject()
		: m_Dimension()
		, m_Object()
	{
	}

	OpenGLTextureObject::OpenGLTextureObject(const ObjectTypeOpenGL& object, const DimensionOpenGL& dimension)
		: m_Object(object)
		, m_Dimension(dimension)
	{
	}

	OpenGLTextureObject::~OpenGLTextureObject()
	{
	}



	void OpenGLTextureObject::Create(uint32_t renderID)
	{
		GL_CHECK();
		switch (m_Object.target)
		{
		case GL_TEXTURE_1D:
			CreateObject1D(renderID, nullptr);
			break;

		case GL_TEXTURE_2D:
		case GL_TEXTURE_2D_MULTISAMPLE:
		case GL_TEXTURE_1D_ARRAY:
			CreateObject2D(renderID, nullptr);
			break;

		case GL_TEXTURE_3D:
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			CreateObject3D(renderID, nullptr);
			break;

		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
		{



#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
			std::vector<uint8_t> vecData;
			std::array<void*, 6> texDataPtrArray;
			if (Converter1DVecInArray2DCubeMap(vecData, texDataPtrArray))
				CreateObjectCubeMap(renderID, texDataPtrArray);
#else
			uint32_t textureCount = 0u;;
			CreateObjectCubeMap(renderID, nullptr, textureCount);
#endif
			break;
		}
		case GL_TEXTURE_RECTANGLE:
			RY_CORE_ASSERT(false, "Not Used Defined OpenGL Texture Targets");
			break;


			// case GL_TEXTURE_CUBE_MAP_ARRAY:
			// 	RY_CORE_ASSERT(false, "Not Used Defined OpenGL Texture Array Targets");
			// 	break;

		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Target");
			break;
		}
		GL_CHECK();
	}

	void OpenGLTextureObject::Create(uint32_t renderID, const std::vector<uint8_t>& texData, uint32_t pixelByteSize)
	{
		GL_CHECK();
		RY_CORE_ASSERT(pixelByteSize == texData.size(), "Not Exepted Size!");

		switch (m_Object.target)
		{
		case GL_TEXTURE_1D:
			RY_CORE_ASSERT(texData.empty() || pixelByteSize == GetTextureByteSize(), "Not Eqaul Size!");
			CreateObject1D(renderID, texData.data());
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_2D_MULTISAMPLE:
		case GL_TEXTURE_1D_ARRAY:
			RY_CORE_ASSERT(texData.empty() || pixelByteSize == GetTextureByteSize(), "Not Eqaul Size!");
			CreateObject2D(renderID, texData.data());
			break;
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
		{

#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
			if (bytePerPixel != pixelPerTexture)
			{
				uint32_t overFlowBytes = pixelPerTexture % bytePerSide2D;
				RY_CORE_ASSERT(0u == overFlowBytes, "not rigth byteSize!");
			}
			std::array<void*, 6> texDataPtrArray;
			if (Converter1DVecInArray2DCubeMap(texData, texDataPtrArray))
				CreateObjectCubeMap(renderID, texDataPtrArray);
#else
			uint32_t textureCount = GetTextureCubeMapCount(texData);
			CreateObjectCubeMap(renderID, texData.data(), textureCount);

#endif 
			break;
		}
		case GL_TEXTURE_3D:
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			RY_CORE_ASSERT(texData.empty() || pixelByteSize == GetTextureByteSize(), "Not Eqaul Size!");
			CreateObject3D(renderID, texData.data());
			break;

		case GL_TEXTURE_RECTANGLE:
			RY_CORE_ASSERT(false, "Not Used Defined OpenGL Texture Targets");
			break;

			// case GL_TEXTURE_CUBE_MAP_ARRAY:
			// 	RY_CORE_ASSERT(false, "Not Used Defined OpenGL Texture Array Targets");
			// 	break;

		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Target");
			break;
		}
		GL_CHECK();
	}


	uint32_t OpenGLTextureObject::GetTextureByteSize() const
	{
		uint32_t bytePerPixel = Utils::ImageChanelsBytes(m_Object.interleFormat);
		uint32_t dataBytesSize = m_Dimension.heigth;
		dataBytesSize *= m_Dimension.withe;
		dataBytesSize *= m_Dimension.depth;
		dataBytesSize *= bytePerPixel;
		RY_CORE_ASSERT(dataBytesSize != 0u, "Zero is a not vaild Size!");
		return dataBytesSize;
	}


	void OpenGLTextureObject::SetData(uint32_t renderID, const std::vector<uint8_t>& texData, uint32_t pixelByteSize, uint32_t offset) const
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive())
			RY_CORE_ASSERT(pixelByteSize == texData.size(), "Not Exepted Size!");

		switch (m_Object.target)
		{
		case GL_TEXTURE_1D:
			SetData2D(renderID, texData.data());
			break;
		case GL_TEXTURE_2D:


		case GL_TEXTURE_2D_MULTISAMPLE:
			SetData2D(renderID, texData.data());
			break;

		case GL_TEXTURE_1D_ARRAY:
			RY_CORE_ASSERT(GetTexture1DByteSize() == texData.size(), "Not Exepted Size!");
			SetData2D(renderID, texData.data(), offset);
			break;

		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			RY_CORE_ASSERT(GetTexture2DByteSize() == texData.size(), "Not Exepted Size!");
			SetData3D(renderID, texData.data(), offset);
			break;

		case GL_TEXTURE_3D:
			SetData3D(renderID, texData.data());
			break;
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
		{
#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
			std::array<void*, 6> texDataPtrArray;
			RY_CORE_ASSERT(6u == m_Dimension.Depth, "If this is a cube map in the depth chanel 6 is expexted, for every image!");

			if (Converter1DVecInArray2DCubeMap(texData, texDataPtrArray))
				SetDataCubeMap(renderID, texDataPtrArray);
#else
			uint32_t textureCount = GetTextureCubeMapCount(texData);
			SetDataCubeMap(renderID, texData.data(), textureCount);
#endif 
			break;
		}
		case GL_TEXTURE_RECTANGLE:
			RY_CORE_ASSERT(false, "Not Used Defined OpenGL Texture Targets");
			break;


		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Target");
			break;
		}

	}


	void OpenGLTextureObject::GetData(uint32_t renderID, std::vector<uint8_t>& texData, uint32_t bpp) const
	{
		uint32_t format = m_Object.formate;

		GetData(renderID, texData, bpp, format);

	}

	void OpenGLTextureObject::GetData(uint32_t renderID, std::vector<uint8_t>& texData, uint32_t bpp, uint32_t format) const
	{
		texData.clear();
		uint32_t withe = m_Dimension.withe;
		uint32_t height = m_Dimension.heigth;

		uint32_t byteImeg = withe * height * bpp;
		texData.resize(byteImeg);
		uint8_t* dataPtr = texData.data();
		uint32_t dataByteSize = texData.size();
		glGetTextureImage(renderID, 0, format, GL_UNSIGNED_BYTE, dataByteSize, dataPtr);
	}



	void OpenGLTextureObject::ClearImage(uint32_t renderID, const uint8_t* clearData, uint32_t type)const
	{
		uint32_t format = m_Object.formate;

		glClearTexImage(renderID, 0, format, type, clearData);
	}

	void OpenGLTextureObject::ClearImage(uint32_t renderID, std::nullptr_t, uint32_t type)const
	{
		RY_CORE_WARN("This Funktion Implemention withe const void* a exception Implemention not with throw a exception to use const void* so nullptr is valid!");

		uint32_t format = m_Object.formate;
		glClearTexImage(renderID, 0, format, type, nullptr);
	}





	void OpenGLTextureObject::PrintSettings()
	{
		RY_CORE_TRACE("Target: {}", OpenGL::GetTargetStr(m_Object.target));
		RY_CORE_TRACE("samples: {}", m_Object.samples);
		RY_CORE_TRACE("Format: {}", OpenGL::GetTextureFomateStr(m_Object.formate));
		RY_CORE_TRACE("InterleFormat: {}", OpenGL::GetTextureFomateInternelStr(m_Object.interleFormat));
		RY_CORE_TRACE("LevelMidMaps: {}", m_Object.levelMidMaps);
		RY_CORE_TRACE("Withe: {}", m_Dimension.withe);
		RY_CORE_TRACE("Heigth: {}", m_Dimension.heigth);
		RY_CORE_TRACE("Depth: {}", m_Dimension.depth);

	}

	bool OpenGLTextureObject::CopyFromTextureToTexture(uint32_t srcRenderID, uint32_t dstRenderID, const OpenGLTextureObject& srcTexture, const DimensionOpenGL& copySize)
	{
		DimensionOpenGL dstDimension = {
			0u, 0u, 0u
		};
		DimensionOpenGL srcDimension = {
			0u, 0u, 0u
		};
		return CopyFromTextureToTexture(srcRenderID, dstRenderID, srcTexture, copySize, dstDimension, srcDimension);
	}

	bool OpenGLTextureObject::CopyFromTextureToTexture(uint32_t srcRenderID, uint32_t dstRenderID, const OpenGLTextureObject& srcTexture, const DimensionOpenGL& copySize, const DimensionOpenGL& dstOffset, const DimensionOpenGL& srcOffset)
	{
		if (dstRenderID == srcRenderID)
			return false;

		if (!IsInsideVaildCopyRange(srcTexture.m_Dimension, srcOffset, copySize))
			return false;

		if (!IsInsideVaildCopyRange(m_Dimension, dstOffset, copySize))
			return false;

		TextureCopyData srcCopyData = {
			srcRenderID,
			srcOffset,
			srcTexture.m_Object
		};

		TextureCopyData dstCopyData = {
			dstRenderID,
			dstOffset,
			m_Object
		};


		CopyTextureToTexture(srcCopyData, dstCopyData, copySize);

		return true;
	}

	void OpenGLTextureObject::CopyTextureToTexture(const TextureCopyData& srcCopy, const TextureCopyData& dstCopy, const OpenGLTextureObject::DimensionOpenGL& dimensionCopy)
	{
		constexpr uint32_t midMapsLevelsrc = 0u;
		constexpr uint32_t midMapsLeveldst = 0u;
		GL_CHECK_LOOP();

		glCopyImageSubData(
			srcCopy.renderID, srcCopy.type.target, midMapsLevelsrc, srcCopy.offset.withe, srcCopy.offset.heigth, srcCopy.offset.depth,
			dstCopy.renderID, dstCopy.type.target, midMapsLeveldst, dstCopy.offset.withe, dstCopy.offset.heigth, dstCopy.offset.depth,
			dimensionCopy.withe, dimensionCopy.heigth, dimensionCopy.depth
		);
#if 0
		RY_CORE_FUNC_LOG(glCopyImageSubData,
			srcCopy.renderID, srcCopy.type.target, midMapsLevelsrc, srcCopy.offset.Withe, srcCopy.offset.Heigth, srcCopy.offset.Depth,
			dstCopy.renderID, dstCopy.type.target, midMapsLeveldst, dstCopy.offset.Withe, dstCopy.offset.Heigth, dstCopy.offset.Depth,
			dimensionCopy.withe, dimensionCopy.heigth, dimensionCopy.Depth
		);
#endif
		GL_CHECK();
	}

	void OpenGLTextureObject::CreateObject1D(uint32_t renderID, const uint8_t* texDataPtr)const
	{

		glTextureStorage1D(renderID, m_Object.levelMidMaps, m_Object.interleFormat, m_Dimension.withe);
		if (nullptr != texDataPtr)
		{
			glTextureSubImage1D(renderID, 0, 0, m_Dimension.withe, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
		}
	}

	void OpenGLTextureObject::CreateObject2D(uint32_t renderID, const uint8_t* texDataPtr)const
	{


		if (GL_TEXTURE_2D == m_Object.target || GL_TEXTURE_1D_ARRAY == m_Object.target)
		{
			uint32_t midMap = m_Object.levelMidMaps != 0u ? m_Object.levelMidMaps : 1u;
			glTextureStorage2D(renderID, midMap, m_Object.interleFormat, m_Dimension.withe, m_Dimension.heigth);
#if RY_OPENGL_TEXTURE_LOG_FUNC
			RY_CORE_FUNC_LOG(glTextureStorage2D, renderID, midMap, m_Object.interleFormat, m_Dimension.Withe, m_Dimension.Heigth);
#endif

		}
		else if (GL_TEXTURE_2D_MULTISAMPLE == m_Object.target)
		{
			glTextureStorage2DMultisample(renderID, m_Object.samples, m_Object.interleFormat, m_Dimension.withe, m_Dimension.heigth, GL_FALSE);
#if RY_OPENGL_TEXTURE_LOG_FUNC
			RY_CORE_FUNC_LOG(glTextureStorage2DMultisample, renderID, m_Object.samples, m_Object.interleFormat, m_Dimension.Withe, m_Dimension.Heigth, GL_FALSE);
#endif

		}

		GL_CHECK();
		if (nullptr != texDataPtr)
		{
			glTextureSubImage2D(renderID, 0, 0, 0, m_Dimension.withe, m_Dimension.heigth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
#if RY_OPENGL_TEXTURE_LOG_FUNC
			RY_CORE_FUNC_LOG(glTextureSubImage2D, renderID, 0, 0, 0, m_Dimension.Withe, m_Dimension.Heigth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
#endif
			GL_CHECK();
		}
	}

	void OpenGLTextureObject::CreateObject3D(uint32_t renderID, const uint8_t* texDataPtr)const
	{

		if (GL_TEXTURE_3D == m_Object.target || GL_TEXTURE_2D_ARRAY == m_Object.target)
		{
			uint32_t midMap = m_Object.levelMidMaps != 0u ? m_Object.levelMidMaps : 1u;
			glTextureStorage3D(renderID, midMap, m_Object.interleFormat, m_Dimension.withe, m_Dimension.heigth, m_Dimension.depth);
		}
		else if (GL_TEXTURE_2D_MULTISAMPLE_ARRAY == m_Object.target)
		{
			uint32_t midMap = m_Object.levelMidMaps != 0u ? m_Object.levelMidMaps : 1u;
			glTextureStorage3DMultisample(renderID, m_Object.levelMidMaps, m_Object.interleFormat, m_Dimension.withe, m_Dimension.heigth, m_Dimension.depth, GL_FALSE);
		}
		else
		{
			RY_CORE_ASSERT(false);
		}

		GL_CHECK();
		if (nullptr != texDataPtr)
		{

			glTextureSubImage3D(renderID, 0, 0, 0, 0, m_Dimension.withe, m_Dimension.heigth, m_Dimension.depth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
		}
		// else if( GL_TEXTURE_2D_ARRAY == m_Object.Target || GL_TEXTURE_2D_MULTISAMPLE_ARRAY == m_Object.Target)
		// {
		// 	OpenGLTextureObject::ClearImage(renderID, nullptr, GL_UNSIGNED_BYTE);
		// 
		// }
		// else if (GL_TEXTURE_2D_ARRAY == m_Object.Target || GL_TEXTURE_2D_MULTISAMPLE_ARRAY == m_Object.Target)
		// {
		// 	std::vector<uint32_t> dataVec;
		// 	dataVec.resize(GetTextureByteSize(), 0xF808F808);
		// 	GL_CHECK();
		// 	glTextureSubImage3D(renderID, 0, 0, 0, 0, m_Dimension.Withe, m_Dimension.Heigth, m_Dimension.Depth, m_Object.formate, GL_UNSIGNED_BYTE, dataVec.data());
		// 	GL_CHECK();
		// }
	}
#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
	void OpenGLTextureObject::CreateObjectCubeMap(uint32_t renderID, const std::array<uint8_t*, 6>& texDataPtrArray) const
#else
	void OpenGLTextureObject::CreateObjectCubeMap(uint32_t renderID, const uint8_t* texDataPtr, uint32_t texureCount) const
#endif
	{
	

		if (GL_TEXTURE_CUBE_MAP == m_Object.target)
		{
			uint32_t midMap = m_Object.levelMidMaps != 0u ? m_Object.levelMidMaps : 1u;
			glTextureStorage2D(renderID, midMap, m_Object.interleFormat, m_Dimension.withe, m_Dimension.heigth);
		}
		else if (GL_TEXTURE_CUBE_MAP_ARRAY == m_Object.target)
		{
			uint32_t midMap = m_Object.levelMidMaps != 0u ? m_Object.levelMidMaps : 1u;
			glTextureStorage3D(renderID, midMap, m_Object.interleFormat, m_Dimension.withe, m_Dimension.heigth, m_Dimension.depth);
		}
		else
		{
			RY_CORE_ASSERT(false);
		}

		GL_CHECK();

#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
		for (int i = 0; i < 6; i++)
		{
			const void* texDataPtr = texDataPtrArray[i];
			if (nullptr != texDataPtr)
			{
				uint32_t depth = 1u;
				glTextureSubImage3D(renderID, 0, 0, 0, i, m_Dimension.Withe, m_Dimension.Heigth, depth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
			}
		}
#else
		if (nullptr == texDataPtr)
			return;
		uint32_t depth = m_Dimension.depth;
		constexpr uint32_t offsetZ = 0u;
		uint32_t depthModule6 = depth % 6;
		RY_CORE_ASSERT(0u == depthModule6, "On A Cube map We expaxted a depth of 6 per Cube map!");

		glTextureSubImage3D(renderID, 0, 0, 0, offsetZ, m_Dimension.withe, m_Dimension.heigth, depth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
#endif

	}

	uint32_t OpenGLTextureObject::GetTexture2DByteSize() const
	{
		uint32_t bytePerPixel = Utils::ImageChanelsBytes(m_Object.interleFormat);
		uint32_t dataBytesSize = m_Dimension.heigth;
		dataBytesSize *= m_Dimension.withe;
		dataBytesSize *= bytePerPixel;
		RY_CORE_ASSERT(0u != dataBytesSize, "Zero is a not vaild Size!");
		return dataBytesSize;
	}

	uint32_t OpenGLTextureObject::GetTexture1DByteSize() const
	{
		uint32_t bytePerPixel = Utils::ImageChanelsBytes(m_Object.interleFormat);
		uint32_t dataBytesSize = m_Dimension.withe;
		dataBytesSize *= bytePerPixel;
		RY_CORE_ASSERT(0u != dataBytesSize, "Zero is a not vaild Size!");
		return dataBytesSize;
	}

	uint32_t OpenGLTextureObject::GetTextureCubeMapCount(const std::vector<uint8_t>& texData) const
	{
		uint32_t depthModuleOf6 = m_Dimension.depth % 6u;
		RY_CORE_ASSERT(6u == m_Dimension.depth || 0u == depthModuleOf6, "If this is a cube map in the depth chanel 6 is expexted, for every image!");
		RY_CORE_ASSERT(m_Dimension.withe == m_Dimension.heigth, "On a Cube map with and heigth need to be Equal");

		const uint32_t count = texData.size();
		const uint32_t bytePerSide2D = count / 6u;
		uint32_t pixelTextureInCubeMap = GetTexture2DByteSize();

		double textureCountDouble = static_cast<double>(count) / static_cast<double>(pixelTextureInCubeMap);
		textureCountDouble = std::round(textureCountDouble);

		uint32_t textureCount = static_cast<uint32_t>(textureCountDouble);
		RY_CORE_ASSERT(textureCount <= m_Dimension.depth, "Not same vaild Size!");

		uint32_t dataByteSize = textureCount * pixelTextureInCubeMap;
		RY_CORE_ASSERT(dataByteSize <= GetTextureByteSize(), " not expexted Size!");
		return textureCount;
	}



	void OpenGLTextureObject::SetData1D(uint32_t renderID, const uint8_t* texDataPtr) const
	{

		glTextureSubImage1D(renderID, 0, 0, m_Dimension.withe, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
	}

	void OpenGLTextureObject::SetData2D(uint32_t renderID, const uint8_t* texDataPtr, int offsetY) const
	{

		glTextureSubImage2D(renderID, 0, 0, offsetY == -1 ? 0 : offsetY, m_Dimension.withe, offsetY == -1 ? m_Dimension.depth : 1u, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);

	}


	void OpenGLTextureObject::SetData3D(uint32_t renderID, const uint8_t* texDataPtr, int offsetZ) const
	{

		glTextureSubImage3D(renderID, 0, 0, 0, offsetZ == -1 ? 0 : offsetZ, m_Dimension.withe, m_Dimension.heigth, offsetZ == -1 ? m_Dimension.depth : 1u, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
	}

#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
	void OpenGLTextureObject::SetDataCubeMap(uint32_t renderID, const std::array<uint8_t*, 6>& texDataPtrArray) const
	{
		for (int i = 0; i < 6; i++)
		{
			const void* texDataPtr = texDataPtrArray[i];
			if (nullptr != texDataPtr)
			{
				uint32_t depth = 1u;
				glTextureSubImage3D(renderID, 0, 0, 0, i, m_Dimension.Withe, m_Dimension.Heigth, depth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
			}
		}
	}
#else
	void OpenGLTextureObject::SetDataCubeMap(uint32_t renderID, const uint8_t* texDataPtr, uint32_t textureCount, uint32_t offsetCubeMapTexture) const
	{
		const uint32_t depth = textureCount;
		const uint32_t offsetZ = offsetCubeMapTexture * 6u;
		const uint32_t offsetDepth = offsetCubeMapTexture * 6;

		uint32_t depthModule6 = m_Dimension.depth % 6;

		RY_CORE_ASSERT(0u == depthModule6, "On A Cube map We expaxted a depth of 6 per Cube map!");
		glTextureSubImage3D(renderID, 0, 0, 0, offsetZ, m_Dimension.withe, m_Dimension.heigth, depth, m_Object.formate, GL_UNSIGNED_BYTE, texDataPtr);
	}
#endif


	bool OpenGLTextureObject::IsInsideVaildCopyRange(DimensionOpenGL textureSize, DimensionOpenGL offsetSize, DimensionOpenGL copySize)
	{
		bool resultWith = IsInsideVaildCopyRange(textureSize.withe, offsetSize.withe, copySize.withe);
		bool resultHeigth = IsInsideVaildCopyRange(textureSize.heigth, offsetSize.heigth, copySize.heigth);
		bool resultDepth = IsInsideVaildCopyRange(textureSize.depth, offsetSize.depth, copySize.depth);
		bool result = resultWith && resultHeigth && resultDepth;
		return result;
	}

	bool OpenGLTextureObject::IsInsideVaildCopyRange(uint32_t textureDimensionSize, uint32_t textureDimensionOffset, uint32_t textureDimensionCopySize)
	{
		uint32_t needSize = textureDimensionOffset + textureDimensionCopySize;
		return 0u != needSize && 0u != textureDimensionSize && needSize <= textureDimensionSize;
	}

	bool OpenGLTextureObject::Converter1DVecInArray2DCubeMap(const std::vector<uint8_t>& texData, std::array<void*, 6>& texDataPtrArray)
	{
		const uint64_t count = texData.size();
		const uint64_t bytePerSide2D = count / 6u;
		texDataPtrArray = {
			nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr
		};
		const uint8_t* dataPtr = texData.data();
		for (int i = 0; i < 6; i++)
		{
			const uint64_t offsetByte = bytePerSide2D * i;
			if (offsetByte < count)
			{
				RY_CORE_ASSERT(offsetByte < count);
				const void* posPtr = dataPtr + offsetByte;
				texDataPtrArray[i] = const_cast<void*>(posPtr);
			}
			else if (offsetByte == count)
			{
				const void* posPtr = dataPtr + offsetByte;
				texDataPtrArray[i] = const_cast<void*>(posPtr);
				return true;
			}
			else
			{
				RY_CORE_ASSERT(offsetByte < count, "Data overflow!");
				return false;
			}
		}
		RY_CORE_ASSERT("We expext the funtion shoud be alrady, fisiched the last loop iteration shoud be leed in the secound condition!");
		return true;

	}






#pragma region SamplerOpenGL

#pragma region Warp

	OpenGLTextureSampler::WarpOpenGL::WarpOpenGL()
		: S(GL_CLAMP_TO_EDGE), T(GL_CLAMP_TO_EDGE), R(GL_CLAMP_TO_EDGE)
	{
	}

	OpenGLTextureSampler::WarpOpenGL::WarpOpenGL(uint32_t v)
		: S(v), T(v), R(v)
	{
	}

	OpenGLTextureSampler::WarpOpenGL::WarpOpenGL(uint32_t s, uint32_t t)
		: S(s), T(t), R(GL_CLAMP_TO_EDGE)
	{
	}

	OpenGLTextureSampler::WarpOpenGL::WarpOpenGL(uint32_t s, uint32_t t, uint32_t r)
		: S(s), T(t), R(r)
	{
	}

#pragma endregion


#pragma region Compare

	OpenGLTextureSampler::CompareOpenGL::CompareOpenGL()
		: Mode(GL_NONE)
		, Func(GL_LEQUAL)
	{
	}

	OpenGLTextureSampler::CompareOpenGL::CompareOpenGL(uint32_t mode, uint32_t func)
		: Mode(mode)
		, Func(func)
	{
	}

#pragma endregion

#pragma region Filter

	OpenGLTextureSampler::FilterOpenGL::FilterOpenGL()
		: Min(GL_LINEAR), Mag(GL_LINEAR)
	{
	}

	OpenGLTextureSampler::FilterOpenGL::FilterOpenGL(uint32_t min, uint32_t mag)
		: Min(min), Mag(mag)
	{
	}

#pragma endregion

	OpenGLTextureSampler::OpenGLTextureSampler()
		: m_Filter(), m_Warp(), m_Compare(), m_Lod(), m_BorderColor()
	{
	}

	OpenGLTextureSampler::OpenGLTextureSampler(const FilterOpenGL& filter, const WarpOpenGL& warp, const CompareOpenGL& compare, const LodOpenGL& lod, const std::array<float, 4>& borderColor)
		: m_Filter(filter), m_Warp(warp), m_Compare(compare), m_Lod(lod), m_BorderColor(borderColor)
	{
	}

	OpenGLTextureSampler::~OpenGLTextureSampler()
	{
	}

	void OpenGLTextureSampler::PrintSettings()
	{
	}

	void OpenGLTextureSampler::Create(uint32_t renderID)
	{
		GL_CHECK();
		CreateFilter(renderID);
		CreateWarp(renderID);
		CreateCompare(renderID);
		CreateLod(renderID);
		GL_CHECK();
	}

	void OpenGLTextureSampler::CreateSampler(uint32_t renderID)
	{
		GL_CHECK();
		// CreateFilter(renderID);
		// CreateWarp(renderID);
		// CreateCompare(renderID);
		// CreateLod(renderID);
		// GL_CHECK();
	}


#if RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER
	void OpenGLTextureSampler::CreateFilter(uint32_t renderID)const
	{
		glTextureParameteri(renderID, GL_TEXTURE_MIN_FILTER, m_Filter.Min);
		glTextureParameteri(renderID, GL_TEXTURE_MAG_FILTER, m_Filter.Mag);
	}

	void OpenGLTextureSampler::CreateWarp(uint32_t renderID)const
	{
		glTextureParameteri(renderID, GL_TEXTURE_WRAP_S, m_Warp.S);
		glTextureParameteri(renderID, GL_TEXTURE_WRAP_R, m_Warp.R);
		glTextureParameteri(renderID, GL_TEXTURE_WRAP_T, m_Warp.T);
	}

	void OpenGLTextureSampler::CreateCompare(uint32_t renderID)const
	{
		glTextureParameteri(renderID, GL_TEXTURE_COMPARE_MODE, m_Compare.Mode);
		glTextureParameteri(renderID, GL_TEXTURE_COMPARE_FUNC, m_Compare.Func);
	}

	void OpenGLTextureSampler::CreateLod(uint32_t renderID) const
	{
		glTextureParameterf(renderID, GL_TEXTURE_MIN_LOD, m_Lod.Min);
		glTextureParameterf(renderID, GL_TEXTURE_MAX_LOD, m_Lod.Max);

		glTextureParameterf(renderID, GL_TEXTURE_LOD_BIAS, m_Lod.Bias);
		glTextureParameterf(renderID, GL_TEXTURE_MAX_ANISOTROPY, m_Lod.Anisotropy);
	}

	void OpenGLTextureSampler::CreateBorderColor(uint32_t renderID) const
	{
		glTextureParameterfv(renderID, GL_TEXTURE_BORDER_COLOR, m_BorderColor.data());
	}
#else
	void OpenGLTextureSampler::CreateFilter(uint32_t renderID)const
	{
		glSamplerParameteri(renderID, GL_TEXTURE_MIN_FILTER, m_Filter.Min);
		glSamplerParameteri(renderID, GL_TEXTURE_MAG_FILTER, m_Filter.Mag);
	}

	void OpenGLTextureSampler::CreateWarp(uint32_t renderID)const
	{
		glSamplerParameteri(renderID, GL_TEXTURE_WRAP_S, m_Warp.S);
		glSamplerParameteri(renderID, GL_TEXTURE_WRAP_R, m_Warp.R);
		glSamplerParameteri(renderID, GL_TEXTURE_WRAP_T, m_Warp.T);
	}

	void OpenGLTextureSampler::CreateCompare(uint32_t renderID)const
	{
		glSamplerParameteri(renderID, GL_TEXTURE_COMPARE_MODE, m_Compare.Mode);
		glSamplerParameteri(renderID, GL_TEXTURE_COMPARE_FUNC, m_Compare.Func);
	}

	void OpenGLTextureSampler::CreateLod(uint32_t renderID) const
	{
		glSamplerParameterf(renderID, GL_TEXTURE_MIN_LOD, m_Lod.Min);
		glSamplerParameterf(renderID, GL_TEXTURE_MAX_LOD, m_Lod.Max);

		glSamplerParameterf(renderID, GL_TEXTURE_LOD_BIAS, m_Lod.Bias);
		glSamplerParameterf(renderID, GL_TEXTURE_MAX_ANISOTROPY, m_Lod.Anisotropy);
	}

	void OpenGLTextureSampler::CreateBorderColor(uint32_t renderID) const
	{
		glSamplerParameterfv(renderID, GL_TEXTURE_BORDER_COLOR, m_BorderColor.data());
	}
#endif

#pragma endregion
#endif
#pragma endregion
}
