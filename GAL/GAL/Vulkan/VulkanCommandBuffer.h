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

		struct CreateInfo : VulkanCreateInfo
		{
		};
		
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

		struct BindIndexBufferInfo final : VulkanRenderInfo
		{
			const class VulkanBuffer* Buffer{ nullptr };
			GTSL::uint32 Offset{ 0 };
			VulkanIndexType IndexType;
		};
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

		struct TraceRaysInfo : VulkanRenderInfo
		{
			
		};
		void TraceRays(const TraceRaysInfo& traceRaysInfo);
		
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

		struct CopyBufferToImageInfo : VulkanRenderInfo
		{
			const class VulkanBuffer* SourceBuffer{ nullptr };
			ImageFormat SourceImageFormat;
			VulkanImageLayout ImageLayout;
			const class VulkanImage* DestinationImage{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToImage(const CopyBufferToImageInfo& copyImageToBufferInfo);

		struct TransitionImageInfo : RenderInfo
		{
			const VulkanImage* Texture{ nullptr };
			VulkanImageLayout SourceLayout, DestinationLayout;
			GTSL::uint32 SourceStage, DestinationStage;
			AccessFlags SourceAccessFlags, DestinationAccessFlags;
		};
		void TransitionImage(const TransitionImageInfo& transitionImageInfo);

		struct CopyBuffersInfo : VulkanRenderInfo
		{
			const class VulkanBuffer* Source{ nullptr };
			GTSL::uint32 SourceOffset{ 0 };
			const class VulkanBuffer* Destination{ nullptr };
			GTSL::uint32 DestinationOffset{ 0 };

			GTSL::uint32 Size{ 0 };
		};
		void CopyBuffers(const CopyBuffersInfo& copyBuffersInfo);
		
		[[nodiscard]] VkCommandBuffer GetVkCommandBuffer() const { return commandBuffer; }

	private:
		VkCommandBuffer commandBuffer = nullptr;
		friend class VulkanCommandPool;
	};

	class VulkanCommandPool final : public CommandPool
	{
	public:
		VulkanCommandPool() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			const class VulkanQueue* Queue{ nullptr };
		};
		VulkanCommandPool(const CreateInfo& createInfo);

		void ResetPool(RenderDevice* renderDevice) const;

		struct AllocateCommandBuffersInfo final : VulkanRenderInfo
		{
			bool IsPrimary = true;
			GTSL::Ranger<const VulkanCommandBuffer::CreateInfo> CommandBufferCreateInfos;
			GTSL::Ranger<VulkanCommandBuffer> CommandBuffers;
		};
		void AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo);
		
		struct FreeCommandBuffersInfo final : VulkanRenderInfo
		{
			GTSL::Ranger<VulkanCommandBuffer> CommandBuffers;
		};
		void FreeCommandBuffers(const FreeCommandBuffersInfo& freeCommandBuffers) const;
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkCommandPool GetVkCommandPool() const { return commandPool; }
		
	private:
		VkCommandPool commandPool = nullptr;
	};
}