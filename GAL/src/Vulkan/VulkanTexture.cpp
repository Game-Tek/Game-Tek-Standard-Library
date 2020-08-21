#include "GAL/Vulkan/VulkanTexture.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanMemory.h"

GAL::VulkanTexture::VulkanTexture(const CreateInfo& createInfo)
{
	VkImageCreateInfo vk_image_create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vk_image_create_info.imageType = static_cast<VkImageType>(createInfo.Dimensions);
	vk_image_create_info.extent = Extent3DToVkExtent3D(createInfo.Extent);
	vk_image_create_info.mipLevels = createInfo.MipLevels;
	vk_image_create_info.arrayLayers = 1;
	vk_image_create_info.format = static_cast<VkFormat>(createInfo.SourceFormat);
	vk_image_create_info.tiling = static_cast<VkImageTiling>(createInfo.Tiling);
	vk_image_create_info.initialLayout = static_cast<VkImageLayout>(createInfo.InitialLayout);
	vk_image_create_info.usage = createInfo.ImageUses;
	vk_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	vk_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	VK_CHECK(vkCreateImage(createInfo.RenderDevice->GetVkDevice(), &vk_image_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &image));
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
	VkImageViewCreateInfo vk_image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vk_image_view_create_info.image = createInfo.Image->GetVkImage();
	vk_image_view_create_info.viewType = static_cast<VkImageViewType>(createInfo.Dimensions);
	vk_image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_image_view_create_info.format = static_cast<VkFormat>(createInfo.SourceFormat);
	vk_image_view_create_info.subresourceRange.aspectMask = ImageTypeToVkImageAspectFlagBits(createInfo.Type);
	vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
	vk_image_view_create_info.subresourceRange.levelCount = createInfo.MipLevels;
	vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
	vk_image_view_create_info.subresourceRange.layerCount = 1;
	
	VK_CHECK(vkCreateImageView(createInfo.RenderDevice->GetVkDevice(), &vk_image_view_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &imageView));
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

	vk_sampler_create_info.anisotropyEnable = VkBool32(createInfo.Anisotropy);
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
