#include "GAL/Vulkan/VulkanFramebuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanRenderTarget.h"
#include "GAL/Vulkan/VulkanRenderPass.h"

VulkanFramebuffer::VulkanFramebuffer(VulkanRenderDevice* vulkanRenderDevice, const GAL::FramebufferCreateInfo& framebufferCreateInfo) : Framebuffer(framebufferCreateInfo)
{
	GTSL::Vector<VkImageView> result(framebufferCreateInfo.Images.GetLength(), vulkanRenderDevice->GetTransientAllocationsAllocatorReference());

	for (GTSL::uint8 i = 0; i < result.GetCapacity(); ++i)
	{
		result.PushBack(static_cast<VulkanRenderTargetBase*>(framebufferCreateInfo.Images[i])->GetVkImageView());
	}

	for (GTSL::uint8 i = 0; i < framebufferCreateInfo.ClearValues.GetLength(); ++i)
	{
		clearValues.EmplaceBack(VkClearValue{ { { framebufferCreateInfo.ClearValues[i].R, framebufferCreateInfo.ClearValues[i].G, framebufferCreateInfo.ClearValues[i].B, framebufferCreateInfo.ClearValues[i].A } } });
	}

	attachmentCount = framebufferCreateInfo.Images.GetLength();

	VkFramebufferCreateInfo vk_framebuffer_create_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	vk_framebuffer_create_info.attachmentCount = framebufferCreateInfo.Images.GetLength();
	vk_framebuffer_create_info.width = framebufferCreateInfo.Extent.Width;
	vk_framebuffer_create_info.height = framebufferCreateInfo.Extent.Height;
	vk_framebuffer_create_info.layers = 1;
	vk_framebuffer_create_info.renderPass = static_cast<VulkanRenderPass*>(framebufferCreateInfo.RenderPass)->GetVkRenderPass();
	vk_framebuffer_create_info.pAttachments = result.GetData();

	VK_CHECK(vkCreateFramebuffer(vulkanRenderDevice->GetVkDevice(), &vk_framebuffer_create_info, vulkanRenderDevice->GetVkAllocationCallbacks(), &framebuffer));
}

void VulkanFramebuffer::Destroy(GAL::RenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyFramebuffer(vk_render_device->GetVkDevice(), framebuffer, vk_render_device->GetVkAllocationCallbacks());
}