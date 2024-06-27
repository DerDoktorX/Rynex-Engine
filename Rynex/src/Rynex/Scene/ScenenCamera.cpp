#include "rypch.h"
#include "ScenenCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Rynex {

	SceneCamera::SceneCamera()
	{
		RecalulateProjection();
	}

	void SceneCamera::SetOrthoGrafic(float sizen, float nearClip, float farClip)
	{
		m_ProjektionType = ProjectionType::Orthographic;
		m_OrthoGraficSizen	= sizen;
		m_OrthoGraficNear	= nearClip;
		m_OrthoGraficFar	= farClip;

		RecalulateProjection();
	}

	void SceneCamera::SetPerspectiv(float verticleFow, float nearClip, float farClip)
	{
		m_ProjektionType = ProjectionType::Perspectiv;
		m_PerspectivFOV = verticleFow;
		m_PerspectivNear = nearClip;
		m_PerspectivFar = farClip;

		RecalulateProjection();
	}

	void SceneCamera::SetViewPortSize(uint32_t withe, uint32_t heigth)
	{
		m_AspectRotatio = (float)withe / (float)heigth;

		RecalulateProjection();
	}

	void SceneCamera::RecalulateProjection()
	{
		switch (m_ProjektionType)
		{
			case ProjectionType::Orthographic:
		{
			float orthoLeft = -m_OrthoGraficSizen * m_AspectRotatio * 0.5f;
			float orthoRigth = m_OrthoGraficSizen * m_AspectRotatio * 0.5f;
			float orthoBottem = -m_OrthoGraficSizen * 0.5f;
			float orthoTop = m_OrthoGraficSizen * 0.5f;

			m_Projektion = glm::ortho(
				orthoLeft, orthoRigth,
				orthoBottem, orthoTop,
				m_OrthoGraficNear, m_OrthoGraficFar
			);
			break;
		}
			case ProjectionType::Perspectiv:
			{
				m_Projektion = glm::perspective(m_PerspectivFOV, m_AspectRotatio, m_PerspectivNear, m_PerspectivFar);
				break;
			}
		};
	}

}