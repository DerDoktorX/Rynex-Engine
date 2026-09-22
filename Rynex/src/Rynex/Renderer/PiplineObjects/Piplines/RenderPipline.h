#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {

	namespace ObjectNamesDefinitionCompile {

		constexpr const char* s_NameGLM_X = "x";
		constexpr const char* s_NameGLM_Y = "y";
		constexpr const char* s_NameGLM_Z = "z";
		constexpr const char* s_NameGLM_W = "w";
		constexpr const char* s_NameArrayBeginIndex = "[";
		constexpr const char* s_NameArrayEndIndex = "]";
	    
		constexpr const char* s_NamePlusOperator = "+";
		constexpr const char* s_NameMinusOperator = "-";
		constexpr const char* s_NameMultiplayerOperator = "*";
		constexpr const char* s_NameDividedOperator = "/";

		constexpr const char* s_NameLocale = "locale";
		constexpr const char* s_NameGlobe = "global";

		constexpr const char* s_NameUniform = "uniform";
		constexpr const char* s_NameVertex = "vertex";
		constexpr const char* s_NameTexture = "texture";
		constexpr const char* s_NameStorage = "storage";


		constexpr const char* s_NameMateriel = "materiel";
		constexpr const char* s_NameGeometry = "geometry";
		constexpr const char* s_NameEntity = "entity";
		constexpr const char* s_NamePass = "pass";
		constexpr const char* s_NameCamera = "camera";
		constexpr const char* s_NameComponent = "component";
		constexpr const char* s_NamePrama = "prama";
		constexpr const char* s_NameAABB = "aabb";
		constexpr const char* s_NameSphere = "sphere";
		constexpr const char* s_NameViewFrustum = "viewFrustum";
		constexpr const char* s_NameLeft = "left";
		constexpr const char* s_NameRight = "right";
		constexpr const char* s_NameBottem = "bottem";
		constexpr const char* s_NameTop = "top";

		constexpr const char* s_NameAlphaTime = "alphaTime";
		constexpr const char* s_NameDeltaTime = "deltaTime";
		constexpr const char* s_NameFrameCount = "frameIndex";

		constexpr const char* s_NameMillisSeconds = "millisSeconds";
		constexpr const char* s_NameMicroSeconds = "microSeconds";

		constexpr const char* s_NameGammaValue= "gamma";
		constexpr const char* s_NameTextureSize = "size";
		constexpr const char* s_NameRenderSize = "renderSize";
		constexpr const char* s_NameFar = "far";
		constexpr const char* s_NameNear = "near";
		constexpr const char* s_NameFov = "fov";
		constexpr const char* s_NameID = "id";
		constexpr const char* s_NameIndex = "index";
		constexpr const char* s_NameAspect = "aspect";
		constexpr const char* s_NameRadius = "radius";


		constexpr const char* s_NameTransformation = "transformation";
		constexpr const char* s_NamePosition = "position";
		constexpr const char* s_NameRotation = "rotation";
		constexpr const char* s_NameCenter = "center";
		constexpr const char* s_NameDirection = "direction";
		constexpr const char* s_NameQuantion = "quantion";
		constexpr const char* s_NameScale = "scale";
		constexpr const char* s_NameMax = "max";
		constexpr const char* s_NameMin = "min";
		constexpr const char* s_NameTextureStCoord = "textureStCoord";
		constexpr const char* s_NameNormale = "normale";

		constexpr const char* s_ValueNone = "none";


		constexpr const char* s_NameModelMatrix = "modelMatrix";
		constexpr const char* s_NameNormaleMatrix = "normaleMatrix";
		constexpr const char* s_NameProjectionMatrix = "projectionMatrix";
		constexpr const char* s_NameViewMatrix = "viewMatrix";
		constexpr const char* s_NameProjectionViewMatrix = "projectionViewMatrix";
		constexpr const char* s_NameInverseProjectionViewMatrix = "inverseProjectionViewMatrix";
		constexpr const char* s_NameTransformScaledProjectionViewMatrix = "transformScaledProjectionViewMatrix";
		constexpr const char* s_NameModelProjectionViewMatrix = "modelProjectionViewMatrix";

	

		constexpr const char* s_NameAllArray[] = {
			s_NameUniform, s_NameVertex, s_NameVertex, s_NameTexture, s_NameStorage,
			s_ValueNone, s_NameGLM_X, s_NameGLM_Y, s_NameGLM_Z, s_NameGLM_W, s_NameArrayBeginIndex, s_NameArrayEndIndex,
			s_NameLocale, s_NameGlobe,
			s_NameViewFrustum, s_NameLeft, s_NameRight,
			s_NameID, s_NameIndex, s_NameRadius,
			s_NameMateriel, s_NameGeometry, s_NameEntity, s_NamePass, s_NameCamera, s_NameComponent, s_NamePrama,
			s_NameAlphaTime, s_NameDeltaTime, s_NameFrameCount, s_NameMicroSeconds, s_NameMicroSeconds,
			s_NameGammaValue, s_NameTextureSize, s_NameRenderSize, s_NameFar, s_NameNear, s_NameFov, s_NameAspect,
			s_NameTransformation, s_NamePosition, s_NameRotation, s_NameDirection, s_NameQuantion, s_NameScale, s_NameMax, s_NameMin,
			s_NameTextureStCoord, s_NameNormale,
			s_NameModelMatrix, s_NameNormaleMatrix, s_NameProjectionMatrix, s_NameViewMatrix, s_NameProjectionViewMatrix, s_NameInverseProjectionViewMatrix,
			s_NameTransformScaledProjectionViewMatrix, s_NameModelProjectionViewMatrix,
			s_NameMinusOperator, s_NameMinusOperator, s_NameMultiplayerOperator, s_NameDividedOperator
		};
	}

	struct ElementSource
	{
		ShaderDataType m_Type;
		std::string m_Name;
		std::string m_Source;
		bool m_Normalized;
	};

	

	struct BufferLayoutSource
	{
		std::string m_NameBuffer;
		BufferType m_BufferType;
		std::vector<ElementSource> m_Layout;

	};
	
	struct BufferSource
	{
		std::string m_NameBuffer;
		BufferType m_BufferType;
		std::string m_Source;
	};

	class RenderPipline : public Asset
	{
	public:
		RenderPipline();
		~RenderPipline();


		static AssetType GetStaticType() { return AssetType::RenderPipline; }
		AssetType GetType() const override { return GetStaticType(); }
	private:
		BufferSource m_BufferSource;
		BufferLayoutSource m_BufferLayoutSource;
	};
}
