#pragma once

#include "GAL/Memory.h"

namespace GAL
{
	class VulkanDeviceMemory final : public DeviceMemory
	{
	public:
		using VkDeviceMemory = void*;
		
		VulkanDeviceMemory() = default;
		~VulkanDeviceMemory() = default;
		VulkanDeviceMemory(const CreateInfo& createInfo);

		void Destroy(class RenderDevice* renderDevice);
		
		[[nodiscard]] VkDeviceMemory GetVkDeviceMemory() const { return deviceMemory; }

		struct MapInfo : RenderInfo
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
