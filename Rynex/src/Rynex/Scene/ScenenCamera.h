#pragma once

#include "Rynex/Renderer/Camera/Camera.h"
#include "Rynex/Renderer/Objects/BoxAABB.h"

namespace Rynex{


	struct ViewFustrumPoints
	{
		glm::vec4 nbl;
		glm::vec4 ntl;
		glm::vec4 nbr;
		glm::vec4 ntr;

		glm::vec4 fbl;
		glm::vec4 ftl;
		glm::vec4 fbr;
		glm::vec4 ftr;
	};


	class RYNEX_API SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspectiv = 0, Orthographic = 1 };
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetViewPortSize(uint32_t withe, uint32_t heigth);


		void SetOrthoGrafic(float sizen, float nearClip, float farClip);
		void SetPerspectiv(float verticleFow, float nearClip, float farClip);

		void SetOrthograficSize(float size) { m_OrthoGraficSizen = size; RecalulateProjection(); }
		void SetOrthograficNearClipe(float nearClip) { m_OrthoGraficNear = nearClip; RecalulateProjection(); }
		void SetOrthograficFarClipe(float farClipe) { m_OrthoGraficFar = farClipe; RecalulateProjection(); }
		
		float GetOrthographicSize() const { return m_OrthoGraficSizen; }
		float GetOrthographicNearClipe() const { return m_OrthoGraficNear; }
		float GetOrthographicFarClipe() const { return m_OrthoGraficFar; }
		


		void SetPerspectivVerticleFOV(float verticleVow) { m_PerspectivFOV = verticleVow; RecalulateProjection(); }
		void SetPerspectivNearClipe(float nearClip) { m_PerspectivNear = nearClip; RecalulateProjection(); }
		void SetPerspectivFarClipe(float farClipe) { m_PerspectivFar = farClipe; RecalulateProjection(); }
		
		float GetPerspectivVerticleFOV() const { return m_PerspectivFOV; }
		float GetPerspectivNearClipe() const { return m_PerspectivNear; }
		float GetPerspectivFarClipe() const { return m_PerspectivFar; }


		ProjectionType GetProjectionType() const{ return m_ProjektionType; }
		void SetProjectionType(ProjectionType type) { m_ProjektionType = type; RecalulateProjection(); }

		
		glm::vec4 GetWorldCameraCenter(const glm::mat4& view) const;

		glm::vec4 GetWorldCameraCenterMax(const glm::mat4& view, float max) const;
		glm::vec4 GetWorldCameraCenterMin(const glm::mat4& view, float min) const;

		static const std::array<glm::vec4, 8>& GetViewFustrum() { return m_ViewFustrum; }
		std::array<glm::vec4, 8> GetViewFustrumWorld(glm::mat4& view)const;

		static std::pair<glm::vec3, glm::vec3> GetMinMaxViewFustrumInSpace(const glm::mat4& spaceMatrix);
		static std::pair<glm::vec3, glm::vec3> GetMinMaxViewFustrumInSpace(const glm::mat4& spaceMatrix, const std::array<glm::vec4, 8>& trasformtViewFustrum);
		static glm::mat4 GetShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction);
	private:
		void RecalulateProjection();
	private:
		ProjectionType m_ProjektionType = ProjectionType::Orthographic;
		
		float m_OrthoGraficSizen = 10.0f;
		float m_OrthoGraficNear = -1.0f, m_OrthoGraficFar = 1.0f;

		float m_PerspectivFOV = glm::radians(45.0f);
		float m_PerspectivNear = 0.01f, m_PerspectivFar = 1000.0f;

		float m_AspectRotatio = 0.0f;
		
		glm::vec3 m_Min;
		glm::vec3 m_Max;
		static std::array<glm::vec4, 8> m_ViewFustrum;
	};

}