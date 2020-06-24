#include "GAL/Vulkan/VulkanTexture.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GAL/Vulkan/VulkanCommandBuffer.h"

#include "GAL/Vulkan/VulkanMemory.h"

GAL::VulkanTexture::VulkanTexture(const CreateInfo& createInfo) : Texture(createInfo), imageLayout(ImageLayoutToVkImageLayout(createInfo.Layout))
{
	VkImageCreateInfo vk_image_create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vk_image_create_info.imageType = VK_IMAGE_TYPE_2D;
	vk_image_create_info.extent.width = createInfo.Extent.Width;
	vk_image_create_info.extent.height = createInfo.Extent.Height;
	vk_image_create_info.extent.depth = 1;
	vk_image_create_info.mipLevels = 1;
	vk_image_create_info.arrayLayers = 1;
	vk_image_create_info.format = ImageFormatToVkFormat(createInfo.SourceFormat);
	vk_image_create_info.tiling = ImageTilingToVkImageTiling(createInfo.ImageTiling);
	vk_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vk_image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	vk_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	vk_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_CHECK(vkCreateImage(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_image_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &textureImage));
	
	{
		auto to_image_layout = ImageLayoutToVkImageLayout(createInfo.Layout);
	
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = to_image_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = textureImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
	
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;
	
		if (to_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			GAL_DEBUG_BREAK;
		}
	
		vkCmdPipelineBarrier(static_cast<VulkanCommandBuffer*>(createInfo.CommandBuffer)->GetVkCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}
	
	VkImageViewCreateInfo vk_image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vk_image_view_create_info.image = textureImage;
	vk_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	vk_image_view_create_info.format = ImageFormatToVkFormat(createInfo.SourceFormat);
	vk_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
	vk_image_view_create_info.subresourceRange.levelCount = 1;
	vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
	vk_image_view_create_info.subresourceRange.layerCount = 1;
	VK_CHECK(vkCreateImageView(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_image_view_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &textureImageView));
	
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
	
	VK_CHECK(vkCreateSampler(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_sampler_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &textureSampler));
}

void GAL::VulkanTexture::Destroy(GAL::RenderDevice* renderDevice)
{
	auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroySampler(vk_render_device->GetVkDevice(), textureSampler, vk_render_device->GetVkAllocationCallbacks());
	vkDestroyImageView(vk_render_device->GetVkDevice(), textureImageView, vk_render_device->GetVkAllocationCallbacks());
	vkDestroyImage(vk_render_device->GetVkDevice(), textureImage, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanTexture::BindToMemory(const BindMemoryInfo& bindMemoryInfo) const
{
	vkBindImageMemory(static_cast<VulkanRenderDevice*>(bindMemoryInfo.RenderDevice)->GetVkDevice(), textureImage, static_cast<VulkanDeviceMemory*>(bindMemoryInfo.Memory)->GetVkDeviceMemory(), bindMemoryInfo.Offset);
}
