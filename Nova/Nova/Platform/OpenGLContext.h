#pragma once 
#ifndef OPENGLCONTEXT_H
#define OPENGLCONTEXT_H
#include <Nova/Renderer/GraphicsContext.h>

struct GLFWwindow;

namespace Nova
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;


	private:
		GLFWwindow* m_windowHandle;

	};
}

#endif
