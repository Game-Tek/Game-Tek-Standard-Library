#include "GAL/Vulkan/VulkanCommandBuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanBuffer.h"
#include "GAL/Vulkan/VulkanFramebuffer.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include "GAL/Vulkan/VulkanTexture.h"

using namespace GTSL;

void GAL::VulkanCommandBuffer::BeginRecording(const BeginRecordingInfo& beginRecordingInfo)
{
	VkCommandBufferBeginInfo vkCommandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	//Hint to primary buffer if this is secondary.
	//vk_command_buffer_begin_info.pInheritanceInfo = static_cast<VulkanCommandBuffer*>(beginRecordingInfo.PrimaryCommandBuffer)->GetVkCommandBuffer();
	vkCommandBufferBeginInfo.pInheritanceInfo = nullptr;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &vkCommandBufferBeginInfo))
}

void GAL::VulkanCommandBuffer::EndRecording(const EndRecordingInfo& endRecordingInfo)
{
	VK_CHECK(vkEndCommandBuffer(commandBuffer))
}

void GAL::VulkanCommandBuffer::BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo)
{
	VkRenderPassBeginInfo vkRenderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	vkRenderPassBeginInfo.renderPass = beginRenderPassInfo.RenderPass.GetVkRenderPass();

	Array<VkClearValue, 32> vkClearValues(static_cast<uint32>(beginRenderPassInfo.ClearValues.ElementCount()));
	
	for(uint8 i = 0; i < static_cast<uint8>(beginRenderPassInfo.ClearValues.ElementCount()); ++i)
	{
		const auto& color = beginRenderPassInfo.ClearValues[i];
		vkClearValues[i] = VkClearValue{ color.R(), color.G(), color.B(), color.A() };
	}
	
	vkRenderPassBeginInfo.pClearValues = vkClearValues.begin();
	vkRenderPassBeginInfo.clearValueCount = vkClearValues.GetLength();
	vkRenderPassBeginInfo.framebuffer = beginRenderPassInfo.Framebuffer.GetVkFramebuffer();
	vkRenderPassBeginInfo.renderArea.extent = Extent2DToVkExtent2D(beginRenderPassInfo.RenderArea);
	vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };

	vkCmdBeginRenderPass(commandBuffer, &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

void GAL::VulkanCommandBuffer::EndRenderPass(const VulkanRenderDevice* renderDevice)
{
	vkCmdEndRenderPass(commandBuffer);
}

void GAL::VulkanCommandBuffer::SetScissor(const SetScissorInfo& info)
{
	VkRect2D scissor;
	scissor.extent = { info.Area.Width, info.Area.Height };
	scissor.offset = { info.Offset.Width, info.Offset.Height };
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void GAL::VulkanCommandBuffer::BindPipeline(const VulkanRenderDevice* renderDevice, VulkanPipeline pipeline, VulkanPipelineType pipelineType)
{
	vkCmdBindPipeline(commandBuffer, static_cast<VkPipelineBindPoint>(pipelineType), pipeline.GetVkPipeline());
}

void GAL::VulkanCommandBuffer::BindIndexBuffer(const VulkanRenderDevice* renderDevice, VulkanBuffer buffer, GTSL::uint32 offset, VulkanIndexType indexType) const
{
	vkCmdBindIndexBuffer(commandBuffer, buffer.GetVkBuffer(), offset, static_cast<VkIndexType>(indexType));
}

void GAL::VulkanCommandBuffer::BindVertexBuffer(const VulkanRenderDevice* renderDevice, VulkanBuffer buffer, GTSL::uint32 offset) const
{
	auto vkBuffer = buffer.GetVkBuffer();
	GTSL::uint64 bigOffset = offset;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, &bigOffset);
}

void GAL::VulkanCommandBuffer::UpdatePushConstant(const VulkanRenderDevice* vulkanRenderDevice, VulkanPipelineLayout pipelineLayout, GTSL::uint32 offset, GTSL::Range<const byte*> data, VulkanShaderStage::value_type shaderStages)
{
	vkCmdPushConstants(commandBuffer, pipelineLayout.GetVkPipelineLayout(), shaderStages, offset, data.Bytes(), data.begin());
}

void GAL::VulkanCommandBuffer::Draw(const VulkanRenderDevice* renderDevice, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
{
	vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void GAL::VulkanCommandBuffer::DrawIndexed(const VulkanRenderDevice* renderDevice, uint32_t indexCount, uint32_t instanceCount) const
{
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
}

void GAL::VulkanCommandBuffer::TraceRays(const VulkanRenderDevice* renderDevice, GTSL::Array<ShaderTableDescriptor, 4> shaderTableDescriptors, GTSL::Extent3D dispatchSize)
{
	VkStridedDeviceAddressRegionKHR raygenSBT, hitSBT, missSBT, callableSBT;
	raygenSBT.deviceAddress = shaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].Address;
	raygenSBT.size = shaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].Entries * shaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].EntrySize;
	raygenSBT.stride = shaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].EntrySize;

	hitSBT.deviceAddress = shaderTableDescriptors[GAL::HIT_TABLE_INDEX].Address;
	hitSBT.size = shaderTableDescriptors[GAL::HIT_TABLE_INDEX].Entries * shaderTableDescriptors[GAL::HIT_TABLE_INDEX].EntrySize;
	hitSBT.stride = shaderTableDescriptors[GAL::HIT_TABLE_INDEX].EntrySize;

	missSBT.deviceAddress = shaderTableDescriptors[GAL::MISS_TABLE_INDEX].Address;
	missSBT.size = shaderTableDescriptors[GAL::MISS_TABLE_INDEX].Entries * shaderTableDescriptors[GAL::MISS_TABLE_INDEX].EntrySize;
	missSBT.stride = shaderTableDescriptors[GAL::MISS_TABLE_INDEX].EntrySize;

	callableSBT.deviceAddress = shaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].Address;
	callableSBT.size = shaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].Entries * shaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].EntrySize;
	callableSBT.stride = shaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].EntrySize;

	renderDevice->vkCmdTraceRaysKHR(commandBuffer, &raygenSBT, &missSBT, &hitSBT, &callableSBT, dispatchSize.Width, dispatchSize.Height, dispatchSize.Depth);
}

void GAL::VulkanCommandBuffer::AddLabel(const VulkanRenderDevice* renderDevice, GTSL::Range<const GTSL::UTF8*> name)
{
	VkDebugUtilsLabelEXT vkLabelInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
	vkLabelInfo.pLabelName = name.begin();
	renderDevice->vkCmdInsertDebugUtilsLabelEXT(commandBuffer, &vkLabelInfo);
}

void GAL::VulkanCommandBuffer::BeginRegion(const VulkanRenderDevice* renderDevice, GTSL::Range<const GTSL::UTF8*> name) const
{
	VkDebugUtilsLabelEXT vkLabelInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
	vkLabelInfo.pLabelName = name.begin();

	renderDevice->vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &vkLabelInfo);
}

void GAL::VulkanCommandBuffer::EndRegion(const VulkanRenderDevice* renderDevice) const
{
	renderDevice->vkCmdEndDebugUtilsLabelEXT(commandBuffer);
}

void GAL::VulkanCommandBuffer::Dispatch(const VulkanRenderDevice* renderDevice, GTSL::Extent3D workGroups)
{
	vkCmdDispatch(commandBuffer, workGroups.Width, workGroups.Height, workGroups.Depth);
}

void GAL::VulkanCommandBuffer::BindBindingsSets(const VulkanRenderDevice* renderDevice, VulkanPipelineType pipelineType,
	GTSL::Range<const VulkanBindingsSet*> bindingsSets, GTSL::Range<const GTSL::uint32*> offsets,
	VulkanPipelineLayout pipelineLayout, GTSL::uint32 firstSet)
{
	vkCmdBindDescriptorSets(commandBuffer, static_cast<VkPipelineBindPoint>(pipelineType), pipelineLayout.GetVkPipelineLayout(), firstSet,
		bindingsSets.ElementCount(), reinterpret_cast<const VkDescriptorSet*>(bindingsSets.begin()), offsets.ElementCount(), offsets.begin());
}

void GAL::VulkanCommandBuffer::CopyTextureToTexture(const VulkanRenderDevice* renderDevice, VulkanTexture sourceTexture, VulkanTexture destinationTexture, VulkanTextureLayout sourceLayout, VulkanTextureLayout destinationLayout, GTSL::Extent3D extent)
{
	VkImageCopy vkImageCopy;
	vkImageCopy.extent = Extent3DToVkExtent3D(extent);
	vkImageCopy.srcOffset = {};
	vkImageCopy.dstOffset = {};
	vkImageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageCopy.srcSubresource.baseArrayLayer = 0;
	vkImageCopy.srcSubresource.layerCount = 1;
	vkImageCopy.srcSubresource.mipLevel = 0;

	vkImageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageCopy.dstSubresource.baseArrayLayer = 0;
	vkImageCopy.dstSubresource.layerCount = 1;
	vkImageCopy.dstSubresource.mipLevel = 0;

	vkCmdCopyImage(commandBuffer, sourceTexture.GetVkImage(), static_cast<VkImageLayout>(sourceLayout),
		destinationTexture.GetVkImage(), static_cast<VkImageLayout>(destinationLayout), 1, &vkImageCopy);
}

void GAL::VulkanCommandBuffer::CopyBufferToTexture(const CopyBufferToTextureInfo& copyBufferToImageInfo)
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
	vkCmdCopyBufferToImage(commandBuffer, copyBufferToImageInfo.SourceBuffer.GetVkBuffer(), copyBufferToImageInfo.DestinationTexture.GetVkImage(),
		static_cast<VkImageLayout>(copyBufferToImageInfo.TextureLayout), 1, &region);
}

void GAL::VulkanCommandBuffer::CopyBuffers(const CopyBuffersInfo& copyBuffersInfo)
{
	VkBufferCopy vk_buffer_copy;
	vk_buffer_copy.size = copyBuffersInfo.Size;
	vk_buffer_copy.srcOffset = copyBuffersInfo.SourceOffset;
	vk_buffer_copy.dstOffset = copyBuffersInfo.DestinationOffset;
	vkCmdCopyBuffer(commandBuffer, copyBuffersInfo.Source.GetVkBuffer(), copyBuffersInfo.Destination.GetVkBuffer(), 1, &vk_buffer_copy);
}

void GAL::VulkanCommandBuffer::SetEvent(GAL::VulkanEvent event, GAL::VulkanPipelineStage pipelineStage)
{
	vkCmdSetEvent(commandBuffer, event.GetVkEvent(), pipelineStage);
}

void GAL::VulkanCommandBuffer::ResetEvent(GAL::VulkanEvent event, GAL::VulkanPipelineStage pipelineStage)
{
	vkCmdResetEvent(commandBuffer, event.GetVkEvent(), pipelineStage);
}

void GAL::VulkanCommandPool::Initialize(const CreateInfo& createInfo)
{
	VkCommandPoolCreateInfo vkCommandPoolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	vkCommandPoolCreateInfo.queueFamilyIndex = createInfo.Queue.GetFamilyIndex();
	VK_CHECK(vkCreateCommandPool(createInfo.RenderDevice->GetVkDevice(), &vkCommandPoolCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &commandPool))

	SET_NAME(commandPool, VK_OBJECT_TYPE_COMMAND_POOL, createInfo)
}

void GAL::VulkanCommandPool::ResetPool(RenderDevice* renderDevice) const
{
	VK_CHECK(vkResetCommandPool(static_cast<const VulkanRenderDevice*>(renderDevice)->GetVkDevice(), commandPool, 0))
}

void GAL::VulkanCommandPool::AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo) const
{
	VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	vkCommandBufferAllocateInfo.commandPool = commandPool;
	vkCommandBufferAllocateInfo.level = allocateCommandBuffersInfo.IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	vkCommandBufferAllocateInfo.commandBufferCount = allocateCommandBuffersInfo.CommandBuffers.ElementCount();

	Array<VkCommandBuffer, 16> command_buffers(allocateCommandBuffersInfo.CommandBuffers.ElementCount());

	VK_CHECK(vkAllocateCommandBuffers(allocateCommandBuffersInfo.RenderDevice->GetVkDevice(), &vkCommandBufferAllocateInfo, command_buffers.begin()));

	if constexpr (_DEBUG)
	{
		for (uint32 i = 0; i < allocateCommandBuffersInfo.CommandBuffers.ElementCount(); ++i)
		{
			allocateCommandBuffersInfo.CommandBuffers[i].commandBuffer = command_buffers[i];
			SET_NAME(command_buffers[i], VK_OBJECT_TYPE_COMMAND_BUFFER, allocateCommandBuffersInfo.CommandBufferCreateInfos[i])
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
