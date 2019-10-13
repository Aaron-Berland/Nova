#pragma once
#ifndef LAYER_H
#define LAYER_H
#include <Nova/Core/Timestep.h>
#include <Nova/Core.h>
#include <string>
#include <Nova/Events/Event.h>
namespace Nova
{

	class NOVA_API Layer
	{
	public:
		Layer(const std::string &name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event &e) {}
		virtual void OnImGuiRender() {}
		inline const std::string& GetName() const { return m_debugName; }

	private:
		std::string m_debugName;
	};
}



#endif