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
		GTSL::String* ShaderCode = nullptr;
	};

	struct PipelineDescriptor
	{
		GTSL::FixedVector<ShaderInfo> Stages;
		CullMode CullMode = CullMode::CULL_NONE;
		bool DepthClampEnable = false;
		bool BlendEnable = false;
		BlendOperation ColorBlendOperation = BlendOperation::ADD;
		SampleCount RasterizationSamples = SampleCount::SAMPLE_COUNT_1;
		CompareOperation DepthCompareOperation = CompareOperation::NEVER;
		StencilOperations StencilOperations;
	};

	class RenderPass;

	class Pipeline : public GALObject
	{
	};

	class GraphicsPipeline : public Pipeline
	{
	public:
	};

	struct PushConstant
	{
		size_t Size = 0;
		ShaderType Stage = ShaderType::ALL_STAGES;
	};

	struct GraphicsPipelineCreateInfo : RenderInfo
	{
		RenderPass* RenderPass = nullptr;
		GTSL::Extent2D SurfaceExtent;
		VertexDescriptor* VDescriptor = nullptr;
		PipelineDescriptor PipelineDescriptor;
		GraphicsPipeline* ParentPipeline = nullptr;

		PushConstant* PushConstant = nullptr;
		GTSL::Array<class BindingsSet*, 16> BindingsSets;
	};

}
