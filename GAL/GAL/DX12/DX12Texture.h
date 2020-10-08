#pragma once

#include "DX12.h"
#include <GTSL/Extent.h>

#include "GAL/RenderCore.h"

namespace GAL
{
	class DX12Texture final
	{
	public:
		DX12Texture() = default;

		struct CreateInfo final : DX12CreateInfo
		{
			GTSL::uint32 InitialLayout;
			TextureUses::value_type Uses;
			DX12TextureFormat Format;
			GTSL::Extent3D Extent{ 1280, 720, 1 };
			GTSL::uint32 Tiling;
			DX12Dimension Dimensions;
			GTSL::uint8 MipLevels = 1;
		};
		void Initialize(const CreateInfo& info);
		
		void Destroy(const DX12RenderDevice* renderDevice);
		
		~DX12Texture() = default;
		
	private:
		ID3D12Resource* resource = nullptr;
	};
}
