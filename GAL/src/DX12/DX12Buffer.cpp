#include "GAL/DX12/DX12Buffer.h"

#include "GAL/RenderCore.h"
#include "GAL/DX12/DX12RenderDevice.h"

void GAL::DX12Buffer::Initialize(const CreateInfo& info)
{	
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = info.Size;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreatePlacedResource(info.Memory.GetID3D12Heap(), info.Offset, &resourceDesc, static_cast<D3D12_RESOURCE_STATES>(info.BufferType), nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resource)))
}

void GAL::DX12Buffer::Destroy(const DX12RenderDevice* renderDevice)
{
	resource->Release();
	debugClear(resource);
}

void GAL::DX12Buffer::GetMemoryRequirements(const GetMemoryRequirementsInfo& info)
{
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = info.CreateInfo.Size;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;

	const auto allocInfo = info.RenderDevice->GetID3D12Device2()->GetResourceAllocationInfo(0, 1, &resourceDesc);

	info.MemoryRequirements->Alignment = static_cast<GTSL::uint32>(allocInfo.Alignment);
	info.MemoryRequirements->Size = static_cast<GTSL::uint32>(allocInfo.SizeInBytes);
	info.MemoryRequirements->MemoryTypes = 0;
}
