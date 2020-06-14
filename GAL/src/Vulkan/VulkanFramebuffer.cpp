#include "GAL/Vulkan/VulkanFramebuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanRenderTarget.h"
#include "GAL/Vulkan/VulkanRenderPass.h"

GAL::VulkanFramebuffer::VulkanFramebuffer(const CreateInfo& createInfo) : Framebuffer(createInfo)
{
	GTSL::Array<VkImageView, 64> result;

	for (GTSL::uint8 i = 0; i < result.GetCapacity(); ++i)
	{
		result.PushBack(static_cast<VulkanRenderTarget*>(createInfo.Images[i])->GetVkImageView());
	}

//for (GTSL::uint8 i = 0; i < createInfo.ClearValues.GetLength(); ++i)
//{
//	clearValues.EmplaceBack(VkClearValue{ { { createInfo.ClearValues[i].R, createInfo.ClearValues[i].G, createInfo.ClearValues[i].B, createInfo.ClearValues[i].A } } });
//}

	attachmentCount = createInfo.Images.GetLength();

	VkFramebufferCreateInfo vk_framebuffer_create_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	vk_framebuffer_create_info.attachmentCount = createInfo.Images.GetLength();
	vk_framebuffer_create_info.width = createInfo.Extent.Width;
	vk_framebuffer_create_info.height = createInfo.Extent.Height;
	vk_framebuffer_create_info.layers = 1;
	vk_framebuffer_create_info.renderPass = static_cast<VulkanRenderPass*>(createInfo.RenderPass)->GetVkRenderPass();
	vk_framebuffer_create_info.pAttachments = result.GetData();

	VK_CHECK(vkCreateFramebuffer(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_framebuffer_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &framebuffer));
}

void GAL::VulkanFramebuffer::Destroy(GAL::RenderDevice* renderDevice) const
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyFramebuffer(vk_render_device->GetVkDevice(), framebuffer, vk_render_device->GetVkAllocationCallbacks());
}
