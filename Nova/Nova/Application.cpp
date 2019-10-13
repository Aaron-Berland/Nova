#include "Application.h"
#include <Nova/Log.h>
/*
	TODO(BYRON): ALso temporart
*/
#include <Nova/Input.h>

/*
	TODO(BYRON): Temporary
*/
#include <Nova/Renderer/Shader.h>
#include <Nova/Renderer/VertexArray.h>
#include <Nova/Renderer/Renderer.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace Nova {
#define BIND_EVENT_FN(x) std::bind(&Application::x,this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;
	Application::Application(const WindowProperties &props) : m_lastFrameTime(0.0f)
	{
		NOVA_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;
		m_Window = Window::Create(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		Renderer::Init();
		m_ImGuiLayer = CreateImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}


	void Application::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCLoseEvent>(BIND_EVENT_FN(OnWindowCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResizeEvent));
	

		for (auto it = m_LayerStack.end(); it!= m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.handled)
				break;
		}
		
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	bool Application::OnWindowCloseEvent(WindowCLoseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResizeEvent(Nova::WindowResizeEvent &e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimize = true;
			return false;
		}
		m_Minimize = false;
		Nova::RenderCommand::ViewportResize(0, 0, e.GetWidth(), e.GetHeight());
		return false;
	}
	void Application::Run() {


		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep ts =  time - m_lastFrameTime;
			m_lastFrameTime = time;
			
			if(!m_Minimize)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(ts);
			}
			
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			m_Window->OnUpdate();
		}
	}

	
}
