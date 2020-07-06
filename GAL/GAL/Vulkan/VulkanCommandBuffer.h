#pragma once

#include "GAL/CommandBuffer.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanRenderDevice;

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;

		explicit VulkanCommandBuffer(const VkCommandBuffer commandBuffer) : commandBuffer(commandBuffer) {}
		
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo);
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo);
		void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo);

		void BindGraphicsPipeline(const BindGraphicsPipelineInfo& bindGraphicsPipelineInfo);
		void BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo);

		void BindIndexBuffer(const BindIndexBufferInfo& buffer);
		void BindVertexBuffer(const BindVertexBufferInfo& buffer);
		
		void UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo);

		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo);
		void Dispatch(const DispatchInfo& dispatchInfo);

		void BindBindingsSet(const BindBindingsSetInfo& bindBindingsSetInfo);

		void CopyImage(const CopyImageInfo& copyImageInfo);

		void CopyBufferToImage(const CopyBufferToImageInfo& copyImageToBufferInfo);

		void TransitionImage(const TransitionImageInfo& transitionImageInfo);

		void CopyBuffers(const CopyBuffersInfo& copyBuffersInfo);
		
		[[nodiscard]] VkCommandBuffer GetVkCommandBuffer() const { return commandBuffer; }

	private:
		VkCommandBuffer commandBuffer = nullptr;

	};

	class VulkanCommandPool final : public CommandPool
	{
	public:
		VulkanCommandPool() = default;
		VulkanCommandPool(const CreateInfo& createInfo);

		void ResetPool(RenderDevice* renderDevice) const;
		
		struct FreeCommandBuffers final : RenderInfo
		{
			GTSL::Ranger<CommandBuffer> CommandBuffers;
		};
		void FreeCommandBuffers(const FreeCommandBuffers& freeCommandBuffers) const;
		
		void Destroy(RenderDevice* renderDevice);

		[[nodiscard]] VkCommandPool GetVkCommandPool() const { return commandPool; }
		
	private:
		VkCommandPool commandPool = nullptr;
	};
}