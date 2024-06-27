#pragma once

#if 0
#include "Rynex/Core/Input.h"

namespace Rynex {
	class WindowsInput : public Input
	{
	protected:

		virtual bool IsKeyPressedImpl(KeyCode keycode) override;

		virtual bool IsMouseButtenPressdImpl(int botton) override;
		virtual std::pair<float, float> GetMousePostionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;

	};
}
#endif
