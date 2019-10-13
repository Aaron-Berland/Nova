#pragma once
#ifndef LAYER_STACK_H
#define LAYER_STACK_H
#include <Nova/Core.h>
#include <Nova/Layer.h>
#include <vector>

namespace Nova {
	class NOVA_API LayerStack
	{
	public:
		using iterator = std::vector<Layer*>::iterator;
		using size_type = std::vector<Layer*>::size_type;
		LayerStack();
		~LayerStack();


		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);
		iterator begin() { return m_layers.begin(); }
		iterator end() { return m_layers.end(); }
	private:
		std::vector<Layer*> m_layers;
		size_type m_layerInsertIndex = 0;

	};
}


#endif