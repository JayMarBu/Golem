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
		
		// Methods ********************************************************************************
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const {return m_debugName;}
	};
}


