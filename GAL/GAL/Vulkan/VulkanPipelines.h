#pragma once

#include "GAL/Pipelines.h"

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"
#include "GTSL/Array.hpp"

namespace GTSL {
	class Buffer;
}

namespace GAL
{
	class VulkanShader final : public Shader
	{
	public:
		VulkanShader() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			GTSL::Ranger<const GTSL::byte> ShaderData;
		};
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

		struct CreateInfo final : VulkanCreateInfo
		{
			bool ExternallySync = false;
			GTSL::Ranger<const GTSL::byte> Data;
		};
		VulkanPipelineCache(const CreateInfo& createInfo);

		struct CreateFromMultipleInfo final : VulkanCreateInfo
		{
			GTSL::Ranger<const VulkanPipelineCache> Caches;
		};
		VulkanPipelineCache(const CreateFromMultipleInfo& createInfo);

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
	
	class VulkanRasterizationPipeline final : public VulkanPipeline
	{
	public:
		VulkanRasterizationPipeline() = default;
		
		struct CreateInfo : VulkanCreateInfo
		{
			const class VulkanRenderPass* RenderPass = nullptr;
			GTSL::Extent2D SurfaceExtent;
			GTSL::Ranger<const VulkanShaderDataType> VertexDescriptor;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::Ranger<const ShaderInfo> Stages;
			bool IsInheritable = false;
			const VulkanPipelineLayout* PipelineLayout = nullptr;
			const VulkanRasterizationPipeline* ParentPipeline = nullptr;
			const VulkanPipelineCache* PipelineCache = nullptr;
		};
		
		VulkanRasterizationPipeline(const CreateInfo& createInfo);
		~VulkanRasterizationPipeline() = default;

		void Destroy(const VulkanRenderDevice* renderDevice);

	private:
		static GTSL::uint32 GetVertexSizeAndOffsetsToMembers(GTSL::Ranger<const VulkanShaderDataType> vertex, GTSL::Array<GTSL::uint8, 20>& offsets)
		{
			GTSL::uint32 size = 0;
			for (const auto& e : vertex)
			{
				offsets.EmplaceBack(size);
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
		struct Group
		{
		private:
			uint32_t type; void* next;
		public:
			VulkanShaderGroupType ShaderGroup;
			uint32_t GeneralShader;
			uint32_t ClosestHitShader;
			uint32_t AnyHitShader;
			uint32_t IntersectionShader;
		private:
			void* replayGroupHandle;
		};
		
		struct CreateInfo : VulkanCreateInfo
		{
			bool IsInheritable = false;
			const VulkanRaytracingPipeline* ParentPipeline = nullptr;

			const VulkanPipelineLayout* PipelineLayout = nullptr;
			
			GTSL::uint32 MaxRecursionDepth = 0;
			GTSL::Ranger<const ShaderInfo> Stages;
			GTSL::Ranger<const VulkanBindingsSetLayout> BindingsSetLayouts;

			GTSL::Ranger<Group> Groups;
		};

		VulkanRaytracingPipeline(const CreateInfo& createInfo);
		void Destroy(const VulkanRenderDevice* renderDevice);
	};
}
