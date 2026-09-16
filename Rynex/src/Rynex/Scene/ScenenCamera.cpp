#include "rypch.h"
#include "ScenenCamera.h"

namespace Rynex {

	std::array<glm::vec4, 8> SceneCamera::m_ViewFrustumEdges = {
		glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // nbl = near, bottem, left
		glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f), // ntl = near, top, left
		glm::vec4( 1.0f, -1.0f, -1.0f, 1.0f), // nbr = near, bottem, right
		glm::vec4( 1.0f,  1.0f, -1.0f, 1.0f), // ntr = near, top, right

		glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f), // fbl = far, bottem, left
		glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f), // ftl = far, top, left
		glm::vec4( 1.0f, -1.0f,  1.0f, 1.0f), // fbr = far, bottem, right
		glm::vec4( 1.0f,  1.0f,  1.0f, 1.0f), // ftr = far, top, right
	};

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
		
	}

	void SceneCamera::SetOrthoGraphic(float sizen, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize	= sizen;
		m_OrthographicNear	= nearClip;
		m_OrthographicFar	= farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticleFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticleFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewPortSize(uint32_t withe, uint32_t height)
	{
		m_AspectRotation = static_cast<float>(withe) / static_cast<float>(height);
		m_ViewAspect = { static_cast<int>(withe), static_cast<int>(height) };
		RecalculateProjection();
	}

	
	

	glm::vec4 SceneCamera::GetWorldCameraCenter(const glm::mat4& view) const
	{
		float nearPlane, farPlane;
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
		{
			farPlane = m_PerspectiveFar;
			nearPlane = m_PerspectiveNear;
			break;
		}
			
		case  ProjectionType::Orthographic:
		{
			
			farPlane = m_OrthographicFar;
			nearPlane = m_OrthographicNear;
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
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
		{
			farPlane = m_PerspectiveFar;
			nearPlane = m_PerspectiveNear;
			break;
		}

		case  ProjectionType::Orthographic:
		{

			farPlane = m_OrthographicFar;
			nearPlane = m_OrthographicNear;
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
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
		{
			farPlane = m_PerspectiveFar;
			nearPlane = m_PerspectiveNear;
			break;
		}

		case  ProjectionType::Orthographic:
		{

			farPlane = m_OrthographicFar;
			nearPlane = m_OrthographicNear;
			break;
		}

		default:
			RY_CORE_ASSERT(false);
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		return view * glm::vec4(0.0f, 0.0f, -(min + farPlane) / 2.0f, 1.0f);
	}


	std::array<glm::vec4, 8> SceneCamera::GetViewFrustumWorld(const glm::mat4& view) const
	{
		std::array<glm::vec4, 8> viewFustremWorld = m_ViewFrustumEdges;
		glm::mat4 VP = glm::inverse(m_Projektion * view);
		for (auto& fust : viewFustremWorld)
			fust = VP * fust;
		return viewFustremWorld;
	}

	float SceneCamera::GetWorldViewFrustumRadius(const glm::mat4& view)
	{
		std::array<glm::vec4, 8> viewFustrum = SceneCamera::GetInverseViewProjectionFrustumWorld(view);
		float radius = 0.0f;
		for (glm::vec4& pos : viewFustrum)
		{
			for (uint8_t i = 0; i < 3; i++)
			{
				float value = pos[i];
				radius = value > radius ? value : radius;
			}
		}

		return radius;
	}

	std::array<glm::vec4, 8> SceneCamera::GetViewFrustumWorld(const glm::mat4& view, const glm::mat4& projetion)
	{
		std::array<glm::vec4, 8> viewFustremWorld = m_ViewFrustumEdges;
		glm::mat4 iVP = glm::inverse(projetion * view);
		return GetInverseViewProjectionFrustumWorld(iVP);
	}

	std::array<glm::vec4, 8> SceneCamera::GetViewProjectionFrustumWorld(const glm::mat4& viewProjetion)
	{
		glm::mat4 iVP = glm::inverse(viewProjetion);
		return GetInverseViewProjectionFrustumWorld(iVP);
	}

	std::array<glm::vec4, 8> SceneCamera::GetInverseViewProjectionFrustumWorld(const glm::mat4& inverseViewProjetion)
	{
		std::array<glm::vec4, 8> viewFustremWorld = m_ViewFrustumEdges;
		for (glm::vec4& fust : viewFustremWorld)
			fust = inverseViewProjetion * fust;
		return viewFustremWorld;
	}

	std::pair<glm::vec3, glm::vec3> SceneCamera::GetMinMaxViewFrustumInSpace(const glm::mat4& spaceMatrix)
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
		for (glm::vec4& fust : m_ViewFrustumEdges)
		{
			fust = spaceMatrix * fust;
			glm::vec3 pos = glm::vec3(fust) / fust.w;
			max = glm::max(max, pos);
			min = glm::min(min, pos);
		}
		return std::pair<glm::vec3, glm::vec3>(min, max);
	}

	std::pair<glm::vec3, glm::vec3> SceneCamera::GetMinMaxViewFrustumInSpace(const glm::mat4& spaceMatrix, const std::array<glm::vec4, 8>& transformViewFrustum)
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
		for (const glm::vec4& fust : transformViewFrustum)
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

	void SceneCamera::RecalculateProjection()
	{
		
		if(m_ProjectionType == ProjectionType::Perspective)
		{			
			m_Projektion = glm::perspective(m_PerspectiveFOV, m_AspectRotation, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRotation * 0.5f;
			float orthoRigth = m_OrthographicSize * m_AspectRotation * 0.5f;
			float orthoBottem = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projektion = glm::ortho(
				orthoLeft, orthoRigth,
				orthoBottem, orthoTop,
				m_OrthographicNear, m_OrthographicFar
			);
		}

		
	}

	

}