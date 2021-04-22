#pragma once

#include "RenderCore.h"
#include <GTSL/Extent.h>
#include <GTSL/Range.h>

#include "GTSL/ShortString.hpp"

namespace GAL
{
	struct StencilOperations
	{
		struct StencilState
		{
			StencilCompareOperation FailOperation = StencilCompareOperation::ZERO;
			StencilCompareOperation PassOperation = StencilCompareOperation::ZERO;
			StencilCompareOperation DepthFailOperation = StencilCompareOperation::ZERO;
			CompareOperation CompareOperation = CompareOperation::NEVER;
			GTSL::uint32 CompareMask;
			GTSL::uint32 WriteMask;
			GTSL::uint32 Reference;
		} Front, Back;
	};

	class Shader : public GALObject
	{
	public:
	};
	
	class RenderPass;

	struct PushConstant
	{
		GTSL::uint32 Size = 0;
		ShaderType Stage = ShaderType::VERTEX_SHADER;
	};
	
	class Pipeline : public GALObject
	{
	public:

		static constexpr GTSL::uint8 MAX_VERTEX_ELEMENTS = 20;

		struct VertexElement {
			GTSL::ShortString<64> Identifier;
			GTSL::uint8 Index = 0;
			bool Enabled; ShaderDataType Type;
		};

		static constexpr auto POSITION = GTSL::ShortString<64>("POSITION");
		static constexpr auto NORMAL = GTSL::ShortString<64>("NORMAL");
		static constexpr auto TANGENT = GTSL::ShortString<64>("TANGENT");
		static constexpr auto BITANGENT = GTSL::ShortString<64>("BITANGENT");
		static constexpr auto TEXTURE_COORDINATES = GTSL::ShortString<64>("TEXTURE_COORDINATES");
		static constexpr auto COLOR = GTSL::ShortString<64>("COLOR");
		
		//struct ShaderInfo
		//{
		//	ShaderType Type = ShaderType::VERTEX_SHADER;
		//	const Shader* Shader = nullptr;
		//};
	};

	class PipelineCache
	{
	public:
	private:
	};

	//struct PipelineDescriptor
	//{
	//	GTSL::Range<const Pipeline::ShaderInfo> Stages;
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
		//	GTSL::Range<const ShaderDataTypes> VertexDescriptor;
		//	PipelineDescriptor PipelineDescriptor;
		//	bool IsInheritable = false;
		//	const GraphicsPipeline* ParentPipeline = nullptr;
		//
		//	const PushConstant* PushConstant = nullptr;
		//	GTSL::Range<const class BindingsPool> BindingsPools;
		//	const PipelineCache* PipelineCache = nullptr;
		//};
		
		GraphicsPipeline() = default;

		//static GTSL::uint32 GetVertexSizeAndOffsetsToMembers(GTSL::Range<const ShaderDataTypes> vertex, GTSL::Range<GTSL::uint8> offsets)
		//{
		//	GTSL::uint32 size = 0;
		//	for (const auto& e : vertex)
		//	{
		//		*(offsets + RangeForIndex(e, vertex)) = size;
		//		size += ShaderDataTypesSize(e);
		//	}
		//	return size;
		//}
		
		static GTSL::uint32 GetVertexSize(GTSL::Range<const ShaderDataType*> vertex)
		{
			GTSL::uint32 size{ 0 };	for (const auto& e : vertex) { size += ShaderDataTypesSize(e); } return size;
		}

		static GTSL::uint32 GetByteOffsetToMember(const GTSL::uint8 member, GTSL::Range<const ShaderDataType*> vertex)
		{
			GTSL::uint32 offset{ 0 };
			for(auto* begin = vertex.begin(); begin != vertex.begin() + member; ++begin) { offset += ShaderDataTypesSize(*begin); }
			return offset;
		}
	};

	class ComputePipeline : public Pipeline
	{
	public:
	};

}
