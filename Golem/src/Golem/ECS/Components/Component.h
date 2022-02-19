#pragma once
#include "entt.hpp"
#include "Golem/Reflection/Reflection.h"

namespace golem
{
	struct Component
	{
		// Members ********************************************************************************
	public:
		entt::entity gObj;

		// Methods ********************************************************************************
	protected:
		Component() = default;
	public:

		REFLECT()
	};
}