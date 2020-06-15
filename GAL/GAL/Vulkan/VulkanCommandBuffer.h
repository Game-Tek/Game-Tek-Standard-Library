#pragma once

#include "GAL/CommandBuffer.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanRenderDevice;

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		explicit VulkanCommandBuffer(VulkanRenderDevice* renderDevice, const CommandBufferCreateInfo& commandBufferCreateInfo);

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

		struct CopyBufferToImageInfo : RenderInfo
		{
			class Buffer* SourceBuffer{ nullptr };
			ImageFormat SourceImageFormat;
			class Texture* DestinationImage{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToImage(const CopyBufferToImageInfo& copyImageToBufferInfo);

		struct TransitionImageInfo : RenderInfo
		{
			
		};
		void TransitionImage(const TransitionImageInfo& transitionImageInfo);

		[[nodiscard]] VkCommandBuffer GetVkCommandBuffer() const { return commandBuffer; }

	private:
		VkCommandPool commandPool = nullptr;
		VkCommandBuffer commandBuffer = nullptr;

	};
}