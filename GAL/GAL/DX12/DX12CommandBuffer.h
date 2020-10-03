#pragma once

#include "DX12.h"

namespace GAL
{
	class DX12CommandBuffer final
	{
	public:
		DX12CommandBuffer() = default;

		void Initialize();

		[[nodiscard]] ID3D12CommandList* GetID3D12CommandList() const { return commandList; }
		
		~DX12CommandBuffer();
		
	private:
		ID3D12CommandList* commandList = nullptr;
	};
}
