#pragma once

#include <GTSL/Core.h>

#include "RenderCore.h"

#include "CommandBuffer.h"
#include "GTSL/Allocator.h"

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
		GTSL::uint8 PipelineCacheUUID[16];
	};

	class Queue
	{
	public:
		struct CreateInfo
		{
			GTSL::uint32 Capabilities{ 0 };
			GTSL::float32 QueuePriority = 1.0f;
		};

		struct SubmitInfo : RenderInfo
		{
			GTSL::Ranger<const class CommandBuffer> CommandBuffers;
			GTSL::Ranger<const class Semaphore> SignalSemaphores;
			GTSL::Ranger<GTSL::uint64> SignalValues;
			GTSL::Ranger<const class Semaphore> WaitSemaphores;
			GTSL::Ranger<GTSL::uint64> WaitValues;
			/**
			 * \brief Pipeline stages at which each corresponding semaphore wait will occur.
			 */
			GTSL::Ranger<GTSL::uint32> WaitPipelineStages;
			class Fence* Fence{ nullptr };
		};
		void Submit(const SubmitInfo& dispatchInfo);

	private:
		friend RenderDevice;
	};

	class RenderDevice
	{
	public:
		struct CreateInfo
		{
			GTSL::Ranger<const GTSL::UTF8> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Ranger<Queue::CreateInfo> QueueCreateInfos;
			GTSL::Ranger<Queue> Queues;
		};

		struct BufferMemoryRequirements
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 Alignment{ 0 };
			GTSL::uint32 MemoryTypes{ 0 };
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
