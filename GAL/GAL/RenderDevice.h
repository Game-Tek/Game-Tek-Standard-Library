#pragma once

#include <GTSL/Core.h>

#include "RenderCore.h"

#include "CommandBuffer.h"
#include "GTSL/Allocator.h"
#include "GTSL/Delegate.hpp"

#include "GTSL/StaticString.hpp"

#undef ERROR

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
		//void Submit(const SubmitInfo& dispatchInfo);

	private:
		friend RenderDevice;
	};

	class RenderDevice
	{
	public:
		enum class MessageSeverity : GTSL::uint8 { MESSAGE, WARNING, ERROR };
		
		//struct CreateInfo
		//{
		//	GTSL::Range<const GTSL::UTF8> ApplicationName;
		//	GTSL::uint16 ApplicationVersion[3];
		//	GTSL::Range<const Queue::CreateInfo> QueueCreateInfos;
		//	GTSL::Range<Queue*> Queues;
		//	GTSL::Delegate<void(const char*, MessageSeverity)> DebugPrintFunction;
		//};
		RenderDevice(GTSL::Delegate<void(const char*, MessageSeverity)> pDelegate) : debugPrintFunction(pDelegate)
		{
		}

		struct MemoryRequirements
		{
			GTSL::uint32 Size{ 0 };
			GTSL::uint32 Alignment{ 0 };
			GTSL::uint32 MemoryTypes{ 0 };
		};
		
		[[nodiscard]] GTSL::AllocatorReference* GetPersistentAllocationsAllocatorReference() const { return persistentAllocatorReference; }
		[[nodiscard]] GTSL::AllocatorReference* GetTransientAllocationsAllocatorReference() const { return transientAllocatorReference; }

		GTSL::Delegate<void(const char*, MessageSeverity)>& GetDebugPrintFunction() { return debugPrintFunction; }
		
	protected:
		RenderDevice() = default;
		~RenderDevice() = default;

		GTSL::Delegate<void(const char*, MessageSeverity)> debugPrintFunction;
		
		GTSL::AllocatorReference* persistentAllocatorReference{ nullptr };
		GTSL::AllocatorReference* transientAllocatorReference{ nullptr };

	};
}
