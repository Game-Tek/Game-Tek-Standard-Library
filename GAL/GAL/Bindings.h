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
		GTSL::uint8 MaxNumberOfBindingsAllocatable{ 0 };
	};
	
	struct ImageBindingDescriptor : BindingDescriptor
	{
		GTSL::Ranger<const class Image> Images;
		GTSL::Ranger<const class Sampler> Samplers;
		GTSL::Ranger<ImageLayout> Layouts;
	};

	struct BufferBindingDescriptor : BindingDescriptor
	{
		GTSL::Ranger<const class Buffer> Buffers;
		GTSL::Ranger<GTSL::uint32> Offsets;
		GTSL::Ranger<GTSL::uint32> Sizes;
	};
	
	struct BindingLayoutCreateInfo
	{
		GTSL::Ranger<BindingDescriptor> BindingsSetLayout;
		GTSL::int32 DescriptorCount = 0;
	};

	struct BindingsSetUpdateInfo : RenderInfo
	{
		GTSL::Array<ImageBindingDescriptor, MAX_BINDINGS_PER_SET> ImageBindingsSetLayout;
		GTSL::Array<BufferBindingDescriptor, MAX_BINDINGS_PER_SET> BufferBindingsSetLayout;
	};

	class BindingsPool : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Ranger<BindingDescriptor> BindingsDescriptors;
			GTSL::Ranger<class BindingsSet> BindingsSets;
		};
		
		~BindingsPool() = default;
		
		struct FreeBindingsSetInfo : RenderInfo
		{
			GTSL::Ranger<class BindingsSet> BindingsSet;
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
		};
		
		~BindingsSet() = default;

		void Update(const BindingsSetUpdateInfo& uniformLayoutUpdateInfo);
	};

}
