#pragma once

#include "GAL/GraphicsPipeline.h"
#include "GAL/ComputePipeline.h"

#include "Vulkan.h"

#include <GTSL/Vector.hpp>

namespace GAL
{
	class VulkanShaders
	{
	public:
		//takes an unitialized fvector
		static void CompileShader(GTSL::Ranger<GTSL::UTF8> code, GTSL::Ranger<GTSL::UTF8> shaderName, GTSL::uint32 shaderStage, GTSL::Vector<GTSL::uint32>& result);
	};

	class VulkanGraphicsPipeline final : public GraphicsPipeline
	{
	public:
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
		VulkanComputePipeline(const ComputePipelineCreateInfo& computePipelineCreateInfo);
		~VulkanComputePipeline() = default;

		void Destroy(RenderDevice* renderDevice);

		[[nodiscard]] VkPipeline GetVkPipeline() const { return pipeline; }

	private:
		VkPipeline pipeline = nullptr;
	};
}