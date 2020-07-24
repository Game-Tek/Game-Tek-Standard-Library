#include "GAL/Vulkan/VulkanCommandBuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanBuffer.h"
#include "GAL/Vulkan/VulkanFramebuffer.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include "GAL/Vulkan/VulkanImage.h"

void GAL::VulkanCommandBuffer::BeginRecording(const BeginRecordingInfo& beginRecordingInfo)
{
	VkCommandBufferBeginInfo vk_command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	vk_command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	//Hint to primary buffer if this is secondary.
	//vk_command_buffer_begin_info.pInheritanceInfo = static_cast<VulkanCommandBuffer*>(beginRecordingInfo.PrimaryCommandBuffer)->GetVkCommandBuffer();
	vk_command_buffer_begin_info.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(commandBuffer, &vk_command_buffer_begin_info);
}

void GAL::VulkanCommandBuffer::EndRecording(const EndRecordingInfo& endRecordingInfo)
{
	vkEndCommandBuffer(commandBuffer);
}

void GAL::VulkanCommandBuffer::BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo)
{
	VkRenderPassBeginInfo vk_render_pass_begin_info{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	vk_render_pass_begin_info.renderPass = static_cast<const VulkanRenderPass*>(beginRenderPassInfo.RenderPass)->GetVkRenderPass();
	
	GTSL::Array<VkClearValue, 32> vk_clear_clear_values(beginRenderPassInfo.ClearValues.ElementCount());
	
	for(const auto& e : beginRenderPassInfo.ClearValues)
	{
		vk_clear_clear_values.EmplaceBack(VkClearValue{ e.R(), e.G(), e.B(), e.A() });
	}
	
	vk_render_pass_begin_info.pClearValues = vk_clear_clear_values.begin();
	vk_render_pass_begin_info.clearValueCount = vk_clear_clear_values.GetLength();
	vk_render_pass_begin_info.framebuffer = static_cast<const VulkanFramebuffer*>(beginRenderPassInfo.Framebuffer)->GetVkFramebuffer();
	vk_render_pass_begin_info.renderArea.extent = Extent2DToVkExtent2D(beginRenderPassInfo.RenderArea);
	vk_render_pass_begin_info.renderArea.offset = { 0, 0 };

	vkCmdBeginRenderPass(commandBuffer, &vk_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void GAL::VulkanCommandBuffer::AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo)
{
	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
}

void GAL::VulkanCommandBuffer::EndRenderPass(const EndRenderPassInfo& endRenderPassInfo)
{
	vkCmdEndRenderPass(commandBuffer);
}

void GAL::VulkanCommandBuffer::BindGraphicsPipeline(const BindGraphicsPipelineInfo& bindGraphicsPipelineInfo)
{
	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0f;
	viewport.width = bindGraphicsPipelineInfo.RenderExtent.Width;
	viewport.height = bindGraphicsPipelineInfo.RenderExtent.Height;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<const VulkanGraphicsPipeline*>(bindGraphicsPipelineInfo.GraphicsPipeline)->GetVkGraphicsPipeline());
}

void GAL::VulkanCommandBuffer::BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, static_cast<const VulkanComputePipeline*>(bindComputePipelineInfo.Pipeline)->GetVkPipeline());
}

void GAL::VulkanCommandBuffer::BindIndexBuffer(const BindIndexBufferInfo& buffer)
{
	vkCmdBindIndexBuffer(commandBuffer, static_cast<const VulkanBuffer*>(buffer.Buffer)->GetVkBuffer(), buffer.Offset, VK_INDEX_TYPE_UINT16);
}

void GAL::VulkanCommandBuffer::BindVertexBuffer(const BindVertexBufferInfo& buffer)
{
	auto* vk_buffer = static_cast<const VulkanBuffer*>(buffer.Buffer)->GetVkBuffer();
	GTSL::uint64 offset = buffer.Offset;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vk_buffer, &offset);
}

void GAL::VulkanCommandBuffer::UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo)
{
	vkCmdPushConstants(commandBuffer, static_cast<const VulkanGraphicsPipeline*>(updatePushConstantsInfo.Pipeline)->GetVkPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, updatePushConstantsInfo.Offset, updatePushConstantsInfo.Size, updatePushConstantsInfo.Data);
}

void GAL::VulkanCommandBuffer::DrawIndexed(const DrawIndexedInfo& drawIndexedInfo)
{
	vkCmdDrawIndexed(commandBuffer, drawIndexedInfo.IndexCount, drawIndexedInfo.InstanceCount, 0, 0, 0);
}

void GAL::VulkanCommandBuffer::Dispatch(const DispatchInfo& dispatchInfo)
{
	vkCmdDispatch(commandBuffer, dispatchInfo.WorkGroups.Width, dispatchInfo.WorkGroups.Height, dispatchInfo.WorkGroups.Depth);
}

void GAL::VulkanCommandBuffer::BindBindingsSet(const BindBindingsSetInfo& bindBindingsSetInfo)
{
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,	static_cast<const VulkanGraphicsPipeline*>(bindBindingsSetInfo.Pipeline)->GetVkPipelineLayout(), 0,
	bindBindingsSetInfo.BindingsSets.ElementCount(), reinterpret_cast<const VkDescriptorSet*>(bindBindingsSetInfo.BindingsSets.begin()), 0, 0);
}

void GAL::VulkanCommandBuffer::CopyImage(const CopyImageInfo& copyImageInfo)
{
	//vkCmdCopyImage(commandBuffer, static_cast<VulkanImage*>(copyImageInfo.SourceImage), , , , , );
}

void GAL::VulkanCommandBuffer::CopyBufferToImage(const CopyBufferToImageInfo& copyBufferToImageInfo)
{
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	//region.imageOffset = Extent3DToVkExtent3D(copyImageToBufferInfo.Offset);
	region.imageExtent = Extent3DToVkExtent3D(copyBufferToImageInfo.Extent);
	vkCmdCopyBufferToImage(commandBuffer, static_cast<const VulkanBuffer*>(copyBufferToImageInfo.SourceBuffer)->GetVkBuffer(), static_cast<const VulkanImage*>(copyBufferToImageInfo.DestinationImage)->GetVkImage(), ImageLayoutToVkImageLayout(copyBufferToImageInfo.ImageLayout), 1, &region);
}

void GAL::VulkanCommandBuffer::TransitionImage(const TransitionImageInfo& transitionImageInfo)
{	
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = ImageLayoutToVkImageLayout(transitionImageInfo.SourceLayout);
	barrier.newLayout = ImageLayoutToVkImageLayout(transitionImageInfo.DestinationLayout);
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = static_cast<const VulkanImage*>(transitionImageInfo.Texture)->GetVkImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = AccessFlagsToVkAccessFlags(transitionImageInfo.SourceAccessFlags);
	barrier.dstAccessMask = AccessFlagsToVkAccessFlags(transitionImageInfo.DestinationAccessFlags);
	
	vkCmdPipelineBarrier(commandBuffer, PipelineStageToVkPipelineStageFlags(transitionImageInfo.SourceStage), PipelineStageToVkPipelineStageFlags(transitionImageInfo.DestinationStage), 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void GAL::VulkanCommandBuffer::CopyBuffers(const CopyBuffersInfo& copyBuffersInfo)
{
	VkBufferCopy vk_buffer_copy;
	vk_buffer_copy.size = copyBuffersInfo.Size;
	vk_buffer_copy.srcOffset = copyBuffersInfo.SourceOffset;
	vk_buffer_copy.dstOffset = copyBuffersInfo.DestinationOffset;
	vkCmdCopyBuffer(commandBuffer, static_cast<const VulkanBuffer*>(copyBuffersInfo.Source)->GetVkBuffer(), static_cast<const VulkanBuffer*>(copyBuffersInfo.Destination)->GetVkBuffer(),
		1, &vk_buffer_copy);
}

GAL::VulkanCommandPool::VulkanCommandPool(const CreateInfo& createInfo)
{
	const auto vulkan_command_buffers = GTSL::Ranger<VulkanCommandBuffer>(createInfo.CommandBuffers);
	
	VkCommandPoolCreateInfo vk_command_pool_create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	vk_command_pool_create_info.queueFamilyIndex = static_cast<const VulkanQueue*>(createInfo.Queue)->GetFamilyIndex();
	vkCreateCommandPool(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_command_pool_create_info,
		static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &commandPool);

	VkCommandBufferAllocateInfo vk_command_buffer_allocate_info;
	vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vk_command_buffer_allocate_info.pNext = nullptr;
	vk_command_buffer_allocate_info.commandPool = commandPool;
	vk_command_buffer_allocate_info.level = createInfo.IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	vk_command_buffer_allocate_info.commandBufferCount = vulkan_command_buffers.ElementCount();
	
	GTSL::Array<VkCommandBuffer, 16> vk_command_buffers(vulkan_command_buffers.ElementCount());

	vkAllocateCommandBuffers(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_command_buffer_allocate_info, vk_command_buffers.begin());
	
	for(GTSL::uint32 i = 0; i < vulkan_command_buffers.ElementCount(); ++i)
	{
		::new(&vulkan_command_buffers[i]) VulkanCommandBuffer(vk_command_buffers[i]);
	}
}

void GAL::VulkanCommandPool::ResetPool(RenderDevice* renderDevice) const
{
	vkResetCommandPool(static_cast<const VulkanRenderDevice*>(renderDevice)->GetVkDevice(), commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}

void GAL::VulkanCommandPool::FreeCommandBuffers(const struct FreeCommandBuffers& freeCommandBuffers) const
{
	const auto vulkan_command_buffers = GTSL::Ranger<VulkanCommandBuffer>(freeCommandBuffers.CommandBuffers);
	vkFreeCommandBuffers(static_cast<const VulkanRenderDevice*>(freeCommandBuffers.RenderDevice)->GetVkDevice(), commandPool, vulkan_command_buffers.ElementCount(),
		reinterpret_cast<const VkCommandBuffer*>(vulkan_command_buffers.begin()));
}

void GAL::VulkanCommandPool::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyCommandPool(renderDevice->GetVkDevice(), commandPool, renderDevice->GetVkAllocationCallbacks());
}
