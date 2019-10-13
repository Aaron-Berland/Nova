#include "LayerStack.h"
#include <algorithm>

namespace Nova
{

	LayerStack::LayerStack()
	{
		
	}


	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
		layer->OnAttach();
		m_layerInsertIndex++;
	}



	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		iterator next = std::next(m_layers.begin() + m_layerInsertIndex, 1);
		iterator it = std::find(m_layers.begin(), next, layer);
		
		if (it != next)
		{
			(*it)->OnDetach();
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		iterator it = std::find(m_layers.begin() + m_layerInsertIndex, m_layers.end(), overlay);
		if (it != m_layers.end())
		{
			(*it)->OnDetach();
			m_layers.erase(it);
		}
			

	}

}