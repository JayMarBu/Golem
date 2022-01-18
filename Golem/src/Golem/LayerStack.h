#pragma once

#include "Golem/Core.h"
#include "Layer.h"

namespace golem
{
	class LayerStack
	{
		// Members ********************************************************************************
	private:
		std::vector<Layer*> m_layers;
		unsigned int m_layerInsertIndex = 0;
		// Methods ********************************************************************************	
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		std::vector<Layer*>::iterator begin() {return m_layers.begin();}
		std::vector<Layer*>::iterator end() {return m_layers.end();}
	};
}

