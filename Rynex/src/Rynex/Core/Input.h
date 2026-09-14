#pragma once

#include <Rynex/Core/Base.h>
#include <Rynex/Core/KeyCodes.h>
#include <Rynex/Core/MouseCodes.h>



namespace Rynex {
	class RYNEX_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}