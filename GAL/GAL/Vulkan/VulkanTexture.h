#pragma once

#include "GAL/Texture.h"

#include "Vulkan.h"

class VulkanTexture : public GAL::Texture
{
	VkImage textureImage = nullptr;
	VkDeviceMemory textureImageMemory = nullptr;
	VkImageView textureImageView = nullptr;
	VkSampler textureSampler = nullptr;

public:
	VulkanTexture(class VulkanRenderDevice* vulkanRenderDevice, const GAL::TextureCreateInfo& textureCreateInfo);

	void Destroy(class GAL::RenderDevice* renderDevice) override;

	[[nodiscard]] VkImageView GetImageView() const { return textureImageView; }
	[[nodiscard]] VkSampler GetImageSampler() const { return textureSampler; }
};
