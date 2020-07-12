#pragma once

#include "GAL/Image.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanImage final : public Image
	{
	public:
		VulkanImage() = default;
		explicit VulkanImage(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		struct BindMemoryInfo : RenderInfo
		{
			class DeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;
		
		[[nodiscard]] VkImage GetVkImage() const { return image; }
		
	private:
		VkImage image = nullptr;

		friend class VulkanRenderContext;
	};

	class VulkanImageView final : public ImageView
	{
	public:
		VulkanImageView() = default;
		explicit VulkanImageView(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VkImageView GetVkImageView() const { return imageView; }
		
	private:
		VkImageView imageView = nullptr;

		friend class VulkanRenderContext;
	};

	class VulkanSampler final : public Sampler
	{
	public:
		explicit VulkanSampler(const CreateInfo& createInfo);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkSampler GetVkSampler() const { return sampler; }
	private:
		VkSampler sampler{ nullptr };
	};
}