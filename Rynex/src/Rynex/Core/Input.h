#pragma once

#include "Rynex/Core/Base.h"
#include "Rynex/Core/KeyCodes.h"
#include "Rynex/Core/MouseCodes.h"


#include <glm/glm.hpp>


namespace Rynex {
	class RYNEX_API Input
	{
	public:
#if 0
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtenPressd(int botton) { return s_Instance->IsMouseButtenPressdImpl(botton); }
		
		inline static std::pair<float, float> GetMousePostion() { return s_Instance->GetMousePostionImpl(); }
		
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }		
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
#endif
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
#if 0
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;

		virtual bool IsMouseButtenPressdImpl(int botton) = 0;

		virtual std::pair<float, float> GetMousePostionImpl() = 0;

		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input* s_Instance;
#endif
	};
}