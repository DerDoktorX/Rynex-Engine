#include <rypch.h>
#include "LayerStack.h"

namespace Rynex {
	LayerStack::LayerStack()
	{
	}
	
	LayerStack::~LayerStack()
	{
		for (Ref<Layer>& layer : m_Layers)
		{
			RY_DESTROY_REF(layer);
		}
	    m_Layers.clear();
	}

	void LayerStack::PushLayer(Ref<Layer>& layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerIntersertIndex, layer);
		m_LayerIntersertIndex++;
	}

	void LayerStack::PushOverlay(Ref<Layer>& overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
	    auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
	    [layer](const Ref<Layer>& layerTest) -> bool
        {
            return layerTest.get() == layer;
        });
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerIntersertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
	    auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
	        [overlay](const Ref<Layer>& layerTest) -> bool
            {
                return layerTest.get() == overlay;
            });
		if (it != m_Layers.end())
			m_Layers.erase(it);
	}
}