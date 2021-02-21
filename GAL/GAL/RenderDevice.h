#pragma once

#include <GTSL/Core.h>

#include "GTSL/Allocator.h"
#include "GTSL/Delegate.hpp"

#include "GTSL/StaticString.hpp"

#undef ERROR

namespace GAL
{
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
	};

	class RenderDevice
	{
	public:
		enum class MessageSeverity : GTSL::uint8 { MESSAGE, WARNING, ERROR };

		enum class Extension
		{
			RAY_TRACING, PIPELINE_CACHE_EXTERNAL_SYNC, SCALAR_LAYOUT, BUFFER_REFERENCE
		};

		struct AllocationInfo
		{
			/**
			 * \brief void* UserData. uint64 Size. uint64 Alignment.
			 */
			GTSL::Delegate<void* (void*, GTSL::uint64, GTSL::uint64)> Allocate;
			/**
			 * \brief void* UserData. void* Original Allocation. uint64 Size. uint64 Alignment.
			 */
			GTSL::Delegate<void* (void*, void*, GTSL::uint64, GTSL::uint64)> Reallocate;
			/**
			 * \brief void* UserData. void* Allocation.
			 */
			GTSL::Delegate<void(void*, void*)> Deallocate;

			/**
			 * \brief void* UserData. uint64 Size. uint64 Alignment.
			 */
			GTSL::Delegate<void(void*, GTSL::uint64, GTSL::uint64)> InternalAllocate;
			/**
			* \brief void* UserData. void* Allocation.
			*/
			GTSL::Delegate<void(void*, void*)> InternalDeallocate;

			void* UserData;
		};
		
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
