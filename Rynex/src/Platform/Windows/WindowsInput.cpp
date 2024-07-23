#include "rypch.h"
#include "Rynex/Core/Input.h"
#include "Rynex/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Rynex {
#if 0
	bool Input::IsKeyPressedImpl(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtenPressdImpl(int botton)
	{

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, botton);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	std::pair<float, float> Input::GetMousePostionImpl()
	{

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double posX, posY;
		glfwGetCursorPos(window, &posX, &posY);

		return {(float)posX, (float)posY };
	}

	float Input::GetMouseXImpl()
	{
		return GetMousePostionImpl().first;
	}

	float Input::GetMouseYImpl()
	{
		return GetMousePostionImpl().second;
	}
#endif


	bool Input::IsKeyPressed(const KeyCode key)
	{
		RY_PROFILE_FUNCTION();
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}
	
	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		RY_PROFILE_FUNCTION();
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		RY_PROFILE_FUNCTION();
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		RY_PROFILE_FUNCTION();
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		RY_PROFILE_FUNCTION();
		return GetMousePosition().y;
	}

}