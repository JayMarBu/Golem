#include "golpch.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/TagComponent.h"

namespace golem
{

	GameObject::GameObject(entt::entity id)
		:m_id(id)
	{

	}

	GameObject GameObject::Create(const std::string& name /*= std::string()*/)
	{
		return Application::Get().GetScene()->CreateGameObject(name);
	}

	golem::GameObject GameObject::Parent()
	{
		if(!HasComponent<Transform>())
			return;

		auto& t = GetComponent<Transform>();

		auto pt = t.Parent();
		if(pt)
		{
			return GameObject(pt->gObj);
		}

		return GameObject();
	}

	bool GameObject::ParentIsRoot()
	{
		if (!HasComponent<Transform>())
			return true;

		auto& t = GetComponent<Transform>();

		auto pt = t.Parent();
		if (pt)
		{
			auto pobj = GameObject(pt->gObj);

			if(pobj.HasComponent<TagComponent>())
			{
				return false;
			}
		}

		return true;
	}

	golem::GameObject GameObject::Child(int index)
	{
		return{};
	}

}