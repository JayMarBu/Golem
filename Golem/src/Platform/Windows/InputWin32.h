#pragma once

#include "Golem/Input/Input.h"

namespace golem
{
	class InputWin32 : public Input
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	protected:
		virtual bool IsKeyPressedImpl(KeyCode keycode) override;

		virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual MousePos GetMousePosImpl() override;
		virtual void SetMousePosImpl(float x, float y) override;
		virtual void SetMousePosCentreImpl() override;
		virtual void SetMouseCursorHiddenImpl(bool hidden) override;
	};
}


