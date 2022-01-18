#pragma once
#pragma once

#include "Golem/Input.h"

namespace golem
{
	class InputWin32 : public Input
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual MousePos GetMousePosImpl() override;
	};
}


