#pragma once

#include "DX12.h"
#include "DX12Bindings.h"
#include "DX12Buffer.h"
#include "DX12Texture.h"
#include "GTSL/RGB.h"

#undef MemoryBarrier

namespace GAL
{
	class DX12PipelineLayout;
	struct BuildAccelerationStructuresInfo;
	class DX12Pipeline;
	class DX12Queue;
	class DX12RenderPass;
	class DX12Framebuffer;

	class DX12CommandBuffer final
	{
	public:
		DX12CommandBuffer() = default;

		struct CreateInfo : DX12CreateInfo
		{
		};

		[[nodiscard]] ID3D12CommandList* GetID3D12CommandList() const { return commandList; }

		struct BeginRecordingInfo final : DX12RenderInfo
		{
			/**
			 * \brief Pointer to primary/parent command buffer, can be null if this command buffer is primary/has no children.
			 */
			const DX12CommandBuffer* PrimaryCommandBuffer{ nullptr };
		};
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);

		struct EndRecordingInfo final : DX12RenderInfo
		{
		};
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		struct BeginRenderPassInfo final : DX12RenderInfo
		{
			const DX12RenderPass* RenderPass = nullptr;
			const DX12Framebuffer* Framebuffer = nullptr;
			GTSL::Extent2D RenderArea;
			GTSL::Range<const GTSL::RGBA*> ClearValues;
		};
		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo) {}

		struct AdvanceSubpassInfo final : DX12RenderInfo
		{
		};
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo) {}

		struct EndRenderPassInfo final : DX12RenderInfo
		{
		};
		void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo) {}
		
		struct MemoryBarrier
		{
			GTSL::uint32 SourceAccessFlags, DestinationAccessFlags;
		};

		struct BufferBarrier
		{
			DX12Buffer Buffer;
			DX12AccessFlags::value_type SourceAccessFlags, DestinationAccessFlags;
		};

		struct TextureBarrier
		{
			DX12Texture Texture;

			DX12TextureLayout CurrentLayout, TargetLayout;
			DX12AccessFlags::value_type SourceAccessFlags, DestinationAccessFlags;
		};
		
		struct AddPipelineBarrierInfo : DX12CreateInfo
		{
			GTSL::Range<const MemoryBarrier*> MemoryBarriers;
			GTSL::Range<const BufferBarrier*> BufferBarriers;
			GTSL::Range<const TextureBarrier*> TextureBarriers;
			GTSL::uint32 InitialStage, FinalStage;
		};
		void AddPipelineBarrier(const AddPipelineBarrierInfo& info);

		struct SetScissorInfo final : DX12RenderInfo
		{
			GTSL::Extent2D Area;
			GTSL::Extent2D Offset;
		};
		void SetScissor(const SetScissorInfo& info);

		struct BindPipelineInfo final : DX12RenderInfo
		{
			const DX12Pipeline* Pipeline = nullptr;
			DX12PipelineType PipelineType;
		};
		void BindPipeline(const BindPipelineInfo& bindPipelineInfo);

		struct BindIndexBufferInfo final : DX12RenderInfo
		{
			const DX12Buffer* Buffer{ nullptr };
			GTSL::uint32 Size = 0, Offset = 0;
			DX12IndexType IndexType;
		};
		void BindIndexBuffer(const BindIndexBufferInfo& buffer) const;

		struct BindVertexBufferInfo final : DX12RenderInfo
		{
			const DX12Buffer* Buffer{ nullptr };
			GTSL::uint32 Offset = 0;
			GTSL::uint32 Size = 0, Stride = 0;
		};
		void BindVertexBuffer(const BindVertexBufferInfo& buffer) const;

		struct UpdatePushConstantsInfo : DX12RenderInfo
		{
			const DX12PipelineLayout* PipelineLayout = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};
		void UpdatePushConstant(const UpdatePushConstantsInfo& info);

		struct DrawInfo final : DX12RenderInfo
		{
			GTSL::uint32 VertexCount;
			GTSL::uint32 InstanceCount;
			GTSL::uint32 FirstVertex;
			GTSL::uint32 FirstInstance;
		};
		void Draw(const DrawInfo& info) const;

		struct DrawIndexedInfo final : DX12RenderInfo
		{
			GTSL::uint32 InstanceCount = 0, IndexCount = 0;
		};
		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo) const;

		struct TraceRaysInfo : DX12RenderInfo
		{
			struct ShaderTableDescriptor
			{
				DX12Buffer* Buffer = nullptr;
				GTSL::uint32 Size = 0, Offset = 0, Stride = 0;
			} RayGenDescriptor, HitDescriptor, MissDescriptor;

			GTSL::Extent3D DispatchSize;
		};
		void TraceRays(const TraceRaysInfo& traceRaysInfo);

		struct AddLabelInfo : DX12RenderInfo
		{
			GTSL::Range<const GTSL::UTF8*> Name;
		};
		void AddLabel(const AddLabelInfo& info);

		struct BeginRegionInfo : DX12RenderInfo
		{
			GTSL::Range<const GTSL::UTF8*> Name;
		};
		void BeginRegion(const BeginRegionInfo& info) const;

		struct EndRegionInfo : DX12RenderInfo
		{
		};
		void EndRegion(const EndRegionInfo& info) const;

		struct DispatchInfo : DX12RenderInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		void Dispatch(const DispatchInfo& dispatchInfo);

		struct BindBindingsSetInfo : DX12RenderInfo
		{
			DX12PipelineType PipelineType;
			GTSL::Range<const DX12BindingsSet*> BindingsSets;
			GTSL::Range<const GTSL::uint32*> Offsets;
			const DX12PipelineLayout* PipelineLayout = nullptr;
			GTSL::uint32 FirstSet = 0, BoundSets = 0;
		};
		void BindBindingsSets(const BindBindingsSetInfo& info);

		struct CopyTextureToTextureInfo final : DX12RenderInfo
		{
			DX12Texture SourceTexture, DestinationTexture;
			DX12TextureLayout SourceLayout, DestinationLayout;

			GTSL::Extent3D Extent;
		};
		void CopyTextureToTexture(const CopyTextureToTextureInfo& info);

		struct CopyBufferToTextureInfo : DX12RenderInfo
		{
			const DX12Buffer* SourceBuffer{ nullptr };
			DX12TextureLayout TextureLayout;
			const DX12Texture* DestinationTexture{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToTexture(const CopyBufferToTextureInfo& copyBufferToImageInfo);

		struct CopyBuffersInfo final : DX12RenderInfo
		{
			const DX12Buffer* Source = nullptr;
			GTSL::uint32 SourceOffset{ 0 };
			const DX12Buffer* Destination{ nullptr };
			GTSL::uint32 DestinationOffset{ 0 };

			GTSL::uint32 Size{ 0 };
		};
		void CopyBuffers(const CopyBuffersInfo& copyBuffersInfo);

		void BuildAccelerationStructure(const BuildAccelerationStructuresInfo& info) const;
		
		~DX12CommandBuffer() = default;
		
	private:
		ID3D12GraphicsCommandList* commandList = nullptr;
	};

	class DX12CommandPool final
	{
	public:
		DX12CommandPool() = default;

		struct CreateInfo final : DX12CreateInfo
		{
			const DX12Queue* Queue = nullptr;
			bool IsPrimary = true;
		};
		void Initialize(const CreateInfo& info);

		[[nodiscard]] ID3D12CommandAllocator* GetID3D12CommandAllocator() const { return commandAllocator; }

		struct AllocateCommandBuffersInfo final : DX12CreateInfo
		{
			bool IsPrimary = true;
			GTSL::Range<const DX12CommandBuffer::CreateInfo*> CommandBufferCreateInfos;
			GTSL::Range<DX12CommandBuffer*> CommandBuffers;
		};
		void AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo) const;
		
		void ResetPool(DX12RenderDevice* renderDevice) const;
		
		void Destroy(const DX12RenderDevice* renderDevice);

		~DX12CommandPool() = default;
		
	private:
		ID3D12CommandAllocator* commandAllocator = nullptr;
	};
}
