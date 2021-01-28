#pragma once

#include "GAL/Pipelines.h"

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"
#include "VulkanRenderPass.h"
#include "GTSL/Array.hpp"

namespace GTSL {
	class BufferInterface;
}

namespace GAL
{
	[[nodiscard]] bool CompileShader(GTSL::Range<const GTSL::UTF8*> code, GTSL::Range<const GTSL::UTF8*> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::BufferInterface result, GTSL::BufferInterface stringError);

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
		void GetCache(const VulkanRenderDevice* renderDevice, GTSL::BufferInterface buffer) const;
		
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
	
	class VulkanPipeline : public Pipeline
	{
	public:
		struct ShaderInfo
		{
			VulkanShaderType Type = VulkanShaderType::VERTEX;
			GTSL::Range<const GTSL::byte*> Blob;
		};

		struct CreateInfo : VulkanCreateInfo
		{
			VulkanPipelineLayout PipelineLayout;
			VulkanPipelineCache PipelineCache;
		};
		
		[[nodiscard]] VkPipeline GetVkPipeline() const { return pipeline; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<uint64_t>(pipeline); }
	protected:
		VkPipeline pipeline = nullptr;
	};
	
	class VulkanRasterizationPipeline final : public VulkanPipeline
	{
	public:
		VulkanRasterizationPipeline() = default;
		
		struct CreateInfo : VulkanPipeline::CreateInfo
		{
			VulkanRenderPass RenderPass;
			uint32_t SubPass;
			GTSL::Extent2D SurfaceExtent;
			GTSL::Range<const VulkanShaderDataType*> VertexDescriptor;
			VulkanPipelineDescriptor PipelineDescriptor;
			GTSL::uint8 AttachmentCount = 0;
			GTSL::Range<const ShaderInfo*> Stages;
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
		VulkanComputePipeline() = default;
		
		struct CreateInfo : VulkanPipeline::CreateInfo
		{
			ShaderInfo ShaderInfo;	
		};
		~VulkanComputePipeline() = default;

		void Initialize(const CreateInfo& createInfo);

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
		
		struct CreateInfo : VulkanPipeline::CreateInfo
		{			
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
