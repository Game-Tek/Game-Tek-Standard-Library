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
		explicit VulkanCommandBuffer(const CreateInfo& commandBufferCreateInfo);

		void Destroy(RenderDevice* renderDevice);
		
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo);
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo);
		void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo);

		void BindGraphicsPipeline(const BindGraphicsPipelineInfo& bindGraphicsPipelineInfo);
		void BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo);

		void BindMesh(const BindMeshInfo& bindMeshInfo);

		void UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo);

		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo);
		void Dispatch(const DispatchInfo& dispatchInfo);

		void BindBindingsSet(const BindBindingsSetInfo& bindBindingsSetInfo);

		void CopyImage(const CopyImageInfo& copyImageInfo);

		void CopyBufferToImage(const CopyBufferToImageInfo& copyImageToBufferInfo);

		void TransitionImage(const TransitionImageInfo& transitionImageInfo);

		[[nodiscard]] VkCommandBuffer GetVkCommandBuffer() const { return commandBuffer; }

	private:
		VkCommandPool commandPool = nullptr;
		VkCommandBuffer commandBuffer = nullptr;

	};
}