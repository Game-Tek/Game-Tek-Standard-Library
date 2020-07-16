#pragma once
#include "RenderCore.h"

namespace GAL
{
	/**
	 * \brief Object to achieve host-device synchronization.
	 */
	class Fence : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			bool IsSignaled{ true };
		};
	};

	/**
	 * \brief Object to achieve inter queue synchronization.
	 */
	class Semaphore : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::uint64 InitialValue{ 0 };
		};
	};
}
