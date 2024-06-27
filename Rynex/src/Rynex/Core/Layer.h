#pragma once

#include "Rynex/Core/Base.h"
#include "Rynex/Events/Event.h"
#include "Rynex/Core/TimeStep.h"

namespace Rynex{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugeName; }

	protected:
		std::string m_DebugeName;
	};
}

