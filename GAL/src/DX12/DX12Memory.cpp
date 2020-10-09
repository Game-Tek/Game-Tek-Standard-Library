#include "GAL/DX12/DX12Memory.h"

#include "GAL/RenderCore.h"
#include "GAL/DX12/DX12RenderDevice.h"

void GAL::DX12Memory::Initialize(const CreateInfo& info)
{
	D3D12_HEAP_DESC heapDesc;
	heapDesc.Flags = D3D12_HEAP_FLAG_NONE;
	heapDesc.Alignment = 1024;
	heapDesc.SizeInBytes = info.Size;
	heapDesc.Properties.CreationNodeMask = 0;
	heapDesc.Properties.VisibleNodeMask = 0;
	heapDesc.Properties.Type;
	heapDesc.Properties.CPUPageProperty;
	heapDesc.Properties.MemoryPoolPreference;
	
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreateHeap(&heapDesc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(&heap)))
	setName(heap, info);
}

void GAL::DX12Memory::Destroy(const DX12RenderDevice* renderDevice)
{
	heap->Release();
	debugClear(heap);
}
