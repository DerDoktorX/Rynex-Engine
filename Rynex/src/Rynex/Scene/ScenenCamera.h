#pragma once

#include <Rynex/Renderer/Camera/Camera.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>

namespace Rynex{


	struct ViewFrustumEdges
	{
		glm::vec4 m_NearBottleLeft;
		glm::vec4 m_NearTopLeft;
		glm::vec4 m_NearBottleRight;
		glm::vec4 m_NearTopRight;

		glm::vec4 m_FarBottleLeft;
		glm::vec4 m_FarTopLeft;
		glm::vec4 m_FarBottleRight;
		glm::vec4 m_FarTopRight;
	};


	class RYNEX_API SceneCamera : public Camera
	{
	public:
		enum class ProjectionType : uint8_t { Perspective = 0, Orthographic = 1 };
	// public member methods --------------------------------------------------------------------------------------------------
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetViewPortSize(uint32_t withe, uint32_t height);


		void SetOrthoGraphic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFow, float nearClip, float farClip);

		void SetOrthographicSize(const float size) { m_OrthographicSize = size; RecalculateProjection(); }
		void SetOrthographicNearClip(const float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		void SetOrthographicFarClip(const float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }
		
		float GetOrthographicSize() const { return m_OrthographicSize; }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		


		void SetPerspectiveVerticalFOV(const float verticalVow) { m_PerspectiveFOV = verticalVow; RecalculateProjection(); }
		void SetPerspectiveNearClip(const float nearCli) { m_PerspectiveNear = nearCli; RecalculateProjection(); }
		void SetPerspectiveFarClip(const float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }
		
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		float GetAspectRotation() const { return m_AspectRotation; }
		glm::ivec2 GetViewAspect() const { return m_ViewAspect; }
		
		ProjectionType GetProjectionType() const{ return m_ProjectionType; }
		void SetProjectionType(const ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

		
		glm::vec4 GetWorldCameraCenter(const glm::mat4& view) const;

		glm::vec4 GetWorldCameraCenterMax(const glm::mat4& view, float max) const;
		glm::vec4 GetWorldCameraCenterMin(const glm::mat4& view, float min) const;

		static const std::array<glm::vec4, 8>& GetViewFrustum() { return m_ViewFrustumEdges; }
		std::array<glm::vec4, 8> GetViewFrustumWorld(const glm::mat4& view)const;
		static float GetWorldViewFrustumRadius(const glm::mat4& view);
		static std::array<glm::vec4, 8> GetViewFrustumWorld(const glm::mat4& view, const glm::mat4& projection);
		static std::array<glm::vec4, 8> GetViewProjectionFrustumWorld(const glm::mat4& viewProjection);
		static std::array<glm::vec4, 8> GetInverseViewProjectionFrustumWorld(const glm::mat4& inverseViewProjection);

		static std::pair<glm::vec3, glm::vec3> GetMinMaxViewFrustumInSpace(const glm::mat4& spaceMatrix);
		static std::pair<glm::vec3, glm::vec3> GetMinMaxViewFrustumInSpace(const glm::mat4& spaceMatrix, const std::array<glm::vec4, 8>& transformViewFrustum);
		static glm::mat4 GetShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction);
		
	private:
		void RecalculateProjection();
	// private member variables -----------------------------------------------------------------------------------------------
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

		float m_AspectRotation = 1.0f;
		glm::ivec2 m_ViewAspect = glm::ivec2{ 1, 1 };
		glm::vec3 m_Min;
		glm::vec3 m_Max;
		static std::array<glm::vec4, 8> m_ViewFrustumEdges;
	};

}