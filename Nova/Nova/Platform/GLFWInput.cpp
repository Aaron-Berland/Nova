#include "GLFWInput.h"
#include <Nova/Application.h>
#include <GLFW/glfw3.h>


namespace Nova
{
	


	bool GLFWInput::IsKeyPressedImpl(int keycode) const
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool GLFWInput::IsMouseButtonPressedImpl(int button) const
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	void GLFWInput::GetMousePositionImpl(float &x, float &y) const
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		x = (float) xpos;
		y = (float) ypos;
		
	}



}