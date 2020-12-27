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
			GTSL::Range<const GTSL::byte*> ShaderData;
		};
		explicit VulkanShader(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		[[nodiscard]] static bool CompileShader(GTSL::Range<const GTSL::UTF8*> code, GTSL::Range<const GTSL::UTF8*> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::Buffer& result, GTSL::Buffer& stringError);

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
			GTSL::Range<const GTSL::byte*> Data;
		};
		VulkanPipelineCache(const CreateInfo& createInfo);

		struct CreateFromMultipleInfo final : VulkanCreateInfo
		{
			GTSL::Range<const VulkanPipelineCache*> Caches;
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
		bool DepthTest = false;
		bool DepthWrite = false;
		CompareOperation DepthCompareOperation = CompareOperation::NEVER;
		bool StencilTest = false;
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
		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<uint64_t>(pipeline); }
	protected:
		VkPipeline pipeline = nullptr;
	};

	class VulkanPipelineLayout final
	{
	public:
		struct PushConstant
		{
			GTSL::uint32 Size = 0, Offset = 0;
			VulkanShaderStage::value_type ShaderStages;
		};
		
		VulkanPipelineLayout() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			const PushConstant* PushConstant = nullptr;
			GTSL::Range<const class VulkanBindingsSetLayout*> BindingsSetLayouts;
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
			GTSL::Range<const VulkanShaderDataType*> VertexDescriptor;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::Range<const ShaderInfo*> Stages;
			bool IsInheritable = false;
			VulkanPipelineLayout PipelineLayout;
			VulkanPipeline ParentPipeline;
			VulkanPipelineCache PipelineCache;
			uint32_t SubPass;
		};
		
		VulkanRasterizationPipeline(const CreateInfo& createInfo);
		~VulkanRasterizationPipeline() = default;

		void Destroy(const VulkanRenderDevice* renderDevice);

	private:
		static GTSL::uint32 GetVertexSizeAndOffsetsToMembers(GTSL::Range<const VulkanShaderDataType*> vertex, GTSL::Array<GTSL::uint8, 20>& offsets)
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
	};

	class VulkanRayTracingPipeline final : public VulkanPipeline
	{
	public:
		struct Group
		{
		private:
			uint32_t type; void* next;
		public:
			static constexpr uint32_t SHADER_UNUSED = (~0U);
			
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
			VulkanPipeline ParentPipeline;
			VulkanPipelineCache PipelineCache;
			VulkanPipelineLayout PipelineLayout;
			
			GTSL::uint32 MaxRecursionDepth = 0;
			GTSL::Range<const ShaderInfo*> Stages;

			GTSL::Range<Group*> Groups;
		};

		explicit VulkanRayTracingPipeline() = default;
		void Initialize(const CreateInfo& createInfo);
		void Destroy(const VulkanRenderDevice* renderDevice);

		void GetShaderGroupHandles(VulkanRenderDevice* renderDevice, GTSL::uint32 firstGroup, GTSL::uint32 groupCount, GTSL::Range<GTSL::byte*> buffer);
	};
}
