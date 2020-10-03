#pragma once

#include "DX12.h"
#include <dxgi1_6.h>

#include "GAL/RenderContext.h"
#include "GTSL/Extent.h"

namespace GAL
{
	class DX12Queue;

	class DX12Surface
	{
	public:
		struct CreateInfo final : DX12CreateInfo
		{
			WindowsWindowData* SystemData;
		};
		void Initialize(const CreateInfo& info);

		[[nodiscard]] void* GetHWND() const { return handle; }
	private:
		void* handle = nullptr;
	};

	class DX12RenderContext
	{
	public:
		DX12RenderContext() = default;

		struct CreateInfo : DX12CreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			DX12PresentMode PresentMode;
			DX12TextureFormat Format;
			//VulkanColorSpace ColorSpace;
			//VulkanTextureUses::value_type TextureUses;
			const DX12Surface* Surface = nullptr;
			const DX12Queue* Queue = nullptr;
		};
		void Initialize(const CreateInfo& info);

		struct RecreateInfo final : DX12CreateInfo
		{
			GTSL::Extent2D SurfaceArea;
			GTSL::uint8 DesiredFramesInFlight = 0;
			DX12PresentMode PresentMode;
			DX12TextureFormat Format;
			//VulkanColorSpace ColorSpace;
			//VulkanTextureUses::value_type TextureUses;
			const DX12Surface* Surface = nullptr;
			const DX12Queue* Queue = nullptr;
		};
		void Recreate(const RecreateInfo& info);
		
		~DX12RenderContext();
		
	private:
		IDXGISwapChain4* swapChain4 = nullptr;
	};
}
