#pragma once
#include "Component.h"
#include "Editor/ComponentEditor.h"

namespace golem
{
	class TagComponentEditor : public ComponentEditor
	{
	public:
		TagComponentEditor(const char* name) : ComponentEditor(name){}

		virtual void Draw(class GameObject gObj) override;
	};

	struct TagComponent : public Component
	{
		// Members ********************************************************************************
		std::string tag{};

		// Methods ********************************************************************************
		TagComponent() = default;
		TagComponent(const std::string& t)
			: tag{ t } {}
		TagComponent(const TagComponent& t) = default;

		operator std::string& () { return tag; }
		TagComponent& operator=(const std::string& t) { tag = t; }
		bool operator==(const std::string& t) { return tag == t; }
		bool operator==(const TagComponent& other) { return other.tag == tag; }

		COMPONENT_EDITOR(TagComponentEditor)
		REFLECT()
	};
}