#pragma once

#include "GAL/Pipelines.h"

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"

namespace GTSL {
	class Buffer;
}

namespace GAL
{
	class VulkanShader final : public Shader
	{
	public:
		VulkanShader() = default;
		explicit VulkanShader(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] static bool CompileShader(GTSL::Ranger<const GTSL::UTF8> code, GTSL::Ranger<const GTSL::UTF8> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::Buffer& result);

		[[nodiscard]] VkShaderModule GetVkShaderModule() const { return shaderModule; }
	protected:
		VkShaderModule shaderModule{ nullptr };
	};

	class VulkanPipelineCache : public PipelineCache
	{
	public:
		VulkanPipelineCache() = default;
		VulkanPipelineCache(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkPipelineCache GetVkPipelineCache() const { return pipelineCache; }

		void GetCacheSize(const VulkanRenderDevice* renderDevice, GTSL::uint32& size) const;
		void GetCache(const VulkanRenderDevice* renderDevice, GTSL::uint32 size, GTSL::Buffer& buffer) const;
		
	private:
		VkPipelineCache pipelineCache = nullptr;
	};

	struct VulkanPipelineDescriptor
	{
		CullMode CullMode = CullMode::CULL_NONE;
		bool DepthClampEnable = false;
		bool BlendEnable = false;
		BlendOperation ColorBlendOperation = BlendOperation::ADD;
		SampleCount RasterizationSamples = SampleCount::SAMPLE_COUNT_1;
		CompareOperation DepthCompareOperation = CompareOperation::NEVER;
		StencilOperations StencilOperations;
	};

	class VulkanPipeline
	{
	public:
		struct ShaderInfo
		{
			VulkanShaderType Type = VulkanShaderType::VERTEX;
			const VulkanShader* Shader = nullptr;
		};
	};
	
	class VulkanGraphicsPipeline final : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline() = default;
		
		struct CreateInfo : VulkanRenderInfo
		{
			const class VulkanRenderPass* RenderPass = nullptr;
			GTSL::Extent2D SurfaceExtent;
			GTSL::Ranger<const ShaderDataTypes> VertexDescriptor;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::Ranger<const VulkanPipeline::ShaderInfo> Stages;
			bool IsInheritable = false;
			const VulkanGraphicsPipeline* ParentPipeline = nullptr;

			const PushConstant* PushConstant = nullptr;
			GTSL::Ranger<const class VulkanBindingsSetLayout> BindingsSetLayouts;
			const VulkanPipelineCache* PipelineCache = nullptr;
		};
		
		VulkanGraphicsPipeline(const CreateInfo& createInfo);
		~VulkanGraphicsPipeline() = default;

		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkPipeline GetVkGraphicsPipeline() const { return pipeline; }
		[[nodiscard]] VkPipelineLayout GetVkPipelineLayout() const { return pipelineLayout; }

	private:
		VkPipelineLayout pipelineLayout = nullptr;
		VkPipeline pipeline = nullptr;
	};

	class VulkanComputePipeline final : public ComputePipeline
	{
	public:
		VulkanComputePipeline(const CreateInfo& createInfo);
		~VulkanComputePipeline() = default;

		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkPipeline GetVkPipeline() const { return pipeline; }

	private:
		VkPipeline pipeline = nullptr;
	};

	class VulkanRaytracingPipeline final
	{
	public:
		struct CreateInfo : VulkanRenderInfo
		{
			bool IsInheritable = false;
			const VulkanRaytracingPipeline* ParentPipeline = nullptr;
			
			GTSL::uint32 MaxRecursionDepth = 0;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::Ranger<const VulkanPipeline::ShaderInfo> Stages;
			GTSL::Ranger<const VulkanBindingsSetLayout> BindingsSetLayouts;
		};

		VkPipeline GetVkPipeline() const { return pipeline; }
		VulkanRaytracingPipeline(const CreateInfo& createInfo);
		
	private:
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
}
