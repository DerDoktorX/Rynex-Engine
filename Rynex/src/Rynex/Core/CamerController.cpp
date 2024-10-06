#include "rypch.h"
#include "CamerController.h"

#include "Rynex/Core/Input.h"
#include "Rynex/Core/KeyCodes.h"

namespace Rynex {
	OrthograficCameraController::OrthograficCameraController(float aspectRotation, bool rotatione)
		: m_AspectRotation(aspectRotation)
		, m_Camera(-m_AspectRotation * m_ZoomLevel, m_AspectRotation * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
		, m_Rotatione(rotatione)
		
	{
	}

	void OrthograficCameraController::OnUpdate(TimeStep ts)
	{
		//if (Input::IsMouseButtenPressd(0)) {
		//	m_CamerPosition += glm::vec3(m_MouseOffset * 0.005f,0.0);
		//	
		//	RY_CORE_INFO("MousPos: {0}, {1}, {2}", m_CamerPosition.x, m_CamerPosition.y, m_CamerPosition.z);
		//}

		if (Input::IsKeyPressed(Key::W))
			m_CamerPosition.y += m_CamerSpeed * ts;
		
		else if (Input::IsKeyPressed(Key::S))
			m_CamerPosition.y -= m_CamerSpeed * ts;

		if (Input::IsKeyPressed(Key::D))
			m_CamerPosition.x += m_CamerSpeed * ts;
		else if (Input::IsKeyPressed(Key::A))
			m_CamerPosition.x -= m_CamerSpeed * ts;

		if (m_Rotatione) {
			if (Input::IsKeyPressed(Key::Q))
				m_CamerRotation += m_CamerSpeed * ts;

			else if (Input::IsKeyPressed(Key::E))
				m_CamerRotation -= m_CamerSpeed * ts;

			m_Camera.SetRotation(m_CamerRotation);
		}
		
		m_Camera.SetPostione(m_CamerPosition);
	}

	void OrthograficCameraController::OnEnvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseSrolledEvent>(RY_BIND_EVENT_FN(OrthograficCameraController::OnMouseSrolled));
		
		dispatcher.Dispatch<MouseMoveEvent>(RY_BIND_EVENT_FN(OrthograficCameraController::OnMouseMoved));
		dispatcher.Dispatch<WindowResizeEvent>(RY_BIND_EVENT_FN(OrthograficCameraController::OnWindowResize));
	}

	void OrthograficCameraController::OnResize(float withe, float heigth)
	{
		m_AspectRotation = withe / heigth;
		m_Camera.SetProjection(-m_AspectRotation * m_ZoomLevel, m_AspectRotation * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthograficCameraController::OnMouseSrolled(MouseSrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRotation * m_ZoomLevel, m_AspectRotation * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthograficCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
	bool OrthograficCameraController::OnMouseMoved(MouseMoveEvent& e)
	{
		m_MouseOffset = glm::vec2(m_MousePos) - glm::vec2(e.GetX() - 640, e.GetY() - 370);
		m_MousePos = { e.GetX()-640, e.GetY()-370 };
		return false;
	}

	//bool OrthograficCameraController::OnMouseButtenPresed(MouseButtenPressedEvent& e)
	//{
	//	if(e.GetMouseButton() == 1)
	//		return true;
	//	return false;
	//}
	
}