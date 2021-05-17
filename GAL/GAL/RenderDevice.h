#pragma once

#include <GTSL/Core.h>

#include "RenderCore.h"
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

	class RenderDevice
	{
	public:
		enum class MessageSeverity : GTSL::uint8 { MESSAGE, WARNING, ERROR };

		enum class Extension {
			RAY_TRACING, PIPELINE_CACHE_EXTERNAL_SYNC, SCALAR_LAYOUT, SWAPCHAIN_RENDERING
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

		GTSL::Delegate<void(const char*, MessageSeverity)>& GetDebugPrintFunction() { return debugPrintFunction; }
		
	protected:
		RenderDevice() = default;
		~RenderDevice() = default;

		GTSL::Delegate<void(const char*, MessageSeverity)> debugPrintFunction;
	};
}
