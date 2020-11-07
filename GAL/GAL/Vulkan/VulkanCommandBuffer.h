#pragma once

#include "GAL/CommandBuffer.h"

#include "Vulkan.h"
#include "VulkanTexture.h"
#include "VulkanAccelerationStructures.h"
#include <GTSL/RGB.h>

#undef MemoryBarrier

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

	class VulkanCommandBuffer final
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
			GTSL::Range<const GTSL::RGBA*> ClearValues;
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

		struct SetScissorInfo final : VulkanRenderInfo
		{
			GTSL::Extent2D Area;
			GTSL::Extent2D Offset;
		};
		void SetScissor(const SetScissorInfo& info);
		
		struct BindPipelineInfo final : VulkanRenderInfo
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
			VulkanShaderStage::value_type ShaderStages = 0;
		};
		void UpdatePushConstant(const UpdatePushConstantsInfo& info);

		struct DrawInfo final : VulkanRenderInfo
		{
			uint32_t VertexCount;
			uint32_t InstanceCount;
			uint32_t FirstVertex;
			uint32_t FirstInstance;
		};
		void Draw(const DrawInfo& info) const;
		
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
			} RayGenDescriptor, HitDescriptor, MissDescriptor;
			
			GTSL::Extent3D DispatchSize;
		};
		void TraceRays(const TraceRaysInfo& traceRaysInfo);

		struct AddLabelInfo : VulkanRenderInfo
		{
			GTSL::Range<const GTSL::UTF8*> Name;
		};
		void AddLabel(const AddLabelInfo& info);

		struct BeginRegionInfo : VulkanRenderInfo
		{
			GTSL::Range<const GTSL::UTF8*> Name;
		};
		void BeginRegion(const BeginRegionInfo& info) const;

		struct EndRegionInfo : VulkanRenderInfo
		{
		};
		void EndRegion(const EndRegionInfo& info) const;
		
		struct DispatchInfo : VulkanRenderInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		void Dispatch(const DispatchInfo& dispatchInfo);

		struct BindBindingsSetInfo : VulkanRenderInfo
		{
			VulkanPipelineType PipelineType;
			GTSL::Range<const VulkanBindingsSet*> BindingsSets;
			GTSL::Range<const GTSL::uint32*> Offsets;
			const VulkanPipelineLayout* PipelineLayout = nullptr;
			GTSL::uint32 FirstSet = 0, BoundSets = 0;
		};
		void BindBindingsSets(const BindBindingsSetInfo& info);

		struct CopyTextureToTextureInfo final : VulkanRenderInfo
		{
			VulkanTexture SourceTexture, DestinationTexture;
			VulkanTextureLayout SourceLayout, DestinationLayout;

			GTSL::Extent3D Extent;
		};
		void CopyTextureToTexture(const CopyTextureToTextureInfo& info);

		struct CopyBufferToTextureInfo : VulkanRenderInfo
		{
			const VulkanBuffer* SourceBuffer{ nullptr };
			VulkanTextureLayout TextureLayout;
			const VulkanTexture* DestinationTexture{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToTexture(const CopyBufferToTextureInfo& copyBufferToImageInfo);

		struct MemoryBarrier
		{
			VulkanAccessFlags::value_type SourceAccessFlags, DestinationAccessFlags;
		};

		struct BufferBarrier
		{
		};

		struct TextureBarrier
		{
			VulkanTexture Texture;

			VulkanTextureLayout CurrentLayout, TargetLayout;
			VulkanAccessFlags::value_type SourceAccessFlags, DestinationAccessFlags;
		};

		struct AddPipelineBarrierInfo : VulkanRenderInfo
		{
			GTSL::Range<const MemoryBarrier*> MemoryBarriers;
			GTSL::Range<const BufferBarrier*> BufferBarriers;
			GTSL::Range<const TextureBarrier*> TextureBarriers;
			VulkanPipelineStage::value_type InitialStage, FinalStage;
		};
		void AddPipelineBarrier(const AddPipelineBarrierInfo& pipelineBarrier) const;

		struct CopyBuffersInfo : VulkanRenderInfo
		{
			const VulkanBuffer* Source{ nullptr };
			GTSL::uint32 SourceOffset{ 0 };
			const class VulkanBuffer* Destination{ nullptr };
			GTSL::uint32 DestinationOffset{ 0 };

			GTSL::uint32 Size{ 0 };
		};
		void CopyBuffers(const CopyBuffersInfo& copyBuffersInfo);

		void BuildAccelerationStructure(const BuildAccelerationStructuresInfo& info) const;
		
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
			const VulkanQueue* Queue = nullptr;
			bool IsPrimary = true;
		};
		VulkanCommandPool(const CreateInfo& createInfo);

		void ResetPool(RenderDevice* renderDevice) const;

		struct AllocateCommandBuffersInfo final : VulkanRenderInfo
		{
			bool IsPrimary = true;
			GTSL::Range<const VulkanCommandBuffer::CreateInfo*> CommandBufferCreateInfos;
			GTSL::Range<VulkanCommandBuffer*> CommandBuffers;
		};
		void AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo) const;
		
		struct FreeCommandBuffersInfo final : VulkanRenderInfo
		{
			GTSL::Range<VulkanCommandBuffer*> CommandBuffers;
		};
		void FreeCommandBuffers(const FreeCommandBuffersInfo& freeCommandBuffers) const;
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkCommandPool GetVkCommandPool() const { return commandPool; }
		
	private:
		VkCommandPool commandPool = nullptr;
	};
}
