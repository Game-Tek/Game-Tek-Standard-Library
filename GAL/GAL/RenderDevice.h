#pragma once

#include <GTSL/Core.h>

#include "RenderCore.h"

#include "Framebuffer.h"
#include "CommandBuffer.h"

#include "GTSL/StaticString.hpp"

namespace GAL
{
	enum class RenderAPI : GTSL::uint8
	{
		VULKAN,
		DIRECTX12
	};

	struct GPUInfo
	{
		GTSL::StaticString<512> GPUName;
		GTSL::uint32 DriverVersion;
		GTSL::uint32 APIVersion;
	};

	class Queue
	{
	public:
		struct CreateInfo
		{
			QueueCapabilities Capabilities;
			GTSL::float32 QueuePriority = 1.0f;
		};

		struct DispatchInfo : RenderInfo
		{
			GTSL::Ranger<class CommandBuffer*> CommandBuffers;
		};
		void Dispatch(const DispatchInfo& dispatchInfo);

	private:
		friend RenderDevice;
	};

	class RenderDevice
	{
	public:
		struct CreateInfo
		{
			GTSL::Ranger<GTSL::UTF8> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Ranger<Queue::CreateInfo> QueueCreateInfos;
			GTSL::Ranger<GAL::Queue*> Queues;
		};
		
		[[nodiscard]] GTSL::AllocatorReference* GetPersistentAllocationsAllocatorReference() const { return persistentAllocatorReference; }
		[[nodiscard]] GTSL::AllocatorReference* GetTransientAllocationsAllocatorReference() const { return transientAllocatorReference; }
		
	protected:
		RenderDevice() = default;
		~RenderDevice() = default;

		GTSL::AllocatorReference* persistentAllocatorReference{ nullptr };
		GTSL::AllocatorReference* transientAllocatorReference{ nullptr };

	};
}
