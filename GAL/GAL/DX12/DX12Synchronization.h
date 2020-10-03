#pragma once

#include "DX12.h"

namespace GAL
{
	class DX12Fence
	{
	public:
		DX12Fence() = default;
		
		struct CreateInfo : DX12CreateInfo
		{
			bool IsSignaled{ true };
		};
		void Initialize(const CreateInfo& info);

		void Wait(const DX12RenderDevice* renderDevice) const;
		
		~DX12Fence();
		
	private:
		ID3D12Fence* fence = nullptr;
	};

	//class DX12Semaphore
	//{
	//public:
	//
	//private:
	//	ID3D12Semaphore* semaphore = nullptr;
	//};
}
