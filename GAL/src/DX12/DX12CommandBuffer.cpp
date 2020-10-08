#include "GAL/DX12/DX12CommandBuffer.h"

#include "GAL/RenderCore.h"
#include "GAL/DX12/DX12RenderDevice.h"

void GAL::DX12CommandBuffer::AddPipelineBarrier(const AddPipelineBarrierInfo& info)
{
	ID3D12GraphicsCommandList* commandList = nullptr;
	commandList->QueryInterface(IID_ID3D12GraphicsCommandList, reinterpret_cast<void**>(commandList));
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
}
