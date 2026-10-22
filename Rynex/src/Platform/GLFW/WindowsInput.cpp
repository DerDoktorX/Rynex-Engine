#include <rypch.h>
#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>

#include <GLFW/glfw3.h>


namespace Rynex {


	bool Input::IsKeyPressed(const KeyCode key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	    const int32_t keyCode = static_cast<int32_t>(key);
		const int state = glfwGetKey(window, keyCode);
		return GLFW_PRESS == state;
	}
	
	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	    const int32_t buttonCode = static_cast<int32_t>(button);
		const int state = glfwGetMouseButton(window, buttonCode);
		return GLFW_PRESS == state;
	}

	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return glm::vec2{ static_cast<float>(xPos), static_cast<float>(yPos) };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}