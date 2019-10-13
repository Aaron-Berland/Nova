#include "Window.h"
#include <Nova/Renderer/Renderer.h>
/*
	TODO(BYRON): This may change when the engine starts supporting multiple platforms
*/
#include <Nova/Platform/GLFWWindow.h>
namespace Nova
{
	Scope<Window> Window::Create(const WindowProperties& props)
	{
		return CreateScope<GLFWWindow>(props);
	}
}
