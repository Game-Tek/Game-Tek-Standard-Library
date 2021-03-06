#pragma once

#include "DX12.h"
#include <GTSL/Extent.h>


#include "DX12Memory.h"
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
			GTSL::uint32 Offset = 0;
			DX12Memory Memory;
		};
		void Initialize(const CreateInfo& info);
		
		void Destroy(const DX12RenderDevice* renderDevice);

		struct GetMemoryRequirementsInfo final : DX12RenderInfo
		{
			CreateInfo CreateInfo;
			MemoryRequirements* MemoryRequirements;
		};
		void GetMemoryRequirements(const GetMemoryRequirementsInfo& info);

		ID3D12Resource* GetID3D12Resource() const { return resource; }
		
		~DX12Texture() = default;
		
	private:
		ID3D12Resource* resource = nullptr;
	};

	class DX12TextureView final
	{
	public:
		DX12TextureView() = default;

	private:
	};

	class DX12Sampler final
	{
	public:
		DX12Sampler() = default;

		struct CreateInfo final : DX12CreateInfo
		{
			GTSL::uint8 Anisotropy = 0;
		};
		void Initialize(const CreateInfo& info);
		
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE sampler;
	};
}
