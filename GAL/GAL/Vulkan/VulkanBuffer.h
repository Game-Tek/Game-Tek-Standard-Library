#pragma once

#include "GAL/Buffer.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanBuffer final : public Buffer
	{
	public:
		VulkanBuffer() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 BufferType{ 0 };
		};
		VulkanBuffer(const CreateInfo& createInfo);

		void Initialize(const CreateInfo& info);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		struct BindMemoryInfo : VulkanRenderInfo
		{
			class VulkanDeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;

		[[nodiscard]] VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		
		[[nodiscard]] VkBuffer GetVkBuffer() const { return buffer; }
		
	private:
		VkBuffer buffer{ 0 };
	};
}
