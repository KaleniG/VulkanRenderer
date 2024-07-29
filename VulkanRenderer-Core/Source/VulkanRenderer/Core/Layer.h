#pragma once

#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Events/Event.h"
#include "VulkanRenderer/Core/Base.h"

namespace vkren 
{

	class Layer
	{
	public:
		Layer(const std::string& name = "Untitled") : m_DebugName(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep timestep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}

		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}