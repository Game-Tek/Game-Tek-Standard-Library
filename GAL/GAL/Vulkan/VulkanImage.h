#pragma once

#include "GAL/Image.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanImage final : public Image
	{
	public:
		VulkanImage() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			ImageLayout InitialLayout{ ImageLayout::COLOR_ATTACHMENT };
			GTSL::uint32 ImageUses{ 0 };
			GTSL::uint32 SourceFormat{ 0 };
			GTSL::Extent2D Extent{ 1280, 720 };
			GTSL::uint32 ImageTiling{ 0 };
			ImageDimensions Dimensions;
			GTSL::uint8 MipLevels{ 1 };
		};
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

		struct CreateInfo final : VulkanCreateInfo
		{
			ImageLayout InitialLayout{ ImageLayout::COLOR_ATTACHMENT };
			const Image* Image{ nullptr };
			GTSL::uint32 SourceFormat{ 0 };
			GTSL::Extent2D Extent{ 1280, 720 };
			ImageDimensions Dimensions;
			GTSL::uint8 MipLevels{ 1 };
			ImageType Type;
		};
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
		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::uint8 Anisotropy{ 0 };
		};
		explicit VulkanSampler(const CreateInfo& createInfo);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkSampler GetVkSampler() const { return sampler; }
	private:
		VkSampler sampler{ nullptr };
	};
}