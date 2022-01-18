#include "golpch.h"
#include "LayerStack.h"

namespace golem
{

	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for(Layer* layer : m_layers)
		{
			CLEANUP(layer)
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
		m_layerInsertIndex++;
		layer->OnAttach();
		layer->SetAttached(true);
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_layers.emplace_back(overlay);
		overlay->OnAttach();
		overlay->SetAttached(true);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if(it != m_layers.end())
		{
			layer->SetAttached(false);
			layer->OnDetach();
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
		if(it != m_layers.end())
		{
			overlay->SetAttached(false);
			overlay->OnDetach();
			m_layers.erase(it);
		}
	}

}