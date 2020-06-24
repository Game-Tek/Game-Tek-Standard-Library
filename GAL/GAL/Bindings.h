#pragma once

#include "RenderCore.h"

#include <GTSL/Array.hpp>

namespace GAL
{
	constexpr GTSL::uint8 MAX_BINDINGS_PER_SET = 10;

	class RenderContext;

	struct BindingDescriptor
	{
		BindingType BindingType = BindingType::UNIFORM_BUFFER;
		ShaderType ShaderStage = ShaderType::ALL_STAGES;
	};
	
	struct ImageBindingDescriptor : BindingDescriptor
	{
		GTSL::Ranger<class Image*> Images;
		GTSL::Ranger<class Sampler*> Samplers;
		GTSL::Ranger<ImageLayout> Layouts;
	};

	struct BufferBindingDescriptor : BindingDescriptor
	{
		GTSL::Ranger<class Buffer*> Buffers;
		GTSL::Ranger<GTSL::uint32> Offsets;
		GTSL::Ranger<GTSL::uint32> Sizes;
	};

	struct BindingLayoutCreateInfo
	{
		GTSL::Array<ImageBindingDescriptor, MAX_BINDINGS_PER_SET> BindingsSetLayout;
		GTSL::int32 DescriptorCount = 0;
	};

	struct BindingsSetUpdateInfo : RenderInfo
	{
		GTSL::Array<ImageBindingDescriptor, MAX_BINDINGS_PER_SET> ImageBindingsSetLayout;
		GTSL::Array<BufferBindingDescriptor, MAX_BINDINGS_PER_SET> BufferBindingsSetLayout;
		GTSL::uint8 DestinationSet = 0;
	};

	class BindingsPool : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Array<ImageBindingDescriptor, MAX_BINDINGS_PER_SET> BindingsSetLayout;
			/**
			 * \brief How many sets to allocate.
			 */
			GTSL::uint8 BindingsSetCount = 0;
		};
		
		~BindingsPool() = default;

		struct FreeBindingsPoolInfo : RenderInfo
		{
		};

		void FreePool(const FreeBindingsPoolInfo& freeDescriptorPoolInfo);
		
		struct FreeBindingsSetInfo : RenderInfo
		{
			class BindingsSet* BindingsSet = nullptr;
		};
		void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo);
	};

	class BindingsSet : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			/**
			 * \brief Pointer to a binding pool to allocated the bindings set from.
			 */
			BindingsPool* BindingsPool = nullptr;
			
			GTSL::Ranger<BindingDescriptor> BindingsSetLayout;
			GTSL::Ranger<GTSL::uint8> BindingsSetBindingCount;
			
			/**
			 * \brief How many sets to allocate.
			 */
			GTSL::uint8 BindingsSetCount = 0;
		};
		
		~BindingsSet() = default;

		void Update(const BindingsSetUpdateInfo& uniformLayoutUpdateInfo);
	};

}
