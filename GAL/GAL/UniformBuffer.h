#pragma once

#include "RenderCore.h"

namespace GAL
{

	struct UniformBufferCreateInfo
	{
		size_t Size = 0;
	};

	struct UniformBufferUpdateInfo
	{
		/**
		 * \brief Pointer to the data to copy.
		 */
		void* Data = nullptr;
		/**
		 * \brief Size(bytes) of the data being copied.
		 */
		size_t Size = 0;
		/**
		 * \brief Offset(bytes) into the buffer to copy data to.
		 */
		size_t Offset = 0;
	};

	class UniformBuffer : public GALObject
	{
	public:
		virtual ~UniformBuffer() = default;
		virtual void UpdateBuffer(const UniformBufferUpdateInfo& _BUI) const = 0;
		void Destroy(class RenderDevice* renderDevice) = 0;
	};
}
