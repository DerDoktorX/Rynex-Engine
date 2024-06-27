#pragma once

#include <glm/glm.hpp>

namespace Rynex {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projektion)
			:m_Projektion(projektion) {}

		const glm::mat4& GetProjektion() const { return m_Projektion; }
	protected:
		glm::mat4 m_Projektion = glm::mat4(1.0f);
	};

	

}