#pragma once

#include "DX12.h"
#include <dxgi1_6.h>

#include "GAL/RenderCore.h"
#include "GTSL/Extent.h"
#include <GAL/Vulkan/VulkanRenderContext.h>

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
			PresentModes PresentMode;
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
			PresentModes PresentMode;
			DX12TextureFormat Format;
			//VulkanColorSpace ColorSpace;
			//VulkanTextureUses::value_type TextureUses;
			const DX12Surface* Surface = nullptr;
			const DX12Queue* Queue = nullptr;
		};
		void Recreate(const RecreateInfo& info);
		
		~DX12RenderContext();

		void Present(const DX12RenderDevice* renderDevice, GTSL::Range<const GTSL::uint32*> waitSemaphores, GTSL::uint32 imageIndex, DX12Queue queue);
	private:
		IDXGISwapChain4* swapChain4 = nullptr; bool vSync = false; bool tear = false;
	};
}
