#include "GAL/Vulkan/VulkanImage.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanMemory.h"

GAL::VulkanImage::VulkanImage(const CreateInfo& createInfo)
{
	VkImageCreateInfo vk_image_create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vk_image_create_info.imageType = ImageDimensionsToVkImageType(createInfo.Dimensions);
	vk_image_create_info.extent.width = createInfo.Extent.Width;
	vk_image_create_info.extent.height = createInfo.Extent.Height;
	vk_image_create_info.extent.depth = 1;
	vk_image_create_info.mipLevels = createInfo.MipLevels;
	vk_image_create_info.arrayLayers = 1;
	vk_image_create_info.format = static_cast<VkFormat>(createInfo.SourceFormat);
	vk_image_create_info.tiling = static_cast<VkImageTiling>(createInfo.ImageTiling);
	vk_image_create_info.initialLayout = ImageLayoutToVkImageLayout(createInfo.InitialLayout);
	vk_image_create_info.usage = createInfo.ImageUses;
	vk_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	vk_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	VK_CHECK(vkCreateImage(createInfo.RenderDevice->GetVkDevice(), &vk_image_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &image));
	SET_NAME(image, VK_OBJECT_TYPE_IMAGE createInfo);
}

void GAL::VulkanImage::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyImage(renderDevice->GetVkDevice(), image, renderDevice->GetVkAllocationCallbacks());
}

void GAL::VulkanImage::BindToMemory(const BindMemoryInfo& bindMemoryInfo) const
{
	vkBindImageMemory(static_cast<const VulkanRenderDevice*>(bindMemoryInfo.RenderDevice)->GetVkDevice(), image,
	static_cast<VkDeviceMemory>(static_cast<VulkanDeviceMemory*>(bindMemoryInfo.Memory)->GetVkDeviceMemory()), bindMemoryInfo.Offset);
}

GAL::VulkanImageView::VulkanImageView(const CreateInfo& createInfo)
{
	VkImageViewCreateInfo vk_image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vk_image_view_create_info.image = static_cast<const VulkanImage*>(createInfo.Image)->GetVkImage();
	vk_image_view_create_info.viewType = ImageDimensionsToVkImageViewType(createInfo.Dimensions);
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

void GAL::VulkanImageView::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyImageView(renderDevice->GetVkDevice(), imageView, renderDevice->GetVkAllocationCallbacks());
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
}
