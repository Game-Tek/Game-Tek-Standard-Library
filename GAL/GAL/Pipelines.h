#pragma once

#include "RenderCore.h"
#include <GTSL/Extent.h>
#include <GTSL/Ranger.h>

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

	class Shader : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Ranger<const GTSL::byte> ShaderData;
		};
	};
	
	class RenderPass;

	class Pipeline : public GALObject
	{
	public:
		struct PushConstant
		{
			GTSL::uint32 Size = 0;
			ShaderType Stage = ShaderType::VERTEX_SHADER;
		};

		//struct ShaderInfo
		//{
		//	ShaderType Type = ShaderType::VERTEX_SHADER;
		//	const Shader* Shader = nullptr;
		//};
	};

	class PipelineCache
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Ranger<const GTSL::byte> Data;
		};
	private:
	};

	//struct PipelineDescriptor
	//{
	//	GTSL::Ranger<const Pipeline::ShaderInfo> Stages;
	//	CullMode CullMode = CullMode::CULL_NONE;
	//	bool DepthClampEnable = false;
	//	bool BlendEnable = false;
	//	BlendOperation ColorBlendOperation = BlendOperation::ADD;
	//	SampleCount RasterizationSamples = SampleCount::SAMPLE_COUNT_1;
	//	CompareOperation DepthCompareOperation = CompareOperation::NEVER;
	//	StencilOperations StencilOperations;
	//};
	
	class GraphicsPipeline : public Pipeline
	{
	public:		
		//struct CreateInfo : RenderInfo
		//{
		//	const RenderPass* RenderPass = nullptr;
		//	GTSL::Extent2D SurfaceExtent;
		//	GTSL::Ranger<const ShaderDataTypes> VertexDescriptor;
		//	PipelineDescriptor PipelineDescriptor;
		//	bool IsInheritable = false;
		//	const GraphicsPipeline* ParentPipeline = nullptr;
		//
		//	const PushConstant* PushConstant = nullptr;
		//	GTSL::Ranger<const class BindingsPool> BindingsPools;
		//	const PipelineCache* PipelineCache = nullptr;
		//};

		static constexpr GTSL::uint8 MAX_VERTEX_ELEMENTS = 20;
		
		GraphicsPipeline() = default;

		static GTSL::uint32 GetVertexSizeAndOffsetsToMembers(GTSL::Ranger<const ShaderDataTypes> vertex, GTSL::Ranger<GTSL::uint8> offsets)
		{
			GTSL::uint32 size = 0;
			for (const auto& e : vertex)
			{
				*(offsets + RangeForIndex(e, vertex)) = size;
				size += ShaderDataTypesSize(e);
			}
			return size;
		}
		
		static GTSL::uint32 GetVertexSize(GTSL::Ranger<const ShaderDataTypes> vertex)
		{
			GTSL::uint32 size{ 0 };	for (const auto& e : vertex) { size += ShaderDataTypesSize(e); } return size;
		}

		static GTSL::uint32 GetByteOffsetToMember(const GTSL::uint8 member, GTSL::Ranger<const ShaderDataTypes> vertex)
		{
			GTSL::uint32 offset{ 0 };
			for(auto* begin = vertex.begin(); begin != vertex.begin() + member; ++begin) { offset += ShaderDataTypesSize(*begin); }
			return offset;
		}
	};

	class ComputePipeline : public Pipeline
	{
	public:
		struct CreateInfo : RenderInfo
		{
		};
	};

}
