#pragma once

#include "GAL/Buffer.h"

#include <GAL/ext/vulkan/vulkan.h>

namespace GAL
{
	class VulkanBuffer final : public Buffer
	{
	public:
		VulkanBuffer() = default;
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
