#pragma once
#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H
#include <Nova/Window.h>


struct GLFWwindow;
namespace Nova
{
	class GLFWWindow : public Window {
	public:
		GLFWWindow(const WindowProperties &props);
		virtual ~GLFWWindow();

		virtual void OnUpdate() override;

		virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
		virtual inline unsigned int GetHeight() const override { return m_Data.Height; }

		virtual inline void SetEventCallback(const EventCallbackFunction &callback) override { m_Data.EventCallback = callback; }
		virtual void SetVsync(bool enabled) override;
		virtual bool isVsync() const override;
		virtual inline void* GetNativeWindow() const override { return m_Window; }
		

	private:
		virtual void Init(const WindowProperties &props);
		virtual void Shutdown();

		GLFWwindow* m_Window;
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool Vsync;
			EventCallbackFunction EventCallback;
		};

		WindowData m_Data;
	};
}

#endif