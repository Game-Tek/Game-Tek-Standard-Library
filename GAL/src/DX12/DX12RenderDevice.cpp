#include "GAL/DX12/DX12RenderDevice.h"

#include "GAL/RenderCore.h"

#include <dxgi1_6.h>

#include "GAL/DX12/DX12CommandBuffer.h"

void GAL::DX12Queue::Submit(const SubmitInfo& submitInfo) const
{
	commandQueue->ExecuteCommandLists(static_cast<GTSL::uint32>(submitInfo.CommandBuffers.ElementCount()), reinterpret_cast<ID3D12CommandList* const*>(submitInfo.CommandBuffers.begin()));
}

GAL::DX12Queue::~DX12Queue()
{
	commandQueue->Release();
	debugClear(commandQueue);
}

void GAL::DX12RenderDevice::Initialize(const CreateInfo& info)
{
	IDXGIFactory4* factory4; GTSL::uint32 factoryFlags = 0;

	if constexpr (_DEBUG)
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	DX_CHECK(CreateDXGIFactory2(factoryFlags, IID_IDXGIFactory4, reinterpret_cast<void**>(&factory4)))

	IDXGIAdapter1* adapter1 = nullptr;
	IDXGIAdapter4* adapter4 = nullptr;

	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; factory4->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			adapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 && SUCCEEDED(D3D12CreateDevice(adapter1,	D3D_FEATURE_LEVEL_12_1, IID_ID3D12Device2, nullptr)) && dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				DX_CHECK(adapter1->QueryInterface(IID_IDXGIAdapter4, reinterpret_cast<void**>(&adapter4)))
			}
		}

		DX_CHECK(D3D12CreateDevice(adapter4, D3D_FEATURE_LEVEL_12_1, IID_ID3D12Device2, reinterpret_cast<void**>(&device)))
		setName(device, info);
	}

	if constexpr (_DEBUG)
	{
		ID3D12InfoQueue* infoQueue;
		DX_CHECK(device->QueryInterface(IID_ID3D12InfoQueue, reinterpret_cast<void**>(&infoQueue)))

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER infoQueueFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		infoQueueFilter.DenyList.NumSeverities = _countof(severities);
		infoQueueFilter.DenyList.pSeverityList = severities;
		infoQueueFilter.DenyList.NumIDs = _countof(denyIds);
		infoQueueFilter.DenyList.pIDList = denyIds;

		DX_CHECK(infoQueue->PushStorageFilter(&infoQueueFilter))
	}

	for(GTSL::uint32 i = 0; i < info.QueueCreateInfos.ElementCount(); ++i)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = static_cast<D3D12_COMMAND_LIST_TYPE>(info.QueueCreateInfos[i].Capabilities);
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		DX_CHECK(device->CreateCommandQueue(&desc, IID_ID3D12CommandQueue, &info.Queues[i]))
		//setName(info.)
	}
}

GAL::DX12RenderDevice::~DX12RenderDevice()
{
	device->Release();
	debug->Release();
	
	debugClear(device);

	if constexpr (_DEBUG)
	{
		debugClear(debug);
	}
}

void GAL::DX12Queue::Wait(const DX12RenderDevice* renderDevice) const
{
	ID3D12Fence* fence;

	renderDevice->GetID3D12Device2()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, reinterpret_cast<void**>(&fence));

	commandQueue->Wait(fence, 1);

	fence->Release();
}