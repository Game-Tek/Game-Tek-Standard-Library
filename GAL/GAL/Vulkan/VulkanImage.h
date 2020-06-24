#pragma once

#include "GAL/Image.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage() = default;
		explicit VulkanImage(const CreateInfo& createInfo);

		void Destroy(class RenderDevice* renderDevice);

		struct BindMemoryInfo : RenderInfo
		{
			class DeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;
		
		[[nodiscard]] VkImage GetVkImage() const { return image; }
		[[nodiscard]] VkImageView GetVkImageView() const { return imageView; }
		
	private:
		VkImage image = nullptr;
		VkImageView imageView = nullptr;

		friend class VulkanRenderContext;
	};

	class VulkanSampler final : public Sampler
	{
	public:
		explicit VulkanSampler(const CreateInfo& createInfo);
		
		void Destroy(class RenderDevice* renderDevice);

		[[nodiscard]] VkSampler GetVkSampler() const { return sampler; }
	private:
		VkSampler sampler{ nullptr };
	};
}