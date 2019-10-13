#pragma once
#ifndef INPUT_H
#define INPUT_H
#include <Nova/Core.h>
#include <memory>
namespace Nova
{
	class NOVA_API Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_instance->IsMouseButtonPressedImpl(button); }
		inline static void GetMousePosition(float& x, float &y){ s_instance->GetMousePositionImpl(x, y); }
		Input(const Input &input) = delete;
		Input& operator=(const Input &input) = delete;
	protected:
		Input() = default;
		virtual bool IsKeyPressedImpl(int keycode) const = 0;
		virtual bool IsMouseButtonPressedImpl(int button) const = 0;
		virtual void GetMousePositionImpl(float &x, float &y) const = 0;
	private:
		static Scope<Input> s_instance;
	};
}
#endif