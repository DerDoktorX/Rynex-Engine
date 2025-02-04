#include "rypch.h"
#include "ScenenCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

	std::array<glm::vec4, 8> SceneCamera::m_ViewFustrum = {
		glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // nbl = near, bottem, left
		glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f), // ntl = near, top, left
		glm::vec4( 1.0f, -1.0f, -1.0f, 1.0f), // nbr = near, bottem, rigth
		glm::vec4( 1.0f,  1.0f, -1.0f, 1.0f), // ntr = near, top, rigth

		glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f), // fbl = far, bottem, left
		glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f), // ftl = far, top, left
		glm::vec4( 1.0f, -1.0f,  1.0f, 1.0f), // fbr = far, bottem, rigth
		glm::vec4( 1.0f,  1.0f,  1.0f, 1.0f), // ftr = far, top, rigth
	};

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

	
	

	glm::vec4 SceneCamera::GetWorldCameraCenter(const glm::mat4& view) const
	{
		float nearPlane, farPlane;
		switch (m_ProjektionType)
		{
		case ProjectionType::Perspectiv:
		{
			farPlane = m_PerspectivFar;
			nearPlane = m_PerspectivNear;
			break;
		}
			
		case  ProjectionType::Orthographic:
		{
			
			farPlane = m_OrthoGraficFar;
			nearPlane = m_OrthoGraficNear;
			break;
		}
			
		default:
			RY_CORE_ASSERT(false);
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
			
		return view * glm::vec4(0.0f, 0.0f, -(nearPlane + farPlane ) / 2.0f, 1.0f);
	}

	glm::vec4 SceneCamera::GetWorldCameraCenterMax(const glm::mat4& view, float max) const
	{
		float nearPlane, farPlane;
		switch (m_ProjektionType)
		{
		case ProjectionType::Perspectiv:
		{
			farPlane = m_PerspectivFar;
			nearPlane = m_PerspectivNear;
			break;
		}

		case  ProjectionType::Orthographic:
		{

			farPlane = m_OrthoGraficFar;
			nearPlane = m_OrthoGraficNear;
			break;
		}

		default:
			RY_CORE_ASSERT(false);
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		return view * glm::vec4(0.0f, 0.0f, -(nearPlane + max) / 2.0f, 1.0f);
	}

	glm::vec4 SceneCamera::GetWorldCameraCenterMin(const glm::mat4& view, float min) const
	{
		float nearPlane, farPlane;
		switch (m_ProjektionType)
		{
		case ProjectionType::Perspectiv:
		{
			farPlane = m_PerspectivFar;
			nearPlane = m_PerspectivNear;
			break;
		}

		case  ProjectionType::Orthographic:
		{

			farPlane = m_OrthoGraficFar;
			nearPlane = m_OrthoGraficNear;
			break;
		}

		default:
			RY_CORE_ASSERT(false);
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		return view * glm::vec4(0.0f, 0.0f, -(min + farPlane) / 2.0f, 1.0f);
	}


	std::array<glm::vec4, 8> SceneCamera::GetViewFustrumWorld(const glm::mat4& view) const
	{
		std::array<glm::vec4, 8> viewFustremWorld = m_ViewFustrum;
		glm::mat4 VP = glm::inverse(m_Projektion * view);
		for (auto& fust : viewFustremWorld)
			fust = VP * fust;
		return viewFustremWorld;
	}

	std::array<glm::vec4, 8> SceneCamera::GetViewFustrumWorld(const glm::mat4& view, const glm::mat4& projetion)
	{
		std::array<glm::vec4, 8> viewFustremWorld = m_ViewFustrum;
		glm::mat4 iVP = glm::inverse(projetion * view);
		return GetInverseViewProjetionFustrumWorld(iVP);
	}

	std::array<glm::vec4, 8> SceneCamera::GetViewProjetionFustrumWorld(const glm::mat4& viewProjetion)
	{
		glm::mat4 iVP = glm::inverse(viewProjetion);
		return GetInverseViewProjetionFustrumWorld(iVP);
	}

	std::array<glm::vec4, 8> SceneCamera::GetInverseViewProjetionFustrumWorld(const glm::mat4& inverseViewProjetion)
	{
		std::array<glm::vec4, 8> viewFustremWorld = m_ViewFustrum;
		for (auto& fust : viewFustremWorld)
			fust = inverseViewProjetion * fust;
		return viewFustremWorld;
	}

	std::pair<glm::vec3, glm::vec3> SceneCamera::GetMinMaxViewFustrumInSpace(const glm::mat4& spaceMatrix)
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
		for (glm::vec4& fust : m_ViewFustrum)
		{
			fust = spaceMatrix * fust;
			glm::vec3 pos = glm::vec3(fust) / fust.w;
			max = glm::max(max, pos);
			min = glm::min(min, pos);
		}
		return std::pair<glm::vec3, glm::vec3>(min, max);
	}

	std::pair<glm::vec3, glm::vec3> SceneCamera::GetMinMaxViewFustrumInSpace(const glm::mat4& spaceMatrix, const std::array<glm::vec4, 8>& trasformtViewFustrum)
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
		for (const glm::vec4& fust : trasformtViewFustrum)
		{
			glm::vec4 pos4 = spaceMatrix * fust;
			glm::vec3 pos3 = glm::vec3(pos4 / pos4.w) ;
			max = glm::max(max, pos3);
			min = glm::min(min, pos3);
		}
		return std::pair<glm::vec3, glm::vec3>(min, max);
	}

	glm::mat4 SceneCamera::GetShadowViewMatrix(const glm::vec3& center, const glm::vec3& direction)
	{
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		return glm::lookAt(center - direction, center, up);
	}

	void SceneCamera::RecalulateProjection()
	{
		
		if(m_ProjektionType == ProjectionType::Perspectiv)
		{			
			m_Projektion = glm::perspective(m_PerspectivFOV, m_AspectRotatio, m_PerspectivNear, m_PerspectivFar);
		}
		else
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
		}

		
	}

	

}