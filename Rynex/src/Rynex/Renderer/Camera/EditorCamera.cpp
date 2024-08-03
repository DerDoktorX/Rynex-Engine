#include "rypch.h"
#include "EditorCamera.h"


#include "Rynex/Core/Input.h"
#include "Rynex/Core/KeyCodes.h"
#include "Rynex/Core/MouseCodes.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Rynex {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		RY_PROFILE_FUNCTION();
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		RY_PROFILE_FUNCTION();
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projektion = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		RY_PROFILE_FUNCTION();
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void EditorCamera::FreeCameraUpdate()
	{
		RY_PROFILE_FUNCTION();
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
			MousePan(delta);
		else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
			MouseRotate(delta);
		else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
			MouseZoom(delta.y);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		RY_PROFILE_FUNCTION();
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		RY_PROFILE_FUNCTION();
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		RY_PROFILE_FUNCTION();
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(TimeStep ts)
	{
		RY_PROFILE_FUNCTION();
		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			if (m_OnlyOnPressed)
				FreeCameraUpdate();
			else
				m_FreeCamerMove = m_FreeCamerMove ? false : true;
		}
			
		
		if (m_FreeCamerMove)
			FreeCameraUpdate();

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		RY_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseSrolledEvent>(RY_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseSrolledEvent& e)
	{
		RY_PROFILE_FUNCTION();
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		RY_PROFILE_FUNCTION();
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		RY_PROFILE_FUNCTION();
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		RY_PROFILE_FUNCTION();
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		RY_PROFILE_FUNCTION();
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		RY_PROFILE_FUNCTION();
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		RY_PROFILE_FUNCTION();
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		RY_PROFILE_FUNCTION();
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		RY_PROFILE_FUNCTION();
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}