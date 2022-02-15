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

	// ********************************************************************************************//
	//								 Viewport Resize Event Class								   //
	// ********************************************************************************************//
	class ViewportResizeEvent : public Event
	{
		// Members ********************************************************************************
	private:
		
		float m_width;
		float m_height;

		// Methods ********************************************************************************
	public:
		ViewportResizeEvent(float width, float height)
			: m_width{width}, m_height{height}
		{

		}

		inline float GetWidth() const {return m_width;}
		inline float GetHeight() const {return m_height;}

		EVENT_CLASS_TYPE(ViewportResized)
		EVENT_CLASS_CATEGORY(EventCategory_RenderSystem)
	};
}