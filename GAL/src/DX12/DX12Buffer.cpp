#include "GAL/DX12/DX12Buffer.h"

#include "GAL/RenderCore.h"
#include "GAL/DX12/DX12RenderDevice.h"

void GAL::DX12Buffer::Initialize(const CreateInfo& info)
{
	ID3D12Heap* heap = nullptr;
	
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = info.Size;
	resourceDesc.Height = 1;
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreatePlacedResource(heap, 0, &resourceDesc, static_cast<D3D12_RESOURCE_STATES>(info.BufferType), nullptr, IID_PPV_ARGS(&resource)))
}

void GAL::DX12Buffer::Destroy(const DX12RenderDevice* renderDevice)
{
	resource->Release();
	debugClear(resource);
}
