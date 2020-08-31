#include "GAL/Vulkan/VulkanTexture.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanMemory.h"

GAL::VulkanTexture::VulkanTexture(const CreateInfo& createInfo)
{
	VkImageCreateInfo vkImageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vkImageCreateInfo.imageType = static_cast<VkImageType>(createInfo.Dimensions);
	vkImageCreateInfo.extent = Extent3DToVkExtent3D(createInfo.Extent);
	vkImageCreateInfo.mipLevels = createInfo.MipLevels;
	vkImageCreateInfo.arrayLayers = 1;
	vkImageCreateInfo.format = static_cast<VkFormat>(createInfo.Format);
	vkImageCreateInfo.tiling = static_cast<VkImageTiling>(createInfo.Tiling);
	vkImageCreateInfo.initialLayout = static_cast<VkImageLayout>(createInfo.InitialLayout);
	vkImageCreateInfo.usage = createInfo.Uses;
	vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	VK_CHECK(vkCreateImage(createInfo.RenderDevice->GetVkDevice(), &vkImageCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &image));
	SET_NAME(image, VK_OBJECT_TYPE_IMAGE, createInfo);
}

void GAL::VulkanTexture::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyImage(renderDevice->GetVkDevice(), image, renderDevice->GetVkAllocationCallbacks());
	debugClear(image);
}

void GAL::VulkanTexture::BindToMemory(const BindMemoryInfo& bindMemoryInfo) const
{
	VK_CHECK(vkBindImageMemory(bindMemoryInfo.RenderDevice->GetVkDevice(), image, static_cast<VkDeviceMemory>(bindMemoryInfo.Memory->GetVkDeviceMemory()), bindMemoryInfo.Offset));
}

GAL::VulkanTextureView::VulkanTextureView(const CreateInfo& createInfo)
{
	VkImageViewCreateInfo vkImageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vkImageViewCreateInfo.image = createInfo.Texture.GetVkImage();
	vkImageViewCreateInfo.viewType = static_cast<VkImageViewType>(createInfo.Dimensions);
	vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.format = static_cast<VkFormat>(createInfo.Format);
	vkImageViewCreateInfo.subresourceRange.aspectMask = createInfo.Type;
	vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	vkImageViewCreateInfo.subresourceRange.levelCount = createInfo.MipLevels;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount = 1;
	
	VK_CHECK(vkCreateImageView(createInfo.RenderDevice->GetVkDevice(), &vkImageViewCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &imageView));
	SET_NAME(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, createInfo);
}

void GAL::VulkanTextureView::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyImageView(renderDevice->GetVkDevice(), imageView, renderDevice->GetVkAllocationCallbacks());
	debugClear(imageView);
}

GAL::VulkanSampler::VulkanSampler(const CreateInfo& createInfo)
{
	VkSamplerCreateInfo vk_sampler_create_info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	vk_sampler_create_info.magFilter = VK_FILTER_LINEAR;
	vk_sampler_create_info.minFilter = VK_FILTER_LINEAR;
	vk_sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vk_sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vk_sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	vk_sampler_create_info.anisotropyEnable = static_cast<VkBool32>(createInfo.Anisotropy);
	vk_sampler_create_info.maxAnisotropy = static_cast<float>(createInfo.Anisotropy == 0 ? 1 : createInfo.Anisotropy);

	vk_sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	vk_sampler_create_info.unnormalizedCoordinates = VK_FALSE;
	vk_sampler_create_info.compareEnable = VK_FALSE;
	vk_sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
	vk_sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	vk_sampler_create_info.mipLodBias = 0.0f;
	vk_sampler_create_info.minLod = 0.0f;
	vk_sampler_create_info.maxLod = 0.0f;

	VK_CHECK(vkCreateSampler(createInfo.RenderDevice->GetVkDevice(), &vk_sampler_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &sampler));
	SET_NAME(sampler, VK_OBJECT_TYPE_SAMPLER, createInfo);
}

void GAL::VulkanSampler::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroySampler(renderDevice->GetVkDevice(), sampler, renderDevice->GetVkAllocationCallbacks());
	debugClear(sampler);
}
