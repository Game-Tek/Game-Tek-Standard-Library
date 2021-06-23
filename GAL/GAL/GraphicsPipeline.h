#pragma once

#include "RenderCore.h"

#include <GTSL/Array.hpp>
#include <GTSL/String.hpp>
#include <GTSL/Extent.h>

#include "GTSL/FixedVector.hpp"

namespace GAL
{
	struct StencilState
	{
		StencilCompareOperation FailOperation = StencilCompareOperation::ZERO;
		StencilCompareOperation PassOperation = StencilCompareOperation::ZERO;
		StencilCompareOperation DepthFailOperation = StencilCompareOperation::ZERO;
		CompareOperation CompareOperation = CompareOperation::NEVER;
	};

	struct StencilOperations
	{
		StencilState Front;
		StencilState Back;
	};

	struct ShaderInfo
	{
		ShaderType Type = ShaderType::VERTEX_SHADER;
		GTSL::Ranger<GTSL::UTF8> Name;
		GTSL::Ranger<GTSL::byte> ShaderData;
	};

	struct PipelineDescriptor
	{
		GTSL::Ranger<ShaderInfo> Stages;
		CullMode CullMode = CullMode::CULL_NONE;
		bool DepthClampEnable = false;
		bool BlendEnable = false;
		BlendOperation ColorBlendOperation = BlendOperation::ADD;
		SampleCount RasterizationSamples = SampleCount::SAMPLE_COUNT_1;
		CompareOperation DepthCompareOperation = CompareOperation::NEVER;
		StencilOperations StencilOperations;
		GTSL::Ranger<GTSL::byte> PipelineCache;
	};

	class RenderPass;

	class Pipeline : public GALObject
	{
	public:
		struct PushConstant
		{
			size_t Size = 0;
			ShaderType Stage = ShaderType::ALL_STAGES;
		};
	};
	
	class GraphicsPipeline : public Pipeline
	{
	public:
		struct CreateInfo : RenderInfo
		{
			RenderPass* RenderPass = nullptr;
			GTSL::Extent2D SurfaceExtent;
			GTSL::Ranger<ShaderDataTypes> VertexDescriptor;
			PipelineDescriptor PipelineDescriptor;
			bool IsInheritable = false;
			GraphicsPipeline* ParentPipeline = nullptr;

			PushConstant* PushConstant = nullptr;
			GTSL::Ranger<class BindingsSet*> BindingsSets;
		};

		struct PipelineCache : RenderInfo
		{
			GTSL::Ranger<GTSL::byte> Data;
		};
		
		static GTSL::uint32 GetVertexSize(GTSL::Ranger<ShaderDataTypes> vertex)
		{
			GTSL::uint32 size{ 0 };	for (auto& e : vertex) { size += ShaderDataTypesSize(e); } return size;
		}

		static GTSL::uint32 GetByteOffsetToMember(const GTSL::uint8 member, GTSL::Ranger<ShaderDataTypes> vertex)
		{
			GTSL::uint32 offset{ 0 };
			for(auto* begin = vertex.begin(); begin != vertex.begin() + member; ++begin) { offset += ShaderDataTypesSize(*begin); }
			return offset;
		}
	};

}
