#pragma once

#include "GAL/Memory.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanDeviceMemory final : public DeviceMemory
	{
	public:
		using VkDeviceMemory = void*;
		
		VulkanDeviceMemory() = default;

		struct CreateInfo final : VulkanRenderInfo
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 MemoryType{ 0 };
		};
		VulkanDeviceMemory(const CreateInfo& createInfo);
		~VulkanDeviceMemory() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VkDeviceMemory GetVkDeviceMemory() const { return deviceMemory; }

		struct MapInfo : VulkanRenderInfo
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 Offset{ 0 };
		};
		[[nodiscard]] void* Map(const MapInfo& mapInfo) const;

		struct UnmapInfo : RenderInfo
		{
		};
		void Unmap(const UnmapInfo& unmapInfo) const;
		
	private:
		VkDeviceMemory deviceMemory{ 0 };
	};
}
