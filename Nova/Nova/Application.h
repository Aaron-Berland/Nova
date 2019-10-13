#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <Nova/Core.h>
#include <Nova/LayerStack.h>
#include <Nova/Events/Event.h>
#include <Nova/Events/ApplicationEvent.h>
#include <Nova/ImGui/ImGuiLayer.h>
#include <Nova/Window.h>
#include <memory>

namespace Nova {

	class NOVA_API Application {
	public:
		Application(const WindowProperties &props = WindowProperties());
		virtual ~Application() = default;

		void Run();
		void OnEvent(Event &e);
		void PushOverlay(Layer* overlay);
		void PushLayer(Layer* layer);
		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_instance; }
	private:
		bool OnWindowCloseEvent(WindowCLoseEvent &e);
		bool OnWindowResizeEvent(WindowResizeEvent &e);
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimize = false;
		LayerStack m_LayerStack;
		float m_lastFrameTime;
		static Application* s_instance;
	};

	//Client Implemented
	Application* CreateApplication();
}

#endif