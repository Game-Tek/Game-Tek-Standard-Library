#include "GAL/Vulkan/VulkanCommandBuffer.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanBuffer.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanRenderMesh.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include "GAL/Vulkan/VulkanTexture.h"

GAL::VulkanCommandBuffer::VulkanCommandBuffer(VulkanRenderDevice* renderDevice, const CommandBufferCreateInfo& commandBufferCreateInfo) : CommandBuffer(commandBufferCreateInfo)
{
	VkCommandPoolCreateInfo vk_command_pool_create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	vk_command_pool_create_info;

	vkCreateCommandPool(renderDevice->GetVkDevice(), &vk_command_pool_create_info, renderDevice->GetVkAllocationCallbacks(), &commandPool);

	VkCommandBufferAllocateInfo vk_command_buffer_allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	vk_command_buffer_allocate_info.commandPool = commandPool;
	vk_command_buffer_allocate_info.commandBufferCount = 1;
	vk_command_buffer_allocate_info.level = commandBufferCreateInfo.IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	
	vkAllocateCommandBuffers(renderDevice->GetVkDevice(), &vk_command_buffer_allocate_info, &commandBuffer);
}

void GAL::VulkanCommandBuffer::BeginRecording(const BeginRecordingInfo& beginRecordingInfo)
{
	VkCommandBufferBeginInfo vk_command_buffer_begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	vk_command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	//Hint to primary buffer if this is secondary.
	vk_command_buffer_begin_info.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(commandBuffer, &vk_command_buffer_begin_info);
}

void GAL::VulkanCommandBuffer::EndRecording(const EndRecordingInfo& endRecordingInfo)
{
	vkEndCommandBuffer(commandBuffer);
}

void GAL::VulkanCommandBuffer::BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo)
{
	VkRenderPassBeginInfo RenderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	RenderPassBeginInfo.renderPass = static_cast<VulkanRenderPass*>(beginRenderPassInfo.RenderPass)->GetVkRenderPass();
	RenderPassBeginInfo.pClearValues = static_cast<VulkanFramebuffer*>(beginRenderPassInfo.Framebuffer)->GetClearValues().GetData();
	RenderPassBeginInfo.clearValueCount = static_cast<GTSL::uint32>(static_cast<VulkanFramebuffer*>(beginRenderPassInfo.Framebuffer)->GetClearValues().GetLength());
	RenderPassBeginInfo.framebuffer = static_cast<VulkanFramebuffer*>(beginRenderPassInfo.Framebuffer)->GetVkFramebuffer();
	RenderPassBeginInfo.renderArea.extent = Extent2DToVkExtent2D(beginRenderPassInfo.RenderArea);
	RenderPassBeginInfo.renderArea.offset = { 0, 0 };

	vkCmdBeginRenderPass(commandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
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

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VulkanGraphicsPipeline*>(bindGraphicsPipelineInfo.GraphicsPipeline)->GetVkGraphicsPipeline());
}

void GAL::VulkanCommandBuffer::BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, static_cast<VulkanComputePipeline*>(bindComputePipelineInfo.Pipeline)->GetVkPipeline());
}

void GAL::VulkanCommandBuffer::BindMesh(const BindMeshInfo& bindMeshInfo)
{
	const auto mesh = static_cast<VulkanRenderMesh*>(bindMeshInfo.Mesh);
	VkDeviceSize offset = 0;

	VkBuffer vertex_buffers = mesh->GetVkBuffer();

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertex_buffers, &offset);
	vkCmdBindIndexBuffer(commandBuffer, mesh->GetVkBuffer(), mesh->GetIndexBufferOffset(), VK_INDEX_TYPE_UINT16);
}

void GAL::VulkanCommandBuffer::UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo)
{
	vkCmdPushConstants(commandBuffer, static_cast<VulkanGraphicsPipeline*>(updatePushConstantsInfo.Pipeline)->GetVkPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, updatePushConstantsInfo.Offset, updatePushConstantsInfo.Size, updatePushConstantsInfo.Data);
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
	GTSL::Array<VkDescriptorSet, 8> descriptor_sets(bindBindingsSetInfo.BindingsSets->GetLength());
	{
		GTSL::uint8 i = 0;

		for (auto& e : descriptor_sets)
		{
			e = static_cast<VulkanBindingsSet*>((*bindBindingsSetInfo.BindingsSets)[i])->GetVkDescriptorSets()[bindBindingsSetInfo.BindingsSetIndex];
			++i;
		}
	}

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,	static_cast<VulkanGraphicsPipeline*>(bindBindingsSetInfo.Pipeline)->GetVkPipelineLayout(), 0, descriptor_sets.GetLength(), descriptor_sets.GetData(), 0, 0);
}

void GAL::VulkanCommandBuffer::CopyImage(const CopyImageInfo& copyImageInfo)
{
	//vkCmdCopyImage(commandBuffer, copyImageInfo, , , , , );
}

void GAL::VulkanCommandBuffer::CopyBufferToImage(const CopyBufferToImageInfo& copyBufferToImageInfo)
{
	GTSL::FixedVector<VkFormat> formats(2, copyBufferToImageInfo.RenderDevice->GetTransientAllocationsAllocatorReference());
	formats.EmplaceBack(ImageFormatToVkFormat(copyBufferToImageInfo.SourceImageFormat)); formats.EmplaceBack(VK_FORMAT_R8G8B8A8_UNORM);

	auto originalFormat = ImageFormatToVkFormat(copyBufferToImageInfo.SourceImageFormat);
	auto supportedFormat = static_cast<VulkanRenderDevice*>(copyBufferToImageInfo.RenderDevice)->FindSupportedVkFormat(formats, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, VK_IMAGE_TILING_OPTIMAL);

	const GTSL::uint64 originalTextureSize = copyBufferToImageInfo.Extent.Width * copyBufferToImageInfo.Extent.Height * ImageFormatSize(copyBufferToImageInfo.SourceImageFormat);
	GTSL::uint64 supportedTextureSize = 0;

	if (originalFormat != supportedFormat)
	{
		switch (originalFormat)
		{
		case VK_FORMAT_R8G8B8_UNORM:
			switch (supportedFormat)
			{
			case VK_FORMAT_R8G8B8A8_UNORM:
				supportedTextureSize = (originalTextureSize / 3) * 4; break;
			}
		}
	}
	
	if (originalFormat != supportedFormat)
	{
		switch (originalFormat)
		{
		case VK_FORMAT_R8G8B8_UNORM:
			switch (supportedFormat)
			{
			case VK_FORMAT_R8G8B8A8_UNORM:
	
				for (GTSL::uint32 i = 0, i_n = 0; i < supportedTextureSize; i += 4, i_n += 3)
				{
					memcpy(static_cast<char*>(data) + i, static_cast<char*>(textureCreateInfo.ImageData) + i_n, 3);
					static_cast<char*>(data)[i + 3] = 0;
				}
	
				break;
			}
		}
	}
	else
	{
		supportedTextureSize = originalTextureSize;
		memcpy(data, textureCreateInfo.ImageData, static_cast<size_t>(supportedTextureSize));
	}
	
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
	vkCmdCopyBufferToImage(commandBuffer, static_cast<VulkanBuffer*>(copyImageToBufferInfo.SourceBuffer)->GetVkBuffer(), static_cast<VulkanTexture*>(copyImageToBufferInfo.DestinationImage)->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
