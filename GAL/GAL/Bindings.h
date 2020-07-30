#pragma once

#include "RenderCore.h"

#include <GTSL/Array.hpp>

namespace GAL
{
	constexpr GTSL::uint8 MAX_BINDINGS_PER_SET = 10;

	class RenderContext;
	
	//struct BindingLayoutCreateInfo
	//{
	//	GTSL::Ranger<BindingDescriptor> BindingsSetLayout;
	//	GTSL::uint32 DescriptorCount = 0;
	//};

	class BindingsPool : public GALObject
	{
	public:
		//struct CreateInfo : RenderInfo
		//{
		//	GTSL::Ranger<BindingDescriptor> BindingsDescriptors;
		//	GTSL::Ranger<class BindingsSet> BindingsSets;
		//};
		
		~BindingsPool() = default;

		//struct BindingDescriptor
		//{
		//	BindingType BindingType = BindingType::UNIFORM_BUFFER;
		//	ShaderType ShaderStage = ShaderType::VERTEX_SHADER;
		//	GTSL::uint8 MaxNumberOfBindingsAllocatable{ 0 };
		//};
		//
		//struct ImageBindingDescriptor : BindingDescriptor
		//{
		//	GTSL::Ranger<const class ImageView> ImageViews;
		//	GTSL::Ranger<const class Sampler> Samplers;
		//	GTSL::Ranger<ImageLayout> Layouts;
		//};
		//
		//struct BufferBindingDescriptor : BindingDescriptor
		//{
		//	GTSL::Ranger<const class Buffer> Buffers;
		//	GTSL::Ranger<GTSL::uint32> Offsets;
		//	GTSL::Ranger<GTSL::uint32> Sizes;
		//};
		
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
		};
		
		~BindingsSet() = default;

		//struct BindingsSetUpdateInfo : RenderInfo
		//{
		//	GTSL::Array<ImageBindingDescriptor, MAX_BINDINGS_PER_SET> ImageBindingsSetLayout;
		//	GTSL::Array<BufferBindingDescriptor, MAX_BINDINGS_PER_SET> BufferBindingsSetLayout;
		//};
		//void Update(const BindingsSetUpdateInfo& uniformLayoutUpdateInfo);
	};

}
