#pragma once

#include "Rynex/Core/Layer.h"
#include "Rynex/Core/Base.h"

#include "Rynex/Events/Event.h"
#include "Rynex/Events/KeyEvent.h"
#include "Rynex/Events/MouseEvent.h"
#include "Rynex/Events/ApplicationEvent.h"

namespace Rynex {

	class RYNEX_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;



		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColore();
	private:
		bool m_BlockEvents = true;
		float m_Time;
	};
}

