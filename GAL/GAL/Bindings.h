#pragma once

#include "RenderCore.h"

#include <GTSL/Array.hpp>

namespace GAL
{
	constexpr GTSL::uint8 MAX_BINDINGS_PER_SET = 10;

	class RenderContext;

	struct BindingDescriptor
	{
		/**
		 * \brief If binding is an array how many elements does it have.
		 */
		GTSL::uint8 ArrayLength = 0;
		BindingType BindingType = BindingType::UNIFORM_BUFFER;
		ShaderType ShaderStage = ShaderType::ALL_STAGES;
		void* BindingResource = nullptr;
	};

	struct BindingLayoutCreateInfo
	{
		GTSL::Array<BindingDescriptor, MAX_BINDINGS_PER_SET> BindingsSetLayout;
		int DescriptorCount = 0;
	};

	struct BindingsSetUpdateInfo : RenderInfo
	{
		GTSL::Array<BindingDescriptor, MAX_BINDINGS_PER_SET> BindingsSetLayout;
		GTSL::uint8 DestinationSet = 0;
	};

	class BindingsPool : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Array<BindingDescriptor, MAX_BINDINGS_PER_SET> BindingsSetLayout;
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
			GTSL::Array<BindingDescriptor, MAX_BINDINGS_PER_SET> BindingsSetLayout;
			/**
			 * \brief How many sets to allocate.
			 */
			GTSL::uint8 BindingsSetCount = 0;
		};
		
		~BindingsSet() = default;

		void Update(const BindingsSetUpdateInfo& uniformLayoutUpdateInfo);
	};

}
