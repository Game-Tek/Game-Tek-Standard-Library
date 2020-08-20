#pragma once

#include "GAL/Image.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanTexture final : public Image
	{
	public:
		VulkanTexture() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			VulkanTextureLayout InitialLayout;
			GTSL::uint32 ImageUses{ 0 };
			GTSL::uint32 SourceFormat{ 0 };
			GTSL::Extent3D Extent{ 1280, 720, 1 };
			GTSL::uint32 ImageTiling{ 0 };
			VulkanDimensions Dimensions;
			GTSL::uint8 MipLevels{ 1 };
		};
		explicit VulkanTexture(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		struct BindMemoryInfo : VulkanRenderInfo
		{
			class VulkanDeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;
		
		[[nodiscard]] VkImage GetVkImage() const { return image; }
		
	private:
		VkImage image = nullptr;

		friend class VulkanRenderContext;
	};

	class VulkanTextureView final : public ImageView
	{
	public:
		VulkanTextureView() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			const VulkanTexture* Image{ nullptr };
			GTSL::uint32 SourceFormat{ 0 };
			GTSL::Extent2D Extent{ 1280, 720 };
			VulkanDimensions Dimensions;
			GTSL::uint8 MipLevels{ 1 };
			ImageType Type;
		};
		explicit VulkanTextureView(const CreateInfo& createInfo);

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