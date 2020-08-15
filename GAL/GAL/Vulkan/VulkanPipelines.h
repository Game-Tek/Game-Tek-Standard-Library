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
		
		[[nodiscard]] static bool CompileShader(GTSL::Ranger<const GTSL::UTF8> code, GTSL::Ranger<const GTSL::UTF8> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::Buffer& result, GTSL::Buffer& stringError);

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

	class VulkanPipeline : public Pipeline
	{
	public:
		struct ShaderInfo
		{
			VulkanShaderType Type = VulkanShaderType::VERTEX;
			const VulkanShader* Shader = nullptr;
		};


		[[nodiscard]] VkPipeline GetVkPipeline() const { return pipeline; }
	protected:
		VkPipeline pipeline = nullptr;
	};

	class VulkanPipelineLayout final
	{
	public:
		VulkanPipelineLayout() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			const struct PushConstant* PushConstant = nullptr;
			GTSL::Ranger<const class VulkanBindingsSetLayout> BindingsSetLayouts;
		};
		VulkanPipelineLayout(const CreateInfo& createInfo);

		void Initialize(const CreateInfo& createInfo);
		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkPipelineLayout GetVkPipelineLayout() const { return pipelineLayout; }
	private:
		VkPipelineLayout pipelineLayout = nullptr;
	};
	
	class VulkanGraphicsPipeline final : public VulkanPipeline
	{
	public:
		VulkanGraphicsPipeline() = default;
		
		struct CreateInfo : VulkanCreateInfo
		{
			const class VulkanRenderPass* RenderPass = nullptr;
			GTSL::Extent2D SurfaceExtent;
			GTSL::Ranger<const VulkanShaderDataType> VertexDescriptor;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::Ranger<const ShaderInfo> Stages;
			bool IsInheritable = false;
			const VulkanPipelineLayout* PipelineLayout = nullptr;
			const VulkanGraphicsPipeline* ParentPipeline = nullptr;
			const VulkanPipelineCache* PipelineCache = nullptr;
		};
		
		VulkanGraphicsPipeline(const CreateInfo& createInfo);
		~VulkanGraphicsPipeline() = default;

		void Destroy(const VulkanRenderDevice* renderDevice);

	private:
		static GTSL::uint32 GetVertexSizeAndOffsetsToMembers(GTSL::Ranger<const VulkanShaderDataType> vertex, GTSL::Ranger<GTSL::uint8> offsets)
		{
			GTSL::uint32 size = 0;
			for (const auto& e : vertex)
			{
				*(offsets + RangeForIndex(e, vertex)) = size;
				size += VulkanShaderDataTypeSize(e);
			}
			return size;
		}
	};

	class VulkanComputePipeline final : public VulkanPipeline
	{
	public:
		struct CreateInfo : VulkanRenderInfo
		{
		};
		VulkanComputePipeline(const CreateInfo& createInfo);
		~VulkanComputePipeline() = default;

		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkPipeline GetVkPipeline() const { return pipeline; }

	private:
		VkPipeline pipeline = nullptr;
	};

	class VulkanRaytracingPipeline final : public VulkanPipeline
	{
	public:
		struct CreateInfo : VulkanCreateInfo
		{
			bool IsInheritable = false;
			const VulkanRaytracingPipeline* ParentPipeline = nullptr;

			const VulkanPipelineLayout* PipelineLayout = nullptr;
			
			GTSL::uint32 MaxRecursionDepth = 0;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::Ranger<const ShaderInfo> Stages;
			GTSL::Ranger<const VulkanBindingsSetLayout> BindingsSetLayouts;
		};

		VulkanRaytracingPipeline(const CreateInfo& createInfo);
		void Destroy(const VulkanRenderDevice* renderDevice);
	};
}
