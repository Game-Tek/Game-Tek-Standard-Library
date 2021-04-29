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

		struct PipelineStateBlock
		{
			struct ViewportState
			{
				GTSL::uint8 ViewportCount = 0;
			};

			struct RasterState
			{				
				WindingOrder WindingOrder = WindingOrder::CLOCKWISE;
				CullMode CullMode = CullMode::CULL_BACK;
			};

			struct DepthState
			{
				CompareOperation CompareOperation = CompareOperation::LESS;
			};

			struct RenderContext
			{
				struct AttachmentState
				{
					bool BlendEnable = true;
				};

				GTSL::Range<const AttachmentState*> Attachments;
				const RenderPass* RenderPass = nullptr;
				GTSL::uint8 SubPassIndex = 0;
			};

			struct VertexState
			{
				GTSL::Range<const VertexElement*> VertexDescriptor;
			};

			union Blocks {
				ViewportState Viewport;
				RasterState Raster;
				DepthState Depth;
				RenderContext Context;
				VertexState Vertex;

				Blocks() = default;
				Blocks(const RasterState& rasterState) : Raster(rasterState) {}
				Blocks(const DepthState& depth) : Depth(depth) {}
				Blocks(const RenderContext& renderContext) : Context(renderContext) {}
				Blocks(const VertexState& vertexState) : Vertex(vertexState) {}
			} Block;
			

			enum class StateType
			{
				VIEWPORT_STATE, RASTER_STATE, DEPTH_STATE, COLOR_BLEND_STATE, VERTEX_STATE
			} Type = StateType::VIEWPORT_STATE;
			
			PipelineStateBlock() = default;
			PipelineStateBlock(const RasterState& rasterState) : Block(rasterState), Type(StateType::RASTER_STATE) {}
			PipelineStateBlock(const DepthState& depth) : Block(depth), Type(StateType::DEPTH_STATE) {}
			PipelineStateBlock(const RenderContext& renderContext) : Block(renderContext), Type(StateType::COLOR_BLEND_STATE) {}
			PipelineStateBlock(const VertexState& vertexState) : Block(vertexState), Type(StateType::VERTEX_STATE) {}
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
	private:
	};
	
	class GraphicsPipeline : public Pipeline
	{
	public:
		
		GraphicsPipeline() = default;
		
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
