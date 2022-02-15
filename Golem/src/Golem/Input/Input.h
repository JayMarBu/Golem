#pragma once
#pragma once
#include "Golem/Core.h"
#include "Golem/Input/KeyCodes.h"
#include "Golem/Input/MouseButtonCodes.h"

namespace golem
{
	class Input
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
		inline static bool IsKeyPressed(KeyCode keycode) { return s_instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(MouseCode button) { return s_instance->IsMouseButtonPressedImpl(button); }

		inline static float GetMouseX() { return s_instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_instance->GetMouseYImpl(); }
		inline static MousePos GetMousePos() { return s_instance->GetMousePosImpl(); }
		inline static void SetMousePos(float x, float y) { s_instance->SetMousePosImpl(x,y);}
		inline static void SetMousePosCentre() { s_instance->SetMousePosCentreImpl();}

		inline static void SetMouseCursorHidden(bool hidden) {s_instance->SetMouseCursorHiddenImpl(hidden);}

	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual MousePos GetMousePosImpl() = 0;
		virtual void SetMousePosImpl(float x, float y) = 0;
		virtual void SetMousePosCentreImpl() = 0;
		virtual void SetMouseCursorHiddenImpl(bool hidden) = 0;
	};
}