#pragma once

#include "Vulkan.h"
#include "VulkanMemory.h"

namespace GAL
{
	class VulkanRenderDevice;
	
	class VulkanTexture final
	{
	public:
		//VulkanTexture() = default;

		void GetMemoryRequirements(const VulkanRenderDevice* renderDevice, MemoryRequirements* memoryRequirements, VulkanTextureLayout initialLayout, VulkanTextureUses uses,
			VulkanTextureFormat format, GTSL::Extent3D extent, VulkanTextureTiling tiling, GTSL::uint8 mipLevels);
		
		void Initialize(const VulkanRenderDevice* renderDevice, VulkanDeviceMemory deviceMemory, GTSL::uint32 offset);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VkImage GetVkImage() const { return image; }
		
	private:
		VkImage image;// = nullptr;

		friend class VulkanRenderContext;
	};

	class VulkanTextureView final
	{
	public:
		VulkanTextureView() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			VulkanTexture Texture;
			VulkanTextureFormat Format;
			VulkanDimensions Dimensions;
			GTSL::uint8 MipLevels = 1;
			VulkanTextureType::value_type Type;
		};
		explicit VulkanTextureView(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] VkImageView GetVkImageView() const { return imageView; }
		
	private:
		VkImageView imageView = nullptr;

		friend class VulkanRenderContext;
	};

	class VulkanSampler final
	{
	public:
		VulkanSampler() = default;
		
		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::uint8 Anisotropy = 0;
		};
		explicit VulkanSampler(const CreateInfo& createInfo);
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkSampler GetVkSampler() const { return sampler; }
	private:
		VkSampler sampler{ nullptr };
	};
}