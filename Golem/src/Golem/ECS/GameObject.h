#pragma once
#include "Golem/ECS/Scene.h"
#include "Golem/Application.h"

namespace golem
{
	class GameObject
	{
		// Members ********************************************************************************
	private:
		entt::entity m_id{ entt::null };

		// Methods ********************************************************************************
	public:
		GameObject(entt::entity id);
		GameObject() {}

		operator bool() const { return m_id != entt::null; }
		operator uint32_t() const { return (uint32_t)m_id; }
		operator entt::entity() const { return m_id; }
		bool operator ==(const GameObject& other) { return other.m_id == m_id; }
		bool operator !=(const GameObject& other) { return !(*this == other); }

		static GameObject Create(const std::string& name = std::string());

		template <typename T>
		bool HasComponent();

		template <typename T, typename... Args>
		T& AddComponent(Args... args);

		template <typename T>
		T& GetComponent();

		template <typename T>
		void RemoveComponent();

		GameObject Parent();

		bool ParentIsRoot();

		GameObject Child(int index);
	};

	template <typename T>
	void GameObject::RemoveComponent()
	{
		GOL_ASSERT(HasComponent<T>(), "entity does not have component of this type");

		Application::Get().GetScene()->m_registry.remove<T>(m_id);
	}

	template <typename T>
	T& GameObject::GetComponent()
	{
		GOL_ASSERT(HasComponent<T>(), "entity does not have component of this type");

		return Application::Get().GetScene()->m_registry.get<T>(m_id);
	}

	template <typename T, typename... Args>
	T& GameObject::AddComponent(Args... args)
	{
		GOL_ASSERT(!HasComponent<T>(), "entity already has component of this type");

		T& comp = Application::Get().GetScene()->m_registry.emplace<T>(m_id, args...);

		return comp;//Application::Get().GetScene()->m_registry.emplace<T>(m_id, args...);
	}

	template <typename T>
	bool GameObject::HasComponent()
	{
		return Application::Get().GetScene()->m_registry.all_of<T>(m_id);
	}
}