#include "GAL/DX12/DX12Texture.h"

#include "GAL/DX12/DX12RenderDevice.h"
#include "GTSL/Bitman.h"

void GAL::DX12Texture::Initialize(const CreateInfo& info)
{
	ID3D12Heap* heap = nullptr;

	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Width = info.Extent.Width;
	resourceDesc.Height = info.Extent.Height;
	resourceDesc.DepthOrArraySize = info.Extent.Depth;
	resourceDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(info.Dimensions);
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Format = static_cast<DXGI_FORMAT>(info.Format);

	//D3D12_RESOURCE_FLAG_NONE = 0,
	//D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET = 0x1,
	//D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL = 0x2,
	//D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS = 0x4,
	//D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE = 0x8,
	//D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER = 0x10,
	//D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS = 0x20,
	//D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY = 0x40
	
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), info.Uses & TextureUses::COLOR_ATTACHMENT, reinterpret_cast<GTSL::uint32&>(resourceDesc.Flags));
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), info.Uses & TextureUses::DEPTH_STENCIL_ATTACHMENT, reinterpret_cast<GTSL::uint32&>(resourceDesc.Flags));
	
	GTSL::uint32 states = 0;
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_STATE_RENDER_TARGET), info.Uses & TextureUses::COLOR_ATTACHMENT, states);
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_STATE_RENDER_TARGET), info.Uses & TextureUses::DEPTH_STENCIL_ATTACHMENT, states);
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_STATE_DEPTH_WRITE), info.Uses & TextureUses::DEPTH_STENCIL_ATTACHMENT, states);
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_STATE_DEPTH_READ), info.Uses & TextureUses::DEPTH_STENCIL_ATTACHMENT, states);
	GTSL::SetBitAs(GTSL::FindFirstSetBit(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE), info.Uses & TextureUses::SAMPLE, states);
	
	resourceDesc.SampleDesc;
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreatePlacedResource(heap, 0, &resourceDesc, static_cast<D3D12_RESOURCE_STATES>(states), nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resource)))
}
