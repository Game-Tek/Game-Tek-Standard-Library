#pragma once

#include <GTSL/Core.h>

#include <GTSL/String.hpp>

#include "RenderCore.h"

#include "Framebuffer.h"
#include "CommandBuffer.h"
#include <GTSL/Array.hpp>

namespace GAL
{
	enum class RenderAPI : GTSL::uint8
	{
		VULKAN,
		DIRECTX12
	};

	struct GPUInfo
	{
		GTSL::String GPUName;
		GTSL::uint32 DriverVersion;
		GTSL::uint32 APIVersion;
	};

	class Queue
	{
	public:
		enum class QueueCapabilities : GTSL::uint8
		{
			GRAPHICS = 1, COMPUTE = 2, TRANSFER = 4
		};

		struct CreateInfo : RenderInfo
		{
			QueueCapabilities Capabilities;
			float QueuePriority = 1.0f;
		};

		struct DispatchInfo : RenderInfo
		{
			class CommandBuffer* CommandBuffer = nullptr;
		};
		void Dispatch(const DispatchInfo& dispatchInfo);

	private:
		QueueCapabilities capabilities;

		friend RenderDevice;

	};

	class RenderDevice
	{
	protected:
		RenderDevice() = default;

		virtual ~RenderDevice() = default;

		GTSL::AllocatorReference* persistentAllocatorReference{ nullptr };
		GTSL::AllocatorReference* transientAllocatorReference{ nullptr };
		
	public:
		static void GetAvailableRenderAPIs(GTSL::Array<RenderAPI, 16>& renderApis);

		struct RenderDeviceCreateInfo
		{
			RenderAPI RenderingAPI;
			GTSL::Ranger<GTSL::UTF8> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Ranger<Queue::CreateInfo> QueueCreateInfos;
			GTSL::Ranger<Queue> Queues;
		};
		static RenderDevice* CreateRenderDevice(const RenderDeviceCreateInfo& renderDeviceCreateInfo);
		static void DestroyRenderDevice(const RenderDevice* renderDevice);
		
		[[nodiscard]] GTSL::AllocatorReference* GetPersistentAllocationsAllocatorReference() const { return persistentAllocatorReference; }
		[[nodiscard]] GTSL::AllocatorReference* GetTransientAllocationsAllocatorReference() const { return transientAllocatorReference; }
	};
}
