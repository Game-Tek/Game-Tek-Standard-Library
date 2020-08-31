#include "GAL/Vulkan/VulkanFramebuffer.h"

#include "GAL/Vulkan/VulkanTexture.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanRenderPass.h"

GAL::VulkanFramebuffer::VulkanFramebuffer(const CreateInfo& createInfo)
{
	VkFramebufferCreateInfo vk_framebuffer_create_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	vk_framebuffer_create_info.width = createInfo.Extent.Width;
	vk_framebuffer_create_info.height = createInfo.Extent.Height;
	vk_framebuffer_create_info.layers = 1;
	vk_framebuffer_create_info.renderPass = createInfo.RenderPass->GetVkRenderPass();
	vk_framebuffer_create_info.attachmentCount = createInfo.TextureViews.ElementCount();
	vk_framebuffer_create_info.pAttachments = reinterpret_cast<const VkImageView*>(createInfo.TextureViews.begin());

	VK_CHECK(vkCreateFramebuffer(createInfo.RenderDevice->GetVkDevice(), &vk_framebuffer_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &framebuffer));
	SET_NAME(framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, createInfo);
}

void GAL::VulkanFramebuffer::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyFramebuffer(renderDevice->GetVkDevice(), framebuffer, renderDevice->GetVkAllocationCallbacks());
	debugClear(framebuffer);
}
