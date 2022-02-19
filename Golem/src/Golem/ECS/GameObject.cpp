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
}