#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {

	namespace ObjectNamesDefnitionCompile {

		static constexpr const char* s_NameGLM_X = "x";
		static constexpr const char* s_NameGLM_Y = "y";
		static constexpr const char* s_NameGLM_Z = "z";
		static constexpr const char* s_NameGLM_W = "w";
		static constexpr const char* s_NameArrayBeginIndex = "[";
		static constexpr const char* s_NameArrayEndIndex = "]";


		static constexpr const char* s_NamePlusOpertor = "+";
		static constexpr const char* s_NameMinusOpertor = "-";
		static constexpr const char* s_NameMultyplayOpertor = "*";
		static constexpr const char* s_NameDividedOpertor = "/";

		static constexpr const char* s_NameLocale = "locale";
		static constexpr const char* s_NameGloble = "globle";

		static constexpr const char* s_NameUniform = "uniform";
		static constexpr const char* s_NameVertex = "vertex";
		static constexpr const char* s_NameTexture = "texture";
		static constexpr const char* s_NameStorage = "storage";


		static constexpr const char* s_NameMateriel = "materiel";
		static constexpr const char* s_NameGeomtry = "geomtry";
		static constexpr const char* s_NameEntity = "entity";
		static constexpr const char* s_NamePass = "pass";
		static constexpr const char* s_NameCamera = "camera";
		static constexpr const char* s_NameComponent = "component";
		static constexpr const char* s_NamePrame = "prame";
		static constexpr const char* s_NameAABB = "aabb";
		static constexpr const char* s_NameSphere = "sphere";
		static constexpr const char* s_NameViewFustrum = "viewFustrum";
		static constexpr const char* s_NameLeft = "left";
		static constexpr const char* s_NameRigth = "rigth";
		static constexpr const char* s_NameBottem = "bottem";
		static constexpr const char* s_NameTop = "top";

		static constexpr const char* s_NameAlphaTime = "alphaTime";
		static constexpr const char* s_NameDeltaTime = "deltaTime";
		static constexpr const char* s_NameFrameCount = "frameIndex";

		static constexpr const char* s_NameMillesSceounds = "millesSecounds";
		static constexpr const char* s_NameMicroSecounds = "microSecounds";

		static constexpr const char* s_NameGammaValue= "gamma";
		static constexpr const char* s_NameTextureSize = "size";
		static constexpr const char* s_NameRenderSize = "renderSize";
		static constexpr const char* s_NameFar = "far";
		static constexpr const char* s_NameNear = "near";
		static constexpr const char* s_NameFov = "fov";
		static constexpr const char* s_NameID = "id";
		static constexpr const char* s_NameIndex = "index";
		static constexpr const char* s_NameAspect = "aspect";
		static constexpr const char* s_NameRadius = "radius";


		static constexpr const char* s_NameTransformtion = "transformation";
		static constexpr const char* s_NamePostion = "postion";
		static constexpr const char* s_NameRotation = "rotation";
		static constexpr const char* s_NameCenter = "center";
		static constexpr const char* s_NameDirection = "direction";
		static constexpr const char* s_NameQuantion = "quantion";
		static constexpr const char* s_NameScale = "scale";
		static constexpr const char* s_NameMax = "max";
		static constexpr const char* s_NameMin = "min";
		static constexpr const char* s_NameTextureStCoord = "textureStCoord";
		static constexpr const char* s_NameNormale = "normale";

		static constexpr const char* s_ValueNone = "none";


		static constexpr const char* s_NameModelMatrix = "modelMatrix";
		static constexpr const char* s_NameNormleMatrix = "normaleMatrix";
		static constexpr const char* s_NameProjtionMatrix = "projtionMatrix";
		static constexpr const char* s_NameViewMatrix = "viewMatrix";
		static constexpr const char* s_NameProjtionViewMatrix = "projtionViewMatrix";
		static constexpr const char* s_NameInverseProjtionViewMatrix = "inverseProjtionViewMatrix";
		static constexpr const char* s_NameTrafmoredScaledProjtionViewMatrix = "trafmoredScaledProjtionViewMatrix";
		static constexpr const char* s_NameModelProjtionViewMatrix = "modelProjtionViewMatrix";

	

		static constexpr const char* s_NameAllArray[] = {
			s_NameUniform, s_NameVertex, s_NameVertex, s_NameTexture, s_NameStorage,
			s_ValueNone, s_NameGLM_X, s_NameGLM_Y, s_NameGLM_Z, s_NameGLM_W, s_NameArrayBeginIndex, s_NameArrayEndIndex,
			s_NameLocale, s_NameGloble, 
			s_NameViewFustrum, s_NameLeft, s_NameRigth, 
			s_NameID, s_NameIndex, s_NameRadius,
			s_NameMateriel, s_NameGeomtry, s_NameEntity, s_NamePass, s_NameCamera, s_NameComponent, s_NamePrame,
			s_NameAlphaTime, s_NameDeltaTime, s_NameFrameCount, s_NameMicroSecounds, s_NameMicroSecounds,
			s_NameGammaValue, s_NameTextureSize, s_NameRenderSize, s_NameFar, s_NameNear, s_NameFov, s_NameAspect,
			s_NameTransformtion, s_NamePostion, s_NameRotation, s_NameDirection, s_NameQuantion, s_NameScale, s_NameMax, s_NameMin,
			s_NameTextureStCoord, s_NameNormale,
			s_NameModelMatrix, s_NameNormleMatrix, s_NameProjtionMatrix, s_NameViewMatrix, s_NameProjtionViewMatrix, s_NameInverseProjtionViewMatrix,
			s_NameTrafmoredScaledProjtionViewMatrix, s_NameModelProjtionViewMatrix, 
			s_NamePlusOpertor, s_NameMinusOpertor, s_NameMultyplayOpertor, s_NameDividedOpertor
		};
	}

	struct ElementSource
	{
		ShaderDataType type;
		std::string name;
		std::string Source;
		bool normilzed;
	};

	

	struct BufferLayouteSource 
	{
		std::string nameBuffer;
		BufferType bufferType;
		std::vector<ElementSource> layoute;

	};
	
	struct BufferSource
	{
		std::string nameBuffer;
		BufferType bufferType;
		std::string source;
	};

	class RenderPipline : public Asset
	{
	public:
		RenderPipline();
		~RenderPipline();


		static AssetType GetStaticType() { return AssetType::RenderPipline; }
		AssetType GetType() const override { return GetStaticType(); }
	private:

	private:
		BufferSource m_BufferSource;
		BufferLayouteSource m_BufferLayouteSource;
	};
}
