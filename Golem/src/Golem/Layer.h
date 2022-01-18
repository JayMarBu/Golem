#pragma once

#include "Golem/Core.h"
#include "Golem/Events/Event.h"

namespace golem
{
	class GOLEM_API Layer
	{
		// Members ********************************************************************************
	protected:
		std::string m_debugName;
		bool m_attached = false;
		
		// Methods ********************************************************************************
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}

		inline void SetAttached(bool attached) {m_attached = attached;}
		inline bool IsAttached() const {return m_attached;}

		inline const std::string& GetName() const {return m_debugName;}
	};
}


