#pragma once

#include "GAL/GraphicsPipeline.h"
#include "GAL/ComputePipeline.h"

#include "Vulkan.h"

#include <GTSL/Vector.hpp>

class VulkanShaders
{
public:
	//takes an unitialized fvector
	static void CompileShader(const GTSL::String& code, const GTSL::String& shaderName, GTSL::uint32 shaderStage, GTSL::Vector<GTSL::uint32>& result);
};

class VulkanGraphicsPipeline final : public GAL::GraphicsPipeline
{
	VkPipelineLayout vkPipelineLayout = nullptr;
	VkPipeline vkPipeline = nullptr;

public:
	VulkanGraphicsPipeline(class VulkanRenderDevice* vulkanRenderDevice, const GAL::GraphicsPipelineCreateInfo& _GPCI);
	~VulkanGraphicsPipeline() = default;

	void Destroy(GAL::RenderDevice* renderDevice) override;

	[[nodiscard]] VkPipeline GetVkGraphicsPipeline() const { return vkPipeline; }
	[[nodiscard]] VkPipelineLayout GetVkPipelineLayout() const { return vkPipelineLayout; }
};

class VulkanComputePipeline final : public GAL::ComputePipeline
{
	VkPipeline vkPipeline = nullptr;

public:
	VulkanComputePipeline(class VulkanRenderDevice* vulkanRenderDevice, const GAL::ComputePipelineCreateInfo& computePipelineCreateInfo);
	~VulkanComputePipeline() = default;

	void Destroy(GAL::RenderDevice* renderDevice) override;
	
	[[nodiscard]] VkPipeline GetVkPipeline() const { return vkPipeline; }
};
