#pragma once
#include "entt.hpp"

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
	};
}