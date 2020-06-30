#include "GAL/Vulkan/VulkanFramebuffer.h"

#include "GAL/Vulkan/VulkanImage.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanRenderPass.h"

GAL::VulkanFramebuffer::VulkanFramebuffer(const CreateInfo& createInfo)
{
	GTSL::Ranger<const VulkanImage> vulkan_images = GTSL::Ranger<const VulkanImage>(createInfo.Images);
	GTSL::Array<VkImageView, 64> vk_image_views(vulkan_images.ElementCount());

	for (const auto& e : vulkan_images) { vk_image_views[&e - vulkan_images.begin()] = e.GetVkImageView(); }

	VkFramebufferCreateInfo vk_framebuffer_create_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	vk_framebuffer_create_info.attachmentCount = vulkan_images.ElementCount();
	vk_framebuffer_create_info.width = createInfo.Extent.Width;
	vk_framebuffer_create_info.height = createInfo.Extent.Height;
	vk_framebuffer_create_info.layers = 1;
	vk_framebuffer_create_info.renderPass = static_cast<VulkanRenderPass*>(createInfo.RenderPass)->GetVkRenderPass();
	vk_framebuffer_create_info.pAttachments = vk_image_views.begin();

	VK_CHECK(vkCreateFramebuffer(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_framebuffer_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &framebuffer));
}

void GAL::VulkanFramebuffer::Destroy(GAL::RenderDevice* renderDevice) const
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyFramebuffer(vk_render_device->GetVkDevice(), framebuffer, vk_render_device->GetVkAllocationCallbacks());
}
