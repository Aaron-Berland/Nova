#pragma once 
#ifndef IMGUILAYER_H
#define IMGUILAYER_H
#include <Nova/Core.h>
#include <Nova/Layer.h>
#include <Nova/Events/ApplicationEvent.h>
#include <Nova/Events/KeyEvent.h>
#include <Nova/Events/MouseEvent.h>
#include <memory.h>
namespace Nova
{
	class NOVA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer() : Layer("ImGuiLayer") {}
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual ~ImGuiLayer() = default;
	protected:
		float m_time = 0.0f;

	};

	ImGuiLayer* CreateImGuiLayer();
}

#endif