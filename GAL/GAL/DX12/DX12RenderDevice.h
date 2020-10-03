#pragma once

#include "GAL/RenderDevice.h"

#include "DX12.h"

#include "GTSL/Delegate.hpp"
#include "GTSL/Range.h"

namespace GAL
{
	class DX12RenderDevice;
	
	class DX12Queue final : public Queue
	{
	public:
		DX12Queue() = default;

		struct SubmitInfo final : DX12RenderInfo
		{
			GTSL::Range<const class DX12CommandBuffer*> CommandBuffers;
			//GTSL::Range<const VulkanSemaphore*> SignalSemaphores;
			//GTSL::Range<const VulkanSemaphore*> WaitSemaphores;
			GTSL::Range<const GTSL::uint64*> SignalValues;
			GTSL::Range<const GTSL::uint64*> WaitValues;
			/**
			 * \brief Pipeline stages at which each corresponding semaphore wait will occur.
			 */
			GTSL::Range<const GTSL::uint32*> WaitPipelineStages;
			//const VulkanFence* Fence{ nullptr };
		};
		void Submit(const SubmitInfo& submitInfo) const;

		void Wait(const DX12RenderDevice* renderDevice) const;
		
		~DX12Queue();
		
		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(commandQueue); }
		[[nodiscard]] ID3D12CommandQueue* GetID3D12CommandQueue() const { return commandQueue; }
		
	private:
		ID3D12CommandQueue* commandQueue = nullptr;

		friend class DX12RenderDevice;
	};
	
	class DX12RenderDevice : public RenderDevice
	{
	public:
		struct CreateInfo : DX12CreateInfo
		{
			GTSL::Range<const GTSL::UTF8*> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Range<const Queue::CreateInfo*> QueueCreateInfos;
			GTSL::Range<void**> Queues;
#if (_DEBUG)
			GTSL::Delegate<void(const char*, MessageSeverity)> DebugPrintFunction;
#endif
			GTSL::Range<const Extension*> Extensions;
			GTSL::Range<void**> ExtensionCapabilities;
			AllocationInfo AllocationInfo;
		};
		void Initialize(const CreateInfo& info);

		~DX12RenderDevice();

		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(device); }
		[[nodiscard]] ID3D12Device2* GetID3D12Device2() const { return device; }

	private:
		ID3D12Device2* device = nullptr;

#if (_DEBUG)
		ID3D12Debug* debug = nullptr;
#endif
	};
}