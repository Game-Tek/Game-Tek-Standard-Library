#pragma once

#include "GAL/Pipelines.h"

#include "Vulkan.h"

#include <GTSL/Vector.hpp>

namespace GTSL {
	class String;
}

namespace GAL
{
	class VulkanShader final : public Shader
	{
	public:
		VulkanShader() = default;
		explicit VulkanShader(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);
		
		static bool CompileShader(GTSL::Ranger<const GTSL::UTF8> code, GTSL::Ranger<const GTSL::UTF8> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::Vector<GTSL::byte>& result, GTSL::String& compilationResult, const GTSL::AllocatorReference& allocatorReference);

		[[nodiscard]] VkShaderModule GetVkShaderModule() const { return shaderModule; }
	protected:
		VkShaderModule shaderModule{ nullptr };
	};

	class VulkanGraphicsPipeline final : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline() = default;
		VulkanGraphicsPipeline(const CreateInfo& createInfo);
		~VulkanGraphicsPipeline() = default;

		void Destroy(RenderDevice* renderDevice);

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

		void Destroy(RenderDevice* renderDevice);

		[[nodiscard]] VkPipeline GetVkPipeline() const { return pipeline; }

	private:
		VkPipeline pipeline = nullptr;
	};
}
