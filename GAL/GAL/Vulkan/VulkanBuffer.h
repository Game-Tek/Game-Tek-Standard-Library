#pragma once

#include "GAL/Buffer.h"

#include "Vulkan.h"
#include "VulkanMemory.h"

namespace GAL
{
	class VulkanRenderDevice;
	
	class VulkanBuffer final : public Buffer
	{
	public:
		VulkanBuffer() = default;
		
		void GetMemoryRequirements(const VulkanRenderDevice* renderDevice, GTSL::uint32 size, VulkanBufferType::value_type bufferType, MemoryRequirements* memoryRequirements);
		
		void Initialize(const VulkanRenderDevice* renderDevice, const MemoryRequirements& memoryRequirements, VulkanDeviceMemory memory, GTSL::uint32 offset);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		
		[[nodiscard]] VkBuffer GetVkBuffer() const { return buffer; }
		
	private:
		VkBuffer buffer = nullptr;
	};
}
