#pragma once

#include "GAL/RenderTarget.h"

#include "GAL/Vulkan/Vulkan.h"

namespace GAL
{
	class VulkanRenderTarget final : public RenderTarget
	{
		VkImageView imageView = nullptr;
		VkImage image = nullptr;

		friend class VulkanRenderContext;
	public:
		VulkanRenderTarget(const CreateInfo& imageCreateInfo);
		void Destroy(GAL::RenderDevice* renderDevice) const;

		struct BindMemoryInfo : RenderInfo
		{
			class DeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;

		[[nodiscard]] VkImage GetVkImage() const { return image; }
		[[nodiscard]] VkImageView GetVkImageView() const { return imageView; }
	};
}