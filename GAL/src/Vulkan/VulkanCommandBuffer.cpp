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

void GAL::VulkanCommandBuffer::EndRenderPass(const EndRenderPassInfo& endRenderPassInfo)
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

void GAL::VulkanCommandBuffer::BindPipeline(const BindPipelineInfo& bindPipelineInfo)
{
	vkCmdBindPipeline(commandBuffer, static_cast<VkPipelineBindPoint>(bindPipelineInfo.PipelineType), bindPipelineInfo.Pipeline.GetVkPipeline());
}

void GAL::VulkanCommandBuffer::BindIndexBuffer(const BindIndexBufferInfo& buffer) const
{
	vkCmdBindIndexBuffer(commandBuffer, buffer.Buffer.GetVkBuffer(), buffer.Offset, static_cast<VkIndexType>(buffer.IndexType));
}

void GAL::VulkanCommandBuffer::BindVertexBuffer(const BindVertexBufferInfo& buffer) const
{
	auto vkBuffer = buffer.Buffer.GetVkBuffer();
	GTSL::uint64 offset = buffer.Offset;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, &offset);
}

void GAL::VulkanCommandBuffer::UpdatePushConstant(const UpdatePushConstantsInfo& info)
{
	vkCmdPushConstants(commandBuffer, info.PipelineLayout.GetVkPipelineLayout(), info.ShaderStages, info.Offset, info.Size, info.Data);
}

void GAL::VulkanCommandBuffer::Draw(const DrawInfo& info) const
{
	vkCmdDraw(commandBuffer, info.VertexCount, info.InstanceCount, info.FirstVertex, info.FirstInstance);
}

void GAL::VulkanCommandBuffer::DrawIndexed(const DrawIndexedInfo& drawIndexedInfo) const
{
	vkCmdDrawIndexed(commandBuffer, drawIndexedInfo.IndexCount, drawIndexedInfo.InstanceCount, 0, 0, 0);
}

void GAL::VulkanCommandBuffer::TraceRays(const TraceRaysInfo& traceRaysInfo)
{
	VkStridedDeviceAddressRegionKHR raygenSBT, hitSBT, missSBT, callableSBT;
	raygenSBT.deviceAddress = traceRaysInfo.ShaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].Address;
	raygenSBT.size = traceRaysInfo.ShaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].Size;
	raygenSBT.stride = traceRaysInfo.ShaderTableDescriptors[GAL::RAY_GEN_TABLE_INDEX].Stride;

	hitSBT.deviceAddress = traceRaysInfo.ShaderTableDescriptors[GAL::HIT_TABLE_INDEX].Address;
	hitSBT.size = traceRaysInfo.ShaderTableDescriptors[GAL::HIT_TABLE_INDEX].Size;
	hitSBT.stride = traceRaysInfo.ShaderTableDescriptors[GAL::HIT_TABLE_INDEX].Stride;

	missSBT.deviceAddress = traceRaysInfo.ShaderTableDescriptors[GAL::MISS_TABLE_INDEX].Address;
	missSBT.size = traceRaysInfo.ShaderTableDescriptors[GAL::MISS_TABLE_INDEX].Size;
	missSBT.stride = traceRaysInfo.ShaderTableDescriptors[GAL::MISS_TABLE_INDEX].Stride;

	callableSBT.deviceAddress = traceRaysInfo.ShaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].Address;
	callableSBT.size = traceRaysInfo.ShaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].Size;
	callableSBT.stride = traceRaysInfo.ShaderTableDescriptors[GAL::CALLABLE_TABLE_INDEX].Stride;
	
	traceRaysInfo.RenderDevice->vkCmdTraceRaysKHR(commandBuffer, &raygenSBT, &missSBT, &hitSBT, &callableSBT, traceRaysInfo.DispatchSize.Width, traceRaysInfo.DispatchSize.Height, traceRaysInfo.DispatchSize.Depth);
}

void GAL::VulkanCommandBuffer::AddLabel(const AddLabelInfo& info)
{
	VkDebugUtilsLabelEXT vkLabelInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
	vkLabelInfo.pLabelName = info.Name.begin();
	
	info.RenderDevice->vkCmdInsertDebugUtilsLabelEXT(commandBuffer, &vkLabelInfo);
}

void GAL::VulkanCommandBuffer::BeginRegion(const BeginRegionInfo& info) const
{
	VkDebugUtilsLabelEXT vkLabelInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
	vkLabelInfo.pLabelName = info.Name.begin();
	
	info.RenderDevice->vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &vkLabelInfo);
}

void GAL::VulkanCommandBuffer::EndRegion(const EndRegionInfo& info) const
{
	info.RenderDevice->vkCmdEndDebugUtilsLabelEXT(commandBuffer);	
}

void GAL::VulkanCommandBuffer::Dispatch(const DispatchInfo& dispatchInfo)
{
	vkCmdDispatch(commandBuffer, dispatchInfo.WorkGroups.Width, dispatchInfo.WorkGroups.Height, dispatchInfo.WorkGroups.Depth);
}

void GAL::VulkanCommandBuffer::BindBindingsSets(const BindBindingsSetInfo& info)
{
	vkCmdBindDescriptorSets(commandBuffer, static_cast<VkPipelineBindPoint>(info.PipelineType), info.PipelineLayout.GetVkPipelineLayout(), info.FirstSet,
	info.BoundSets, reinterpret_cast<const VkDescriptorSet*>(info.BindingsSets.begin()), info.Offsets.ElementCount(), info.Offsets.begin());
}

void GAL::VulkanCommandBuffer::CopyTextureToTexture(const CopyTextureToTextureInfo& info)
{
	VkImageCopy vkImageCopy;
	vkImageCopy.extent = Extent3DToVkExtent3D(info.Extent);
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

	vkCmdCopyImage(commandBuffer, info.SourceTexture.GetVkImage(), static_cast<VkImageLayout>(info.SourceLayout),
		info.DestinationTexture.GetVkImage(), static_cast<VkImageLayout>(info.DestinationLayout), 1, &vkImageCopy);
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

void GAL::VulkanCommandBuffer::AddPipelineBarrier(const AddPipelineBarrierInfo& pipelineBarrier) const
{
	Array<VkImageMemoryBarrier, 32> imageMemoryBarriers(pipelineBarrier.TextureBarriers.ElementCount());
	//GTSL::Array<VkBufferMemoryBarrier, 1024> bufferMemoryBarriers;
	Array<VkMemoryBarrier, 32> memoryBarriers(pipelineBarrier.MemoryBarriers.ElementCount());

	for (uint32 i = 0; i < pipelineBarrier.TextureBarriers.ElementCount(); ++i)
	{
		imageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarriers[i].pNext = nullptr;
		imageMemoryBarriers[i].oldLayout = static_cast<VkImageLayout>(pipelineBarrier.TextureBarriers[i].CurrentLayout);
		imageMemoryBarriers[i].newLayout = static_cast<VkImageLayout>(pipelineBarrier.TextureBarriers[i].TargetLayout);
		imageMemoryBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarriers[i].image = pipelineBarrier.TextureBarriers[i].Texture.GetVkImage();
		imageMemoryBarriers[i].subresourceRange.aspectMask = (VkImageAspectFlags)pipelineBarrier.TextureBarriers[i].TextureType;
		imageMemoryBarriers[i].subresourceRange.baseMipLevel = 0;
		imageMemoryBarriers[i].subresourceRange.levelCount = 1;
		imageMemoryBarriers[i].subresourceRange.baseArrayLayer = 0;
		imageMemoryBarriers[i].subresourceRange.layerCount = 1;
		imageMemoryBarriers[i].srcAccessMask = static_cast<VkAccessFlags>(pipelineBarrier.TextureBarriers[i].SourceAccessFlags);
		imageMemoryBarriers[i].dstAccessMask = static_cast<VkAccessFlags>(pipelineBarrier.TextureBarriers[i].DestinationAccessFlags);
	}

	for(uint32 i = 0; i < pipelineBarrier.MemoryBarriers.ElementCount(); ++i)
	{
		memoryBarriers[i].sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarriers[i].pNext = nullptr;
		memoryBarriers[i].srcAccessMask = static_cast<VkAccessFlags>(pipelineBarrier.MemoryBarriers[i].SourceAccessFlags);
		memoryBarriers[i].dstAccessMask = static_cast<VkAccessFlags>(pipelineBarrier.MemoryBarriers[i].DestinationAccessFlags);
	}
	
	vkCmdPipelineBarrier(commandBuffer, static_cast<VkPipelineStageFlags>(pipelineBarrier.InitialStage), static_cast<VkPipelineStageFlags>(pipelineBarrier.FinalStage), 0,
		memoryBarriers.GetLength(), memoryBarriers.begin(), 0, nullptr, imageMemoryBarriers.GetLength(), imageMemoryBarriers.begin());
}

void GAL::VulkanCommandBuffer::CopyBuffers(const CopyBuffersInfo& copyBuffersInfo)
{
	VkBufferCopy vk_buffer_copy;
	vk_buffer_copy.size = copyBuffersInfo.Size;
	vk_buffer_copy.srcOffset = copyBuffersInfo.SourceOffset;
	vk_buffer_copy.dstOffset = copyBuffersInfo.DestinationOffset;
	vkCmdCopyBuffer(commandBuffer, copyBuffersInfo.Source.GetVkBuffer(), copyBuffersInfo.Destination.GetVkBuffer(), 1, &vk_buffer_copy);
}

void GAL::VulkanCommandBuffer::BuildAccelerationStructure(const BuildAccelerationStructuresInfo& info) const
{	
	GTSL::Array<VkAccelerationStructureBuildGeometryInfoKHR, 8> buildGeometryInfos;
	GTSL::Array<GTSL::Array<VkAccelerationStructureGeometryKHR, 8>, 8> geometriesPerAccelerationStructure;
	GTSL::Array<GTSL::Array<VkAccelerationStructureBuildRangeInfoKHR, 8>, 8> buildRangesPerAccelerationStructure;
	GTSL::Array<VkAccelerationStructureBuildRangeInfoKHR*, 8> buildRangesRangePerAccelerationStructure;

	for (GTSL::uint32 accStrInfoIndex = 0; accStrInfoIndex < info.BuildAccelerationStructureInfos.ElementCount(); ++accStrInfoIndex)
	{
		auto& source = info.BuildAccelerationStructureInfos[accStrInfoIndex];

		geometriesPerAccelerationStructure.EmplaceBack();
		buildRangesPerAccelerationStructure.EmplaceBack();
		buildRangesRangePerAccelerationStructure.EmplaceBack(buildRangesPerAccelerationStructure[accStrInfoIndex].begin());
		
		for(uint32 i = 0; i < source.Geometries.ElementCount(); ++i)
		{
			VkAccelerationStructureGeometryKHR accelerationStructureGeometry; VkAccelerationStructureBuildRangeInfoKHR buildRange;
			buildGeometryAndRange(source.Geometries[i], accelerationStructureGeometry, buildRange);
			geometriesPerAccelerationStructure[accStrInfoIndex].EmplaceBack(accelerationStructureGeometry);
			buildRangesPerAccelerationStructure[accStrInfoIndex].EmplaceBack(buildRange);
		}

		VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
		buildGeometryInfo.flags = source.Flags;
		buildGeometryInfo.srcAccelerationStructure = source.SourceAccelerationStructure.GetVkAccelerationStructure();
		buildGeometryInfo.dstAccelerationStructure = source.DestinationAccelerationStructure.GetVkAccelerationStructure();
		buildGeometryInfo.type = source.Geometries[0].Type == VulkanGeometryType::INSTANCES ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildGeometryInfo.pGeometries = geometriesPerAccelerationStructure[accStrInfoIndex].begin();
		buildGeometryInfo.ppGeometries = nullptr;
		buildGeometryInfo.geometryCount = geometriesPerAccelerationStructure[accStrInfoIndex].GetLength();
		buildGeometryInfo.scratchData.deviceAddress = source.ScratchBufferAddress;
		buildGeometryInfo.mode = source.SourceAccelerationStructure.GetVkAccelerationStructure() ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildGeometryInfos.EmplaceBack(buildGeometryInfo);
	}
	
	info.RenderDevice->vkCmdBuildAccelerationStructuresKHR(commandBuffer, info.BuildAccelerationStructureInfos.ElementCount(),
		buildGeometryInfos.begin(), buildRangesRangePerAccelerationStructure.begin());
}

GAL::VulkanCommandPool::VulkanCommandPool(const CreateInfo& createInfo)
{
	VkCommandPoolCreateInfo vkCommandPoolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	vkCommandPoolCreateInfo.queueFamilyIndex = createInfo.Queue->GetFamilyIndex();
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
