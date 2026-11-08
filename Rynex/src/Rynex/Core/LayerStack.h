#pragma once

#include <Rynex/Core/Layer.h>

namespace Rynex{

	class RYNEX_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Ref<Layer>& layer);
		void PushOverlay(Ref<Layer>& overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Ref<Layer>>::iterator begin() { return m_Layers.begin(); }
		std::vector<Ref<Layer>>::iterator end() { return m_Layers.end(); }

		
	private:
		std::vector<Ref<Layer>> m_Layers;
		uint32_t m_LayerIntersertIndex = 0;
	};

}

