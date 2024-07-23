#include "rypch.h"
#include "LayerStack.h"

namespace Rynex {
	LayerStack::LayerStack()
	{
		RY_PROFILE_FUNCTION();
	}
	
	LayerStack::~LayerStack()
	{
		RY_PROFILE_FUNCTION();
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		RY_PROFILE_FUNCTION();
		m_Layers.emplace(m_Layers.begin() + m_LayerIntersertIndex, layer);
		m_LayerIntersertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		RY_PROFILE_FUNCTION();
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		RY_PROFILE_FUNCTION();
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerIntersertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		RY_PROFILE_FUNCTION();
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
			m_Layers.erase(it);
	}
}