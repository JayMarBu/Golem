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

	struct TagComponent : public Component
	{
		// Members ********************************************************************************
		std::string tag{};

		// Methods ********************************************************************************
		TagComponent() = default;
		TagComponent(const std::string & t)
			: tag{ t } {}
		TagComponent(const TagComponent & t) = default;

		operator std::string& () { return tag; }
		TagComponent& operator=(const std::string & t) { tag = t; }
		bool operator==(const std::string & t) { return tag == t; }
		bool operator==(const TagComponent & other) { return other.tag == tag; }
	};
}