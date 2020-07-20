#pragma once

#include "GAL/Pipelines.h"

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
	
	class VulkanGraphicsPipeline final : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline() = default;
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
}
