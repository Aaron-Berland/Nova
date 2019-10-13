#pragma once
#ifndef GLFWINPUT_H
#define GLFWINPUT_H
#include <Nova/Input.h>

namespace Nova
{
	class GLFWInput : public Input 
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) const override;
		virtual bool IsMouseButtonPressedImpl(int button) const override;
		virtual void GetMousePositionImpl(float &x, float &y) const override;


	};
}
#endif