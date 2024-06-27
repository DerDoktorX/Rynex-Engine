#pragma once

#include <glm/glm.hpp>

namespace Rynex {
	class OrthograficCamera
	{
	public:
		OrthograficCamera(float left, float rigth, float bottom, float top/*, float near = 1.0f, float far = -1.0f*/);


		

		const glm::vec3& GetPostion() const { return m_Position; }
		void SetPostione(const glm::vec3& postion) { m_Position = postion; RecalculateViewMatrix(); }
		
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		float GetRotation() const { return m_Rotation; }

		void SetProjection(float left, float rigth, float bottom, float top);

		const glm::mat4& GetProjectionMatrix()		const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix()				const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix()	const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix; 
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f,0.0f,0.0f};
		float m_Rotation = 0.0f;
	};

	
}