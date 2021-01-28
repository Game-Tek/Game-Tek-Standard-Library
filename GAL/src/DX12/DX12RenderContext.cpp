#include "GAL/DX12/DX12RenderContext.h"


#include "GAL/RenderCore.h"
#include "GAL/DX12/DX12RenderDevice.h"

void GAL::DX12Surface::Initialize(const CreateInfo& info)
{
	handle = info.SystemData->WindowHandle;
}

void GAL::DX12RenderContext::Initialize(const CreateInfo& info)
{
	IDXGIFactory4* factory4; GTSL::uint32 factoryFlags = 0;

	if constexpr (_DEBUG)
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	DX_CHECK(CreateDXGIFactory2(factoryFlags, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&factory4)))

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = info.SurfaceArea.Width;
	swapChainDesc.Height = info.SurfaceArea.Height;
	swapChainDesc.Format = static_cast<DXGI_FORMAT>(info.Format);
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = info.DesiredFramesInFlight;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = static_cast<GTSL::uint32>(info.PresentMode);

	IDXGISwapChain1* swapChain1;

	DX_CHECK(factory4->CreateSwapChainForHwnd(info.Queue->GetID3D12CommandQueue(), static_cast<HWND>(info.Surface->GetHWND()), &swapChainDesc, nullptr, nullptr, &swapChain1))

	DX_CHECK(factory4->MakeWindowAssociation(static_cast<HWND>(info.Surface->GetHWND()), DXGI_MWA_NO_ALT_ENTER))

	DX_CHECK(swapChain1->QueryInterface(IID_IDXGISwapChain4, reinterpret_cast<void**>(&swapChain4)))

	swapChain1->Release();

	factory4->Release();
}

void GAL::DX12RenderContext::Recreate(const RecreateInfo& info)
{
	IDXGIFactory4* factory4; GTSL::uint32 factoryFlags = 0;

	if constexpr (_DEBUG)
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	DX_CHECK(CreateDXGIFactory2(factoryFlags, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(&factory4)))

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = info.SurfaceArea.Width;
	swapChainDesc.Height = info.SurfaceArea.Height;
	swapChainDesc.Format = static_cast<DXGI_FORMAT>(info.Format);
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = info.DesiredFramesInFlight;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = static_cast<GTSL::uint32>(info.PresentMode);

	IDXGISwapChain1* swapChain1;

	DX_CHECK(factory4->CreateSwapChainForHwnd(info.Queue->GetID3D12CommandQueue(), static_cast<HWND>(info.Surface->GetHWND()), &swapChainDesc, nullptr, nullptr, &swapChain1))

	DX_CHECK(factory4->MakeWindowAssociation(static_cast<HWND>(info.Surface->GetHWND()), DXGI_MWA_NO_ALT_ENTER))

	DX_CHECK(swapChain1->QueryInterface(IID_IDXGISwapChain4, reinterpret_cast<void**>(&swapChain4)))

	swapChain1->Release();

	factory4->Release();
}

GAL::DX12RenderContext::~DX12RenderContext()
{
	swapChain4->Release();
	debugClear(swapChain4);
}
