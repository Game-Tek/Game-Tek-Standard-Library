#include "GAL/DX12/DX12Synchronization.h"

#include "GAL/RenderCore.h"

#include "GAL/DX12/DX12RenderDevice.h"

void GAL::DX12Fence::Initialize(const CreateInfo& info)
{
	info.RenderDevice->GetID3D12Device2()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, reinterpret_cast<void**>(&fence));
	setName(fence, info);
}

void GAL::DX12Fence::Wait(const DX12RenderDevice* renderDevice) const
{
	const auto event = CreateEventA(nullptr, false, false, nullptr);
	fence->SetEventOnCompletion(1, event);

	WaitForSingleObject(event, 0xFFFFFFFF);
}

GAL::DX12Fence::~DX12Fence()
{
	fence->Release();
	debugClear(fence);
}
