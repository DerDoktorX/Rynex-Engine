#pragma once

#include "Rynex/Renderer/Camera/Camera.h"

namespace Rynex{

	class SceneCamera : public Camera
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
	private:
		void RecalulateProjection();
	private:
		ProjectionType m_ProjektionType = ProjectionType::Orthographic;

		float m_OrthoGraficSizen = 10.0f;
		float m_OrthoGraficNear = -1.0f, m_OrthoGraficFar = 1.0f;

		float m_PerspectivFOV = glm::radians(45.0f);
		float m_PerspectivNear = 0.01f, m_PerspectivFar = 1000.0f;

		float m_AspectRotatio = 0.0f;
	};

}