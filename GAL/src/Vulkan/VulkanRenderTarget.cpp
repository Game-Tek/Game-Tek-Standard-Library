#include "GAL/Vulkan/VulkanRenderTarget.h"

#include "GAL/Vulkan/VulkanMemory.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanRenderTarget::VulkanRenderTarget(const CreateInfo& createInfo)
{
	const auto image_format = ImageFormatToVkFormat(createInfo.Format);
	const auto image_extent = Extent3DToVkExtent3D(createInfo.Extent);

	VkImageCreateInfo vk_image_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vk_image_create_info.format = image_format;
	vk_image_create_info.arrayLayers = 1;
	vk_image_create_info.extent = image_extent;
	vk_image_create_info.imageType = ImageDimensionsToVkImageType(createInfo.Dimensions);
	vk_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vk_image_create_info.usage = ImageUseToVkImageUsageFlagBits(createInfo.Use);
	vk_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	vk_image_create_info.mipLevels = 1;

	VK_CHECK(vkCreateImage(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_image_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &image));

	VkImageViewCreateInfo vk_image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vk_image_view_create_info.format = image_format;
	vk_image_view_create_info.image = image;
	vk_image_view_create_info.viewType = ImageDimensionsToVkImageViewType(createInfo.Dimensions);
	vk_image_view_create_info.subresourceRange.aspectMask = ImageTypeToVkImageAspectFlagBits(createInfo.Type);
	vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
	vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
	vk_image_view_create_info.subresourceRange.layerCount = 1;
	vk_image_view_create_info.subresourceRange.levelCount = 1;

	VK_CHECK(vkCreateImageView(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_image_view_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &imageView));
}

void GAL::VulkanRenderTarget::Destroy(RenderDevice* renderDevice) const
{
	auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyImageView(vk_render_device->GetVkDevice(), imageView, vk_render_device->GetVkAllocationCallbacks());
	vkDestroyImage(vk_render_device->GetVkDevice(), image, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanRenderTarget::BindToMemory(const BindMemoryInfo& bindMemoryInfo) const
{
	vkBindImageMemory(static_cast<VulkanRenderDevice*>(bindMemoryInfo.RenderDevice)->GetVkDevice(), image, static_cast<VulkanDeviceMemory*>(bindMemoryInfo.Memory)->GetVkDeviceMemory(), bindMemoryInfo.Offset);
}
