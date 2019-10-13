#pragma once
#ifndef WINDOW_H
#define WINDOW_H
#include <string>
#include <functional>
#include <Nova/Core.h>
#include <Nova/Events/Event.h>
#include <Nova/Renderer/GraphicsContext.h>
namespace Nova
{
	struct WindowProperties
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		WindowProperties(const std::string &title = "Nova Engine",unsigned int width = 1280, unsigned int height = 720)
					: Title(title), Width(width), Height(height) {}

	};

	class NOVA_API Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() = default;
		
		virtual void OnUpdate() = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFunction &callback) = 0;
		virtual void SetVsync(bool enables) = 0;
		virtual bool isVsync() const = 0;
		virtual void* GetNativeWindow() const = 0;
		static Scope<Window> Create(const WindowProperties& props = WindowProperties());
	protected:
		Scope<GraphicsContext> m_context;
	};




}




#endif 