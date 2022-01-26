#pragma once

#include "Golem/Events/Event.h"
#include "Golem/Render/RenderSystem/RenderSystemBase.h"

namespace golem
{
	// ********************************************************************************************//
	//								 Shader Re-Compile Event Class								   //
	// ********************************************************************************************//
	class ShaderReCompileEvent : public Event
	{
		// Members ********************************************************************************
	private:
		RenderSystemBase* m_renderSystem;

		// Methods ********************************************************************************
	public:
		ShaderReCompileEvent(RenderSystemBase* renderSystem)
			: m_renderSystem(renderSystem) {}

		inline RenderSystemBase* GetRenderSystem() const {return m_renderSystem;} 

		EVENT_CLASS_TYPE(ShaderRecompiled)
		EVENT_CLASS_CATEGORY(EventCategory_RenderSystem)
	};
}