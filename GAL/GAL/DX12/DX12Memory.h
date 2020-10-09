#pragma once

#include "DX12.h"

namespace GAL
{
	class DX12Memory final
	{
	public:
		DX12Memory() = default;

		struct CreateInfo final : DX12CreateInfo
		{
			DX12AllocateFlags::value_type Flags;
			GTSL::uint32 Size = 0, MemoryType = 0;
		};
		void Initialize(const CreateInfo& info);
		void Destroy(const DX12RenderDevice* renderDevice);
		
		[[nodiscard]] ID3D12Heap* GetID3D12Heap() const { return heap; }
		
		~DX12Memory() = default;
		
	private:
		ID3D12Heap* heap = nullptr;
	};
}
