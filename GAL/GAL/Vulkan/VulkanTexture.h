#pragma once

#include "Vulkan.h"
#include "VulkanMemory.h"

namespace GAL
{
	class VulkanTexture final
	{
	public:
		//VulkanTexture() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			VulkanTextureLayout InitialLayout;
			VulkanTextureUses Uses;
			VulkanTextureFormat Format;
			GTSL::Extent3D Extent{ 1280, 720, 1 };
			VulkanTextureTiling Tiling;
			VulkanDimensions Dimensions;
			GTSL::uint8 MipLevels = 1;
			GTSL::uint32 Offset = 0;
			VulkanDeviceMemory Memory;
		};
		//explicit VulkanTexture(const CreateInfo& createInfo);

		struct GetMemoryRequirementsInfo final : VulkanRenderInfo
		{
			const CreateInfo* CreateInfo;
			MemoryRequirements MemoryRequirements;
		};
		void GetMemoryRequirements(GetMemoryRequirementsInfo* info);
		
		void Initialize(const CreateInfo& createInfo);
		
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