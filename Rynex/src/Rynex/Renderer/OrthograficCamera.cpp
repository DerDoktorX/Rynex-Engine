#include "rypch.h"
#include "OrthograficCamera.h"

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Rynex {

	OrthograficCamera::OrthograficCamera(float left, float rigth, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, rigth, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		RY_PROFILE_FUNCTION();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}


	void OrthograficCamera::SetProjection(float left, float rigth, float bottom, float top)
	{
		RY_PROFILE_FUNCTION();
		m_ProjectionMatrix = glm::ortho(left, rigth, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthograficCamera::RecalculateViewMatrix()
	{
		RY_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
			glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}