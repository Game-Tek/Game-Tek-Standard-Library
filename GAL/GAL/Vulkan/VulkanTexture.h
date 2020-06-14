#pragma once

#include "GAL/Texture.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanTexture : public GAL::Texture
	{
	public:
		explicit VulkanTexture(const CreateInfo& createInfo);

		void Destroy(class GAL::RenderDevice* renderDevice) override;

		struct BindMemoryInfo : RenderInfo
		{
			class DeviceMemory* Memory{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindToMemory(const BindMemoryInfo& bindMemoryInfo) const;

		[[nodiscard]] VkImage GetVkImage() const { return textureImage; }
		[[nodiscard]] VkImageView GetImageView() const { return textureImageView; }
		[[nodiscard]] VkSampler GetImageSampler() const { return textureSampler; }

	private:
		VkImage textureImage = nullptr;
		VkImageView textureImageView = nullptr;
		VkSampler textureSampler = nullptr;

	};
}