#pragma once

#include "RenderCore.h"
#include "GTSL/Core.h"

namespace GAL {
	class CommandBuffer;
	class Semaphore;

	class Queue {
	public:
		struct WorkUnit final {
			const CommandBuffer* CommandBuffer = nullptr;
			const Semaphore* SignalSemaphore = nullptr;
			const Semaphore* WaitSemaphore = nullptr;
			GTSL::uint64 SignalValue = 0, WaitValue = 0;
			/**
			 * \brief Pipeline stages at which each corresponding semaphore wait will occur.
			 */
			PipelineStage WaitPipelineStage;
		};
	};
}
