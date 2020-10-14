#pragma once

#include "GAL/Buffer.h"

#include "Vulkan.h"
#include "VulkanMemory.h"

namespace GAL
{
	class VulkanBuffer final : public Buffer
	{
	public:
		VulkanBuffer() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			GTSL::uint32 Size = 0, Offset = 0;
			VulkanBufferType::value_type BufferType = 0;
			VulkanDeviceMemory Memory;
		};
		//VulkanBuffer(const CreateInfo& createInfo);

		struct GetMemoryRequirementsInfo final : VulkanRenderInfo
		{
			CreateInfo CreateInfo;
			MemoryRequirements* MemoryRequirements;
		};
		void GetMemoryRequirements(const GetMemoryRequirementsInfo& info);
		
		void Initialize(const CreateInfo& info);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		
		[[nodiscard]] VkBuffer GetVkBuffer() const { return buffer; }
		
	private:
		VkBuffer buffer = nullptr;
	};
}
