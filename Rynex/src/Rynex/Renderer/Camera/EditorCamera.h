#pragma once

#include "Camera.h"

#include "Rynex/Core/Timestep.h"
#include "Rynex/Events/Event.h"
#include "Rynex/Events/MouseEvent.h"

#include <glm/glm.hpp>


namespace Rynex {
	class RYNEX_API EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projektion * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetPosition() const { return glm::vec3(m_ViewMatrix[3].x, m_ViewMatrix[3].y, m_ViewMatrix[3].z); }
		glm::quat GetOrientation() const;
		const glm::vec3& GetWorldPostionCenterView() const { return m_Center; }
		const std::array<glm::vec3, 8>& GetViewFustrem() const { return m_ViewFustrum; }
		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		void SetModeFreeCamerMove(bool mode = false) { m_FreeCamerMove = mode; }
		void SetModeOnlyOnPressed(bool mode = false) { m_OnlyOnPressed = mode; }
	private:
		void UpdateProjection();
		void UpdateView();
		void UpdateCenter();
		void UpdateAABB();

		void FreeCameraUpdate();

		bool OnMouseScroll(MouseSrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Center;
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;

		bool m_FreeCamerMove = false;
		bool m_OnlyOnPressed = true;
		glm::vec3 m_Min;
		glm::vec3 m_Max;

		std::array<glm::vec3, 8> m_ViewFustrum;
	};

}


