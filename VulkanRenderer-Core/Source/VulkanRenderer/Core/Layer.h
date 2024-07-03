#pragma once

#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Core/Base.h"

namespace vkren 
{

	class Layer
	{
	public:
		Layer(const std::string& name) : m_DebugName(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep timestep) {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}