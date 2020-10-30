#include "GAL/DX12/DX12CommandBuffer.h"

#include "GAL/RenderCore.h"
#include "GAL/DX12/DX12Pipeline.h"
#include "GAL/DX12/DX12RenderDevice.h"
#include "GTSL/Array.hpp"

void GAL::DX12CommandBuffer::BeginRecording(const BeginRecordingInfo& beginRecordingInfo)
{
}

void GAL::DX12CommandBuffer::EndRecording(const EndRecordingInfo& endRecordingInfo)
{
	commandList->Close();
}

void GAL::DX12CommandBuffer::AddPipelineBarrier(const AddPipelineBarrierInfo& info)
{
	GTSL::Array<D3D12_RESOURCE_BARRIER, 16> resourceBarriers;

	for(const TextureBarrier& textureBarrier : info.TextureBarriers)
	{
		resourceBarriers.EmplaceBack();
		auto& resourceBarrier = resourceBarriers.back();
		
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

		resourceBarrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(textureBarrier.CurrentLayout);
		resourceBarrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(textureBarrier.TargetLayout);
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.pResource = textureBarrier.Texture.GetID3D12Resource();
	}

	for(const BufferBarrier& bufferBarrier : info.BufferBarriers)
	{
		resourceBarriers.EmplaceBack();
		auto& resourceBarrier = resourceBarriers.back();
		
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

		resourceBarrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(bufferBarrier.SourceAccessFlags);
		resourceBarrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(bufferBarrier.DestinationAccessFlags);
		resourceBarrier.Transition.Subresource = 0;
		resourceBarrier.Transition.pResource = bufferBarrier.Buffer.GetID3D12Resource();
	}
	
	commandList->ResourceBarrier(resourceBarriers.GetLength(), resourceBarriers.begin());
}

void GAL::DX12CommandBuffer::SetScissor(const SetScissorInfo& info)
{
	D3D12_RECT rect;
	rect.top;
	rect.right;
	rect.bottom;
	rect.left;
	commandList->RSSetScissorRects(1, &rect);
}

void GAL::DX12CommandBuffer::BindPipeline(const BindPipelineInfo& bindPipelineInfo)
{
	commandList->SetPipelineState(static_cast<const DX12RasterPipeline*>(bindPipelineInfo.Pipeline)->GetID3D12PipelineState());
}

void GAL::DX12CommandBuffer::BindIndexBuffer(const BindIndexBufferInfo& buffer) const
{	
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.Format = static_cast<DXGI_FORMAT>(buffer.IndexType);
	indexBufferView.BufferLocation = buffer.Buffer->GetID3D12Resource()->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = buffer.Size;
	commandList->IASetIndexBuffer(&indexBufferView);
}

void GAL::DX12CommandBuffer::BindVertexBuffer(const BindVertexBufferInfo& buffer) const
{
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	vertexBufferView.SizeInBytes = buffer.Size;
	vertexBufferView.BufferLocation = buffer.Buffer->GetID3D12Resource()->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = buffer.Stride;
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
}

void GAL::DX12CommandBuffer::DrawIndexed(const DrawIndexedInfo& drawIndexedInfo) const
{
	commandList->DrawIndexedInstanced(drawIndexedInfo.IndexCount, drawIndexedInfo.InstanceCount, 0, 0, 0);
}

void GAL::DX12CommandBuffer::CopyTextureToTexture(const CopyTextureToTextureInfo& info)
{
	D3D12_TEXTURE_COPY_LOCATION sourceTextureCopyLocation, destinationTextureCopyLocation;
	sourceTextureCopyLocation.Type;
	sourceTextureCopyLocation.pResource = info.SourceTexture.GetID3D12Resource();
	sourceTextureCopyLocation.PlacedFootprint;
	sourceTextureCopyLocation.SubresourceIndex;

	destinationTextureCopyLocation.pResource = info.DestinationTexture.GetID3D12Resource();

	D3D12_BOX box;
	box.back;
	box.bottom;
	box.front;
	box.left;
	box.right;
	box.top;
	
	commandList->CopyTextureRegion(&destinationTextureCopyLocation, 0, 0, 0, &sourceTextureCopyLocation, &box);
}

void GAL::DX12CommandBuffer::CopyBufferToTexture(const CopyBufferToTextureInfo& copyBufferToImageInfo)
{
	commandList->CopyResource(copyBufferToImageInfo.DestinationTexture->GetID3D12Resource(), copyBufferToImageInfo.SourceBuffer->GetID3D12Resource());
}

void GAL::DX12CommandBuffer::CopyBuffers(const CopyBuffersInfo& copyBuffersInfo)
{
	commandList->CopyBufferRegion(copyBuffersInfo.Destination->GetID3D12Resource(), copyBuffersInfo.DestinationOffset, copyBuffersInfo.Source->GetID3D12Resource(),
		copyBuffersInfo.SourceOffset, copyBuffersInfo.Size);
}

void GAL::DX12CommandPool::Initialize(const CreateInfo& info)
{
	const D3D12_COMMAND_LIST_TYPE type = info.IsPrimary ? D3D12_COMMAND_LIST_TYPE_DIRECT : D3D12_COMMAND_LIST_TYPE_BUNDLE;
	
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreateCommandAllocator(type, IID_ID3D12CommandAllocator, reinterpret_cast<void**>(commandAllocator)))

	setName(commandAllocator, info);
}

void GAL::DX12CommandPool::AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo) const
{
	auto* device = allocateCommandBuffersInfo.RenderDevice->GetID3D12Device2();
	
	for(GTSL::uint32 i = 0; i < allocateCommandBuffersInfo.CommandBufferCreateInfos.ElementCount(); ++i)
	{
		const D3D12_COMMAND_LIST_TYPE type = allocateCommandBuffersInfo.IsPrimary ? D3D12_COMMAND_LIST_TYPE_DIRECT : D3D12_COMMAND_LIST_TYPE_BUNDLE;
		DX_CHECK(device->CreateCommandList(0, type, commandAllocator, nullptr, IID_ID3D12CommandList, reinterpret_cast<void**>(&allocateCommandBuffersInfo.CommandBuffers[i])))

		setName(allocateCommandBuffersInfo.CommandBuffers[i].GetID3D12CommandList(), allocateCommandBuffersInfo.CommandBufferCreateInfos[i]);
	}
}

void GAL::DX12CommandPool::ResetPool(DX12RenderDevice* renderDevice) const
{
	DX_CHECK(commandAllocator->Reset())
}

void GAL::DX12CommandPool::Destroy(const DX12RenderDevice* renderDevice)
{
	//commandList->Release();
	//debugClear(commandList);
	
	commandAllocator->Release();
	debugClear(commandAllocator);
}
