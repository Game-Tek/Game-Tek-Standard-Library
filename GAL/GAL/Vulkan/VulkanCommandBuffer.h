#pragma once

#include "GAL/CommandBuffer.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanFramebuffer;
	class VulkanRenderPass;
	class VulkanPipelineLayout;
	class VulkanQueue;
	class VulkanTexture;
	class VulkanPipeline;
	class VulkanBindingsSet;
	class VulkanRenderDevice;
	class VulkanBuffer;

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		VulkanCommandBuffer() = default;

		struct CreateInfo : VulkanCreateInfo
		{
		};
		
		explicit VulkanCommandBuffer(const VkCommandBuffer commandBuffer) : commandBuffer(commandBuffer) {}

		struct BeginRecordingInfo final : VulkanRenderInfo
		{
			/**
			 * \brief Pointer to primary/parent command buffer, can be null if this command buffer is primary/has no children.
			 */
			const CommandBuffer* PrimaryCommandBuffer{ nullptr };
		};
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);

		struct EndRecordingInfo final : VulkanRenderInfo
		{
		};
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		struct BeginRenderPassInfo final : VulkanRenderInfo
		{
			const VulkanRenderPass* RenderPass = nullptr;
			const VulkanFramebuffer* Framebuffer = nullptr;
			GTSL::Extent2D RenderArea;
			GTSL::Ranger<const GTSL::RGBA> ClearValues;
		};
		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo);

		struct AdvanceSubpassInfo final : VulkanRenderInfo
		{
		};
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo);

		struct EndRenderPassInfo final : VulkanRenderInfo
		{
		};
		void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo);

		struct BindPipelineInfo : VulkanRenderInfo
		{
			const VulkanPipeline* Pipeline = nullptr;
			VulkanPipelineType PipelineType;
		};
		void BindPipeline(const BindPipelineInfo& bindPipelineInfo);

		struct BindIndexBufferInfo final : VulkanRenderInfo
		{
			const VulkanBuffer* Buffer{ nullptr };
			GTSL::uint32 Offset{ 0 };
			VulkanIndexType IndexType;
		};
		void BindIndexBuffer(const BindIndexBufferInfo& buffer) const;

		struct BindVertexBufferInfo final : VulkanRenderInfo
		{
			const VulkanBuffer* Buffer{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		void BindVertexBuffer(const BindVertexBufferInfo& buffer) const;

		struct UpdatePushConstantsInfo : VulkanRenderInfo
		{
			const VulkanPipelineLayout* PipelineLayout = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};
		void UpdatePushConstant(const UpdatePushConstantsInfo& info);

		struct DrawIndexedInfo final : VulkanRenderInfo
		{
			GTSL::uint32 InstanceCount = 0, IndexCount = 0;
		};
		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo) const;

		struct TraceRaysInfo : VulkanRenderInfo
		{
			struct ShaderTableDescriptor
			{
				VulkanBuffer* Buffer = nullptr;
				GTSL::uint32 Size = 0, Offset = 0, Stride = 0;
			} RaygenDescriptor, HitDescriptor, MissDescriptor;
			
			GTSL::Extent3D DispatchSize;
		};
		void TraceRays(const TraceRaysInfo& traceRaysInfo);

		struct DispatchInfo : VulkanRenderInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		void Dispatch(const DispatchInfo& dispatchInfo);

		struct BindBindingsSetInfo : VulkanRenderInfo
		{
			VulkanPipelineType PipelineType;
			GTSL::Ranger<const VulkanBindingsSet> BindingsSets;
			GTSL::Ranger<const GTSL::uint32> Offsets;
			const VulkanPipelineLayout* PipelineLayout = nullptr;
			GTSL::uint32 FirstSet = 0, BoundSets = 0;
		};
		void BindBindingsSets(const BindBindingsSetInfo& info);

		struct CopyImageInfo final : VulkanRenderInfo
		{};
		void CopyImage(const CopyImageInfo& copyImageInfo);

		struct CopyBufferToImageInfo : VulkanRenderInfo
		{
			const VulkanBuffer* SourceBuffer{ nullptr };
			VulkanTextureLayout TextureLayout;
			const VulkanTexture* DestinationImage{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToImage(const CopyBufferToImageInfo& copyBufferToImageInfo);

		struct TransitionImageInfo : VulkanRenderInfo
		{
			const VulkanTexture* Texture{ nullptr };
			VulkanTextureLayout SourceLayout, DestinationLayout;
			GTSL::uint32 SourceStage, DestinationStage;
			AccessFlags SourceAccessFlags, DestinationAccessFlags;
		};
		void TransitionImage(const TransitionImageInfo& transitionImageInfo);

		struct CopyBuffersInfo : VulkanRenderInfo
		{
			const VulkanBuffer* Source{ nullptr };
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
			const VulkanQueue* Queue{ nullptr };
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