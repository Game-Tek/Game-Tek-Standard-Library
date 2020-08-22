#include "GAL/Vulkan/VulkanCommandBuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanBuffer.h"
#include "GAL/Vulkan/VulkanFramebuffer.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include "GAL/Vulkan/VulkanTexture.h"

void GAL::VulkanCommandBuffer::BeginRecording(const BeginRecordingInfo& beginRecordingInfo)
{
	VkCommandBufferBeginInfo vk_command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	//Hint to primary buffer if this is secondary.
	//vk_command_buffer_begin_info.pInheritanceInfo = static_cast<VulkanCommandBuffer*>(beginRecordingInfo.PrimaryCommandBuffer)->GetVkCommandBuffer();
	vk_command_buffer_begin_info.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &vk_command_buffer_begin_info));
}

void GAL::VulkanCommandBuffer::EndRecording(const EndRecordingInfo& endRecordingInfo)
{
	VK_CHECK(vkEndCommandBuffer(commandBuffer));
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

	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0f;
	viewport.width = beginRenderPassInfo.RenderArea.Width;
	viewport.height = beginRenderPassInfo.RenderArea.Height;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor;
	scissor.extent.width = beginRenderPassInfo.RenderArea.Width;
	scissor.extent.height = beginRenderPassInfo.RenderArea.Height;
	scissor.offset = { 0, 0 };
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void GAL::VulkanCommandBuffer::AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo)
{
	vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
}

void GAL::VulkanCommandBuffer::EndRenderPass(const EndRenderPassInfo& endRenderPassInfo)
{
	vkCmdEndRenderPass(commandBuffer);
}

void GAL::VulkanCommandBuffer::BindPipeline(const BindPipelineInfo& bindPipelineInfo)
{
	vkCmdBindPipeline(commandBuffer, static_cast<VkPipelineBindPoint>(bindPipelineInfo.PipelineType), bindPipelineInfo.Pipeline->GetVkPipeline());
}

void GAL::VulkanCommandBuffer::BindIndexBuffer(const BindIndexBufferInfo& buffer) const
{
	vkCmdBindIndexBuffer(commandBuffer, buffer.Buffer->GetVkBuffer(), buffer.Offset, static_cast<VkIndexType>(buffer.IndexType));
}

void GAL::VulkanCommandBuffer::BindVertexBuffer(const BindVertexBufferInfo& buffer) const
{
	auto* vk_buffer = static_cast<const VulkanBuffer*>(buffer.Buffer)->GetVkBuffer();
	GTSL::uint64 offset = buffer.Offset;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vk_buffer, &offset);
}

void GAL::VulkanCommandBuffer::UpdatePushConstant(const UpdatePushConstantsInfo& info)
{
	vkCmdPushConstants(commandBuffer, info.PipelineLayout->GetVkPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, info.Offset, info.Size, info.Data);
}

void GAL::VulkanCommandBuffer::DrawIndexed(const DrawIndexedInfo& drawIndexedInfo) const
{
	vkCmdDrawIndexed(commandBuffer, drawIndexedInfo.IndexCount, drawIndexedInfo.InstanceCount, 0, 0, 0);
}

void GAL::VulkanCommandBuffer::TraceRays(const TraceRaysInfo& traceRaysInfo)
{
	VkStridedBufferRegionKHR raygen_sbt, hit_sbt, miss_sbt;
	raygen_sbt.size = traceRaysInfo.RaygenDescriptor.Size;
	raygen_sbt.offset = traceRaysInfo.RaygenDescriptor.Offset;
	raygen_sbt.buffer = traceRaysInfo.RaygenDescriptor.Buffer->GetVkBuffer();
	raygen_sbt.stride = traceRaysInfo.RaygenDescriptor.Stride;

	hit_sbt.size = traceRaysInfo.HitDescriptor.Size;
	hit_sbt.offset = traceRaysInfo.HitDescriptor.Offset;
	hit_sbt.buffer = traceRaysInfo.HitDescriptor.Buffer->GetVkBuffer();
	hit_sbt.stride = traceRaysInfo.HitDescriptor.Stride;

	miss_sbt.size = traceRaysInfo.MissDescriptor.Size;
	miss_sbt.offset = traceRaysInfo.MissDescriptor.Offset;
	miss_sbt.buffer = traceRaysInfo.MissDescriptor.Buffer->GetVkBuffer();
	miss_sbt.stride = traceRaysInfo.MissDescriptor.Stride;
	
	traceRaysInfo.RenderDevice->vkCmdTraceRaysKHR(commandBuffer, &raygen_sbt, &miss_sbt, &hit_sbt, nullptr, traceRaysInfo.DispatchSize.Width, traceRaysInfo.DispatchSize.Height, traceRaysInfo.DispatchSize.Depth);
}

void GAL::VulkanCommandBuffer::Dispatch(const DispatchInfo& dispatchInfo)
{
	vkCmdDispatch(commandBuffer, dispatchInfo.WorkGroups.Width, dispatchInfo.WorkGroups.Height, dispatchInfo.WorkGroups.Depth);
}

void GAL::VulkanCommandBuffer::BindBindingsSets(const BindBindingsSetInfo& info)
{
	vkCmdBindDescriptorSets(commandBuffer, static_cast<VkPipelineBindPoint>(info.PipelineType), info.PipelineLayout->GetVkPipelineLayout(), info.FirstSet,
	info.BoundSets, reinterpret_cast<const VkDescriptorSet*>(info.BindingsSets.begin()), info.Offsets.ElementCount(), info.Offsets.begin());
}

void GAL::VulkanCommandBuffer::CopyImage(const CopyImageInfo& copyImageInfo)
{
	__debugbreak();
	//vkCmdCopyImage(commandBuffer, static_cast<VulkanImage*>(copyImageInfo.SourceImage), , , , , );
}

void GAL::VulkanCommandBuffer::CopyBufferToImage(const CopyBufferToImageInfo& copyBufferToImageInfo)
{
	VkBufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = VkOffset3D{ copyBufferToImageInfo.Offset.Width, copyBufferToImageInfo.Offset.Depth, copyBufferToImageInfo.Offset.Height };
	region.imageExtent = Extent3DToVkExtent3D(copyBufferToImageInfo.Extent);
	vkCmdCopyBufferToImage(commandBuffer, copyBufferToImageInfo.SourceBuffer->GetVkBuffer(), copyBufferToImageInfo.DestinationImage->GetVkImage(),
		static_cast<VkImageLayout>(copyBufferToImageInfo.TextureLayout), 1, &region);
}

void GAL::VulkanCommandBuffer::AddPipelineBarrier(const AddPipelineBarrierInfo& pipelineBarrier)
{
	GTSL::Array<VkImageMemoryBarrier, 1024> imageMemoryBarriers(pipelineBarrier.TextureBarriers.ElementCount());
	//GTSL::Array<VkBufferMemoryBarrier, 1024> bufferMemoryBarriers;
	//GTSL::Array<VkMemoryBarrier, 1024> memoryBarriers;

	for (GTSL::uint32 i = 0; i < pipelineBarrier.TextureBarriers.ElementCount(); ++i)
	{
		imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarriers[i].pNext = nullptr;
		imageMemoryBarriers[i].oldLayout = static_cast<VkImageLayout>(pipelineBarrier.TextureBarriers[i].CurrentLayout);
		imageMemoryBarriers[i].newLayout = static_cast<VkImageLayout>(pipelineBarrier.TextureBarriers[i].TargetLayout);
		imageMemoryBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarriers[i].image = pipelineBarrier.TextureBarriers[i].Texture.GetVkImage();
		imageMemoryBarriers[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarriers[i].subresourceRange.baseMipLevel = 0;
		imageMemoryBarriers[i].subresourceRange.levelCount = 1;
		imageMemoryBarriers[i].subresourceRange.baseArrayLayer = 0;
		imageMemoryBarriers[i].subresourceRange.layerCount = 1;
		imageMemoryBarriers[i].srcAccessMask = static_cast<VkAccessFlags>(pipelineBarrier.TextureBarriers[i].SourceAccessFlags);
		imageMemoryBarriers[i].dstAccessMask = static_cast<VkAccessFlags>(pipelineBarrier.TextureBarriers[i].DestinationAccessFlags);
	}
	
	vkCmdPipelineBarrier(commandBuffer, static_cast<VkPipelineStageFlags>(pipelineBarrier.InitialStage), static_cast<VkPipelineStageFlags>(pipelineBarrier.FinalStage), 0, 0, nullptr, 0, nullptr, imageMemoryBarriers.GetLength(), imageMemoryBarriers.begin());
}

void GAL::VulkanCommandBuffer::CopyBuffers(const CopyBuffersInfo& copyBuffersInfo)
{
	VkBufferCopy vk_buffer_copy;
	vk_buffer_copy.size = copyBuffersInfo.Size;
	vk_buffer_copy.srcOffset = copyBuffersInfo.SourceOffset;
	vk_buffer_copy.dstOffset = copyBuffersInfo.DestinationOffset;
	vkCmdCopyBuffer(commandBuffer, copyBuffersInfo.Source->GetVkBuffer(), copyBuffersInfo.Destination->GetVkBuffer(), 1, &vk_buffer_copy);
}

GAL::VulkanCommandPool::VulkanCommandPool(const CreateInfo& createInfo)
{
	VkCommandPoolCreateInfo vk_command_pool_create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	vk_command_pool_create_info.queueFamilyIndex = createInfo.Queue->GetFamilyIndex();
	VK_CHECK(vkCreateCommandPool(createInfo.RenderDevice->GetVkDevice(), &vk_command_pool_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &commandPool));

	SET_NAME(commandPool, VK_OBJECT_TYPE_COMMAND_POOL, createInfo);
}

void GAL::VulkanCommandPool::ResetPool(RenderDevice* renderDevice) const
{
	VK_CHECK(vkResetCommandPool(static_cast<const VulkanRenderDevice*>(renderDevice)->GetVkDevice(), commandPool, 0));
}

void GAL::VulkanCommandPool::AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo)
{
	VkCommandBufferAllocateInfo vk_command_buffer_allocate_info{ vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	vk_command_buffer_allocate_info.commandPool = commandPool;
	vk_command_buffer_allocate_info.level = allocateCommandBuffersInfo.IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	vk_command_buffer_allocate_info.commandBufferCount = allocateCommandBuffersInfo.CommandBuffers.ElementCount();

	GTSL::Array<VkCommandBuffer, 16> command_buffers(allocateCommandBuffersInfo.CommandBuffers.ElementCount());

	VK_CHECK(vkAllocateCommandBuffers(allocateCommandBuffersInfo.RenderDevice->GetVkDevice(), &vk_command_buffer_allocate_info, command_buffers.begin()));

	if constexpr (_DEBUG)
	{
		for (GTSL::uint32 i = 0; i < allocateCommandBuffersInfo.CommandBuffers.ElementCount(); ++i)
		{
			allocateCommandBuffersInfo.CommandBuffers[i].commandBuffer = command_buffers[i];
			SET_NAME(command_buffers[i], VK_OBJECT_TYPE_COMMAND_BUFFER, allocateCommandBuffersInfo.CommandBufferCreateInfos[i]);
		}
	}
}

void GAL::VulkanCommandPool::FreeCommandBuffers(const struct FreeCommandBuffersInfo& freeCommandBuffers) const
{
	vkFreeCommandBuffers(freeCommandBuffers.RenderDevice->GetVkDevice(), commandPool, freeCommandBuffers.CommandBuffers.ElementCount(),	reinterpret_cast<const VkCommandBuffer*>(freeCommandBuffers.CommandBuffers.begin()));
	for(auto& e : freeCommandBuffers.CommandBuffers) { debugClear(reinterpret_cast<VkCommandBuffer&>(e)); }
}

void GAL::VulkanCommandPool::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyCommandPool(renderDevice->GetVkDevice(), commandPool, renderDevice->GetVkAllocationCallbacks());
	debugClear(commandPool);
}
