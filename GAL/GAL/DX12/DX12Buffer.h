#pragma once

#include "DX12.h"
#include "DX12Memory.h"

namespace GAL
{
	class DX12Buffer final
	{
	public:
		DX12Buffer() = default;

		struct CreateInfo final : DX12CreateInfo
		{
			GTSL::uint32 Size = 0, Offset = 0;
			DX12BufferType::value_type BufferType = 0;
			const DX12Memory Memory;
		};
		void Initialize(const CreateInfo& info);

		void Destroy(const DX12RenderDevice* renderDevice);

		~DX12Buffer() = default;
		
	private:
		ID3D12Resource* resource = nullptr;
	};
}
