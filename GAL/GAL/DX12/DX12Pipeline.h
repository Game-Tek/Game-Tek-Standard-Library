#pragma once

#include "DX12.h"
#include "GAL/Pipelines.h"
#include "GAL/RenderCore.h"
#include "GTSL/Extent.h"

namespace GAL
{
	struct DX12PipelineDescriptor
	{
		CullMode CullMode = CullMode::CULL_NONE;
		bool DepthClampEnable = false;
		bool BlendEnable = false;
		BlendOperation ColorBlendOperation = BlendOperation::ADD;
		SampleCount RasterizationSamples = SampleCount::SAMPLE_COUNT_1;
		bool DepthTest = false;
		bool DepthWrite = false;
		CompareOperation DepthCompareOperation = CompareOperation::NEVER;
		bool StencilTest = false;
		StencilOperations StencilOperations;
	};

	struct DX12ShaderInfo
	{
		ShaderType Type;
		const DX12Shader* Shader = nullptr;
		GTSL::Range<const GTSL::byte*> ShaderData;
	};

	class DX12PipelineLayout final
	{
	public:
		DX12PipelineLayout() = default;

		struct CreateInfo final : DX12CreateInfo
		{
		};
		void Initialize(const CreateInfo& info);

		void Destroy(const DX12RenderDevice* renderDevice);
		
		~DX12PipelineLayout() = default;
		
	private:
		ID3D12RootSignature* rootSignature = nullptr;
	};
	
	class DX12RasterPipeline final
	{
	public:
		DX12RasterPipeline() = default;

		struct CreateInfo final : DX12CreateInfo
		{
			const class VulkanRenderPass* RenderPass = nullptr;
			GTSL::Extent2D SurfaceExtent;
			GTSL::Range<const DX12ShaderDataType*> VertexDescriptor;
			DX12PipelineDescriptor PipelineDescriptor;
			GTSL::Range<const DX12ShaderInfo*> Stages;
			bool IsInheritable = false;
			const DX12PipelineLayout* PipelineLayout = nullptr;
			const DX12RasterPipeline* ParentPipeline = nullptr;
			const DX12PipelineCache* PipelineCache = nullptr;
			GTSL::uint32 SubPass = 0;
		};
		void Initialize(const CreateInfo& info);

		void Destroy(const DX12RenderDevice* renderDevice);

		~DX12RasterPipeline() = default;
		
	private:
		ID3D12PipelineState* pipelineState = nullptr;
	};
}
