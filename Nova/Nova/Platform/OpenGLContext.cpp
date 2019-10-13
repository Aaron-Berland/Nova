#include "OpenGLContext.h"

#include <Nova/Core.h>
#include <Nova/Log.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Nova
{

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle)
	{
		NOVA_CORE_ASSERT(windowHandle, "Window handle in null!!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NOVA_CORE_ASSERT(status, "Failed to initialize Glad!");
		NOVA_CORE_INFO("OpenGL Info:");
		NOVA_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		NOVA_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		NOVA_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}

}
