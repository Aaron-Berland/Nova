#include "GLFWWindow.h"
#include <Nova/Log.h>
#include <Nova/Events/ApplicationEvent.h>
#include <Nova/Events/MouseEvent.h>
#include <Nova/Events/KeyEvent.h>
/*
	TODO(BYRON): This should change once the engine supports multiple platforms
*/
#include <Nova/Platform/OpenGLContext.h>
#include <GLFW/glfw3.h>
namespace Nova
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		NOVA_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}
	GLFWWindow::GLFWWindow(const WindowProperties &props)
	{
		Init(props);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}
	void GLFWWindow::Init(const WindowProperties &props) {
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		NOVA_CORE_INFO("Creating GLFW Window {0} with dimensions ({1},{2})", props.Title, props.Width, props.Height);
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			NOVA_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;

		}

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		/*
			TODO(BYRON): Thismmay change once rela multiplatform support is added
		*/
		m_context = CreateScope<OpenGLContext>(m_Window);
		m_context->Init();
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVsync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			WindowResizeEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCLoseEvent e;
			data.EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallback(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key,1);
					data.EventCallback(e);
					break;
				}

			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent e(keycode);
			data.EventCallback(e);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xOffset, (float)yOffset);
			data.EventCallback(e);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)xPos, (float)yPos);
			data.EventCallback(e);
		});
	}

	void GLFWWindow::Shutdown() {
		glfwDestroyWindow(m_Window);
	}

	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();
		m_context->SwapBuffers();
	}

	void GLFWWindow::SetVsync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.Vsync = enabled;
	}


	bool GLFWWindow::isVsync() const
	{
		return m_Data.Vsync;
	}

}