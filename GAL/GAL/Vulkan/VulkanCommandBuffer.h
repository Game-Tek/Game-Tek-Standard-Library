#pragma once

#include "GAL/CommandBuffer.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanPipeline;
	class VulkanBindingsSet;
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

		struct BindPipelineInfo : VulkanRenderInfo
		{
			const VulkanPipeline* Pipeline = nullptr;
			VulkanPipelineType PipelineType;
		};
		void BindPipeline(const BindPipelineInfo& bindPipelineInfo);

		void BindIndexBuffer(const BindIndexBufferInfo& buffer);
		void BindVertexBuffer(const BindVertexBufferInfo& buffer);

		struct UpdatePushConstantsInfo : VulkanRenderInfo
		{
			const VulkanPipeline* Pipeline = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};
		void UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo);

		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo);
		void Dispatch(const DispatchInfo& dispatchInfo);

		struct BindBindingsSetInfo : VulkanRenderInfo
		{
			VulkanPipelineType PipelineType;
			GTSL::Ranger<const VulkanBindingsSet> BindingsSets;
			GTSL::Ranger<const GTSL::uint32> Offsets;
			const VulkanPipeline* Pipeline = nullptr;
		};
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
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkCommandPool GetVkCommandPool() const { return commandPool; }
		
	private:
		VkCommandPool commandPool = nullptr;
	};
}