#pragma once
#pragma once
#include "Golem/Core.h"

namespace golem
{
	class GOLEM_API Input
	{
		// Members ********************************************************************************
	private:
		static Input* s_instance;
	public:
		// Temp
		struct MousePos
		{
			float x;
			float y;

			MousePos(float _x, float _y)
				: x{ _x }, y{ _y } {}
		};
		// Methods ********************************************************************************
	public:
		inline static bool IsKeyPressed(int keycode) { return s_instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_instance->IsMouseButtonPressedImpl(button); }

		inline static float GetMouseX() { return s_instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_instance->GetMouseYImpl(); }
		inline static MousePos GetMousePos() { return s_instance->GetMousePosImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual MousePos GetMousePosImpl() = 0;
	};
}