#pragma once
#include "Rynex/Core/TimeStep.h"
#include "Rynex/Events/ApplicationEvent.h";
#include "Rynex/Events/MouseEvent.h";
#include "Rynex/Renderer/Camera/OrthograficCamera.h"

namespace Rynex {
	class RYNEX_API OrthograficCameraController
	{
	public:
		OrthograficCameraController(float aspectRotation, bool rotatione = false);

		void OnUpdate(TimeStep ts);
		void OnEnvent(Event& e);

		void OnResize(float withe, float heigth);

		OrthograficCamera& GetCamera() { return m_Camera; }
		inline const OrthograficCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() const { return m_ZoomLevel; }

	private:
		bool OnMouseSrolled(MouseSrolledEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMoveEvent& e);
		//bool OnMouseButtenPresed(MouseButtenPressedEvent& e);
	private:
		float m_AspectRotation;
		float m_ZoomLevel = 1.0f;
		bool m_Rotatione;
		glm::vec3 m_CamerPosition = { 0.0f,0.0f,0.0f };
		float m_CamerRotation = 0.0f;
		glm::vec2 m_MousePos = { 0.0, 0.0 };
		glm::vec2 m_MouseOffset;
		float m_CamerSpeed = 2.0;
		OrthograficCamera m_Camera;
	};
}


