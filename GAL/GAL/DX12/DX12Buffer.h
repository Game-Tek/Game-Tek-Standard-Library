#pragma once

#include "DX12.h"
#include "DX12Memory.h"

namespace GAL
{
	struct MemoryRequirements;

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

		struct GetMemoryRequirementsInfo final : DX12RenderInfo
		{
			CreateInfo CreateInfo;
			MemoryRequirements* MemoryRequirements;
		};
		void GetMemoryRequirements(const GetMemoryRequirementsInfo& info);

		GTSL::uint64 GetAddress() const { return static_cast<GTSL::uint64>(resource->GetGPUVirtualAddress()); }
		
		void Initialize(const CreateInfo& info);

		[[nodiscard]] ID3D12Resource* GetID3D12Resource() const { return resource; }
		GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(resource); }
		
		void Destroy(const DX12RenderDevice* renderDevice);
		
		~DX12Buffer() = default;
		
	private:
		ID3D12Resource* resource = nullptr;
	};
}
