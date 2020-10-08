#pragma once

#include "DX12.h"
#include "DX12Texture.h"

#undef MemoryBarrier

namespace GAL
{
	class DX12Queue;

	class DX12CommandBuffer final
	{
	public:
		DX12CommandBuffer() = default;

		struct CreateInfo : DX12CreateInfo
		{
		};

		[[nodiscard]] ID3D12CommandList* GetID3D12CommandList() const { return commandList; }

		struct MemoryBarrier
		{
			GTSL::uint32 SourceAccessFlags, DestinationAccessFlags;
		};

		struct BufferBarrier
		{
		};

		struct TextureBarrier
		{
			DX12Texture Texture;

			GTSL::uint32 CurrentLayout, TargetLayout;
			GTSL::uint32 SourceAccessFlags, DestinationAccessFlags;
		};
		
		struct AddPipelineBarrierInfo : DX12CreateInfo
		{
			GTSL::Range<const MemoryBarrier*> MemoryBarriers;
			GTSL::Range<const BufferBarrier*> BufferBarriers;
			GTSL::Range<const TextureBarrier*> TextureBarriers;
			GTSL::uint32 InitialStage, FinalStage;
		};
		void AddPipelineBarrier(const AddPipelineBarrierInfo& info);
		
		~DX12CommandBuffer() = default;
		
	private:
		ID3D12CommandList* commandList = nullptr;
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
