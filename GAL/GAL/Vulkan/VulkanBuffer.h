#pragma once

#include <vulkan.h>

#include "GAL/Buffer.h"

namespace GAL
{
	class VulkanBuffer : public Buffer
	{
	public:
		VulkanBuffer(const CreateInfo& createInfo);

		struct BindMemoryInfo : RenderInfo
		{
			class DeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;
		
		[[nodiscard]] VkBuffer GetVkBuffer() const { return buffer; }
	private:
		VkBuffer buffer{ 0 };
	};
}
