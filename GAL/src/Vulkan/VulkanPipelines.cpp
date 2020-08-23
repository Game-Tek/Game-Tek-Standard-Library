#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include <GAL/ext/shaderc/shaderc.hpp>

#include "GTSL/Bitman.h"
#include "GTSL/Buffer.h"
#include "GTSL/String.hpp"

GAL::VulkanShader::VulkanShader(const CreateInfo& createInfo)
{
	VkShaderModuleCreateInfo vk_shader_module_create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	vk_shader_module_create_info.codeSize = createInfo.ShaderData.Bytes();
	vk_shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(createInfo.ShaderData.begin());

	VK_CHECK(vkCreateShaderModule(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_shader_module_create_info, static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &shaderModule));
}

void GAL::VulkanShader::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyShaderModule(renderDevice->GetVkDevice(), shaderModule, renderDevice->GetVkAllocationCallbacks());
	debugClear(shaderModule);
}

bool GAL::VulkanShader::CompileShader(GTSL::Ranger<const GTSL::UTF8> code, GTSL::Ranger<const GTSL::UTF8> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::Buffer& result, GTSL::Buffer& stringResult)
{
	shaderc_shader_kind shaderc_stage;

	switch (ShaderTypeToVkShaderStageFlagBits(shaderType))
	{
	case VK_SHADER_STAGE_VERTEX_BIT: shaderc_stage = shaderc_vertex_shader;	break;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT: shaderc_stage = shaderc_tess_control_shader;	break;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: shaderc_stage = shaderc_tess_evaluation_shader; break;
	case VK_SHADER_STAGE_GEOMETRY_BIT: shaderc_stage = shaderc_geometry_shader;	break;
	case VK_SHADER_STAGE_FRAGMENT_BIT: shaderc_stage = shaderc_fragment_shader;	break;
	case VK_SHADER_STAGE_COMPUTE_BIT: shaderc_stage = shaderc_compute_shader; break;
	default: GAL_DEBUG_BREAK;
	}

	const shaderc::Compiler shaderc_compiler;
	shaderc::CompileOptions shaderc_compile_options;
	shaderc_compile_options.SetTargetSpirv(shaderc_spirv_version_1_3);
	shaderc_compile_options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

	shaderc_source_language shaderc_source_language;
	switch (shaderLanguage)
	{
	case ShaderLanguage::GLSL: shaderc_source_language = shaderc_source_language_glsl; break;
	case ShaderLanguage::HLSL: shaderc_source_language = shaderc_source_language_hlsl; break;
	default: GAL_DEBUG_BREAK;
	}

	shaderc_compile_options.SetSourceLanguage(shaderc_source_language);
	shaderc_compile_options.SetOptimizationLevel(shaderc_optimization_level_performance);
	const auto shaderc_module = shaderc_compiler.CompileGlslToSpv(static_cast<const char*>(code.begin()), code.Bytes(), shaderc_stage, shaderName.begin(), shaderc_compile_options);

	if (shaderc_module.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		stringResult.WriteBytes(shaderc_module.GetErrorMessage().size(), reinterpret_cast<const GTSL::byte*>(shaderc_module.GetErrorMessage().c_str()));
		return false;
	}

	result.WriteBytes((shaderc_module.end() - shaderc_module.begin()) * sizeof(GTSL::uint32), reinterpret_cast<const GTSL::byte*>(shaderc_module.begin()));

	auto test = shaderc_module.end() - shaderc_module.begin();
	auto test2 = (shaderc_module.end() - shaderc_module.begin()) * sizeof(GTSL::uint32);
	
	return true;
}

GAL::VulkanPipelineCache::VulkanPipelineCache(const CreateInfo& createInfo)
{
	VkPipelineCacheCreateInfo vk_pipeline_cache_create_info{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
	vk_pipeline_cache_create_info.initialDataSize = createInfo.Data.Bytes();
	vk_pipeline_cache_create_info.pInitialData = createInfo.Data.begin();

	VK_CHECK(vkCreatePipelineCache(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_pipeline_cache_create_info,
		static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &pipelineCache));
}

void GAL::VulkanPipelineCache::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipelineCache(renderDevice->GetVkDevice(), pipelineCache, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipelineCache);
}

void GAL::VulkanPipelineCache::GetCacheSize(const VulkanRenderDevice* renderDevice, GTSL::uint32& size) const
{
	size_t data_size = 0;
	VK_CHECK(vkGetPipelineCacheData(renderDevice->GetVkDevice(), pipelineCache, &data_size, nullptr));
	size = static_cast<GTSL::uint32>(data_size);
}

void GAL::VulkanPipelineCache::GetCache(const VulkanRenderDevice* renderDevice, const GTSL::uint32 size, GTSL::Buffer& buffer) const
{
	GTSL::uint64 data_size = size;
	VK_CHECK(vkGetPipelineCacheData(renderDevice->GetVkDevice(), pipelineCache, &data_size, buffer.GetData()));
	buffer.Resize(data_size);
}

void GAL::VulkanPipelineLayout::Initialize(const CreateInfo& createInfo)
{
	VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

	VkPushConstantRange vkPushConstantRange;
	if (createInfo.PushConstant)
	{
		vkPushConstantRange.size = static_cast<GTSL::uint32>(createInfo.PushConstant->Size);
		vkPushConstantRange.offset = 0;
		vkPushConstantRange.stageFlags = ShaderTypeToVkShaderStageFlagBits(createInfo.PushConstant->Stage);

		vkPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		vkPipelineLayoutCreateInfo.pPushConstantRanges = &vkPushConstantRange;
	}
	else
	{
		vkPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		vkPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	}

	vkPipelineLayoutCreateInfo.setLayoutCount = static_cast<GTSL::uint32>(createInfo.BindingsSetLayouts.ElementCount());
	//What sets this pipeline layout uses.
	vkPipelineLayoutCreateInfo.pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout*>(createInfo.BindingsSetLayouts.begin());

	VK_CHECK(vkCreatePipelineLayout(createInfo.RenderDevice->GetVkDevice(), &vkPipelineLayoutCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipelineLayout));
	SET_NAME(pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, createInfo);
}

void GAL::VulkanPipelineLayout::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipelineLayout(renderDevice->GetVkDevice(), pipelineLayout, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipelineLayout);
}

GAL::VulkanRasterizationPipeline::VulkanRasterizationPipeline(const CreateInfo& createInfo)
{
	//  VERTEX INPUT STATE

	GTSL::Array<GTSL::uint8, MAX_VERTEX_ELEMENTS> offsets;
	
	GTSL::Array<VkVertexInputBindingDescription, 4> vkVertexInputBindingDescriptions(1);
	vkVertexInputBindingDescriptions[0].binding = 0;
	vkVertexInputBindingDescriptions[0].stride = GetVertexSizeAndOffsetsToMembers(createInfo.VertexDescriptor, offsets);
	vkVertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	GTSL::Array<VkVertexInputAttributeDescription, MAX_VERTEX_ELEMENTS> vkVertexInputAttributeDescriptions(static_cast<GTSL::uint32>(createInfo.VertexDescriptor.ElementCount()));
	for (GTSL::uint8 i = 0; i < vkVertexInputAttributeDescriptions.GetLength(); ++i)
	{
		vkVertexInputAttributeDescriptions[i].binding = 0;
		vkVertexInputAttributeDescriptions[i].location = i;
		vkVertexInputAttributeDescriptions[i].format = static_cast<VkFormat>(createInfo.VertexDescriptor[i]);
		vkVertexInputAttributeDescriptions[i].offset = offsets[i];
	}

	VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	vkPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vkVertexInputBindingDescriptions.GetLength();
	vkPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vkVertexInputBindingDescriptions.begin();
	vkPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vkVertexInputAttributeDescriptions.GetLength();
	vkPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vkVertexInputAttributeDescriptions.begin();

	//  INPUT ASSEMBLY STATE
	VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	vkPipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	//  TESSELLATION STATE
	VkPipelineTessellationStateCreateInfo vkPipelineTessellationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO	};

	//  VIEWPORT STATE
	VkViewport vkViewport;
	vkViewport.x = 0;
	vkViewport.y = 0;
	auto windowExtent = createInfo.SurfaceExtent;
	vkViewport.width = windowExtent.Width;
	vkViewport.height = windowExtent.Height;
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;

	VkRect2D vk_scissor = { { 0, 0 }, { windowExtent.Width, windowExtent.Height } };

	VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	vkPipelineViewportStateCreateInfo.viewportCount = 1;
	vkPipelineViewportStateCreateInfo.pViewports = &vkViewport;
	vkPipelineViewportStateCreateInfo.scissorCount = 1;
	vkPipelineViewportStateCreateInfo.pScissors = &vk_scissor;

	//  RASTERIZATION STATE
	VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	vkPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	vkPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
	vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	vkPipelineRasterizationStateCreateInfo.cullMode = CullModeToVkCullModeFlagBits(createInfo.PipelineDescriptor.CullMode);
	vkPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	vkPipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
	vkPipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
	vkPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional

	//  MULTISAMPLE STATE
	VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	vkPipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	vkPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	vkPipelineMultisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
	vkPipelineMultisampleStateCreateInfo.pSampleMask = nullptr; // Optional
	vkPipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	vkPipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

	//  DEPTH STENCIL STATE
	VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	vkPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	vkPipelineDepthStencilStateCreateInfo.depthCompareOp = CompareOperationToVkCompareOp(createInfo.PipelineDescriptor.DepthCompareOperation);
	vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	vkPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
	vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
	vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	vkPipelineDepthStencilStateCreateInfo.front = {}; // Optional
	vkPipelineDepthStencilStateCreateInfo.back = {}; // Optional

	//  COLOR BLEND STATE
	VkPipelineColorBlendAttachmentState vkPipelineColorblendAttachmentState;
	vkPipelineColorblendAttachmentState.blendEnable = createInfo.PipelineDescriptor.BlendEnable;
	vkPipelineColorblendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT	| VK_COLOR_COMPONENT_A_BIT;
	vkPipelineColorblendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	vkPipelineColorblendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	vkPipelineColorblendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	vkPipelineColorblendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	vkPipelineColorblendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	vkPipelineColorblendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo vkPipelineColorblendStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	vkPipelineColorblendStateCreateInfo.logicOpEnable = VK_FALSE;
	vkPipelineColorblendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	vkPipelineColorblendStateCreateInfo.attachmentCount = 1;
	vkPipelineColorblendStateCreateInfo.pAttachments = &vkPipelineColorblendAttachmentState;
	vkPipelineColorblendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
	vkPipelineColorblendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
	vkPipelineColorblendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
	vkPipelineColorblendStateCreateInfo.blendConstants[3] = 0.0f; // Optional

	//  DYNAMIC STATE
	VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	GTSL::Array<VkDynamicState, 4> vkDynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	vkPipelineDynamicStateCreateInfo.dynamicStateCount = vkDynamicStates.GetLength();
	vkPipelineDynamicStateCreateInfo.pDynamicStates = vkDynamicStates.begin();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	GTSL::Array<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGES> vkPipelineShaderStageCreateInfos(createInfo.Stages.ElementCount());

	for (GTSL::uint8 i = 0; i < createInfo.Stages.ElementCount(); ++i)
	{		
		vkPipelineShaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vkPipelineShaderStageCreateInfos[i].pNext = nullptr;
		vkPipelineShaderStageCreateInfos[i].flags = 0;
		vkPipelineShaderStageCreateInfos[i].stage = static_cast<VkShaderStageFlagBits>(createInfo.Stages[i].Type);
		vkPipelineShaderStageCreateInfos[i].pName = "main";
		vkPipelineShaderStageCreateInfos[i].module = createInfo.Stages[i].Shader->GetVkShaderModule();
		vkPipelineShaderStageCreateInfos[i].pSpecializationInfo = nullptr;
	}
	
	VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	GTSL::SetBitAs(1, createInfo.IsInheritable, vkGraphicsPipelineCreateInfo.flags);
	GTSL::SetBitAs(2, createInfo.ParentPipeline, vkGraphicsPipelineCreateInfo.flags);
	vkGraphicsPipelineCreateInfo.stageCount = vkPipelineShaderStageCreateInfos.GetLength();
	vkGraphicsPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfos.begin();
	vkGraphicsPipelineCreateInfo.pVertexInputState = &vkPipelineVertexInputStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pTessellationState = &vkPipelineTessellationStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pColorBlendState = &vkPipelineColorblendStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pDynamicState = &vkPipelineDynamicStateCreateInfo;
	vkGraphicsPipelineCreateInfo.layout = createInfo.PipelineLayout->GetVkPipelineLayout();
	vkGraphicsPipelineCreateInfo.renderPass = createInfo.RenderPass->GetVkRenderPass();
	vkGraphicsPipelineCreateInfo.subpass = 0;
	vkGraphicsPipelineCreateInfo.basePipelineHandle = createInfo.ParentPipeline ? createInfo.ParentPipeline->pipeline : nullptr; // Optional
	vkGraphicsPipelineCreateInfo.basePipelineIndex = createInfo.ParentPipeline ? 0 : -1;

	if(createInfo.PipelineCache)
	{
		VK_CHECK(vkCreateGraphicsPipelines(createInfo.RenderDevice->GetVkDevice(), createInfo.PipelineCache->GetVkPipelineCache(), 1, &vkGraphicsPipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline));

		SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);
		return;
	}
	
	VK_CHECK(vkCreateGraphicsPipelines(createInfo.RenderDevice->GetVkDevice(), nullptr, 1, &vkGraphicsPipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline));
	SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);
}

void GAL::VulkanRasterizationPipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}

void GAL::VulkanComputePipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}

static_assert(sizeof(GAL::VulkanRaytracingPipeline::Group) == sizeof(VkRayTracingShaderGroupCreateInfoKHR), "Size doesn't match!");

GAL::VulkanRaytracingPipeline::VulkanRaytracingPipeline(const CreateInfo& createInfo)
{
	VkRayTracingPipelineCreateInfoKHR vkRayTracingPipelineCreateInfo{VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
	vkRayTracingPipelineCreateInfo.maxRecursionDepth = createInfo.MaxRecursionDepth;

	GTSL::Array<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGES> vkPipelineShaderStageCreateInfos(static_cast<GTSL::uint32>(createInfo.Stages.ElementCount()));

	for (GTSL::uint32 i = 0; i < vkPipelineShaderStageCreateInfos.GetLength(); ++i)
	{
		vkPipelineShaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vkPipelineShaderStageCreateInfos[i].pNext = nullptr;
		vkPipelineShaderStageCreateInfos[i].flags = 0;
		vkPipelineShaderStageCreateInfos[i].stage = static_cast<VkShaderStageFlagBits>(createInfo.Stages[i].Type);
		vkPipelineShaderStageCreateInfos[i].pName = "main";
		vkPipelineShaderStageCreateInfos[i].module = createInfo.Stages[i].Shader->GetVkShaderModule();
		vkPipelineShaderStageCreateInfos[i].pSpecializationInfo = nullptr;
	}

	for(uint32_t i = 0; i < createInfo.Groups.ElementCount(); ++i)
	{
		reinterpret_cast<VkRayTracingShaderGroupCreateInfoKHR*>(createInfo.Groups.begin())->sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		reinterpret_cast<VkRayTracingShaderGroupCreateInfoKHR*>(createInfo.Groups.begin())->pNext = nullptr;
		reinterpret_cast<VkRayTracingShaderGroupCreateInfoKHR*>(createInfo.Groups.begin())->pShaderGroupCaptureReplayHandle = nullptr;
	}
	
	vkRayTracingPipelineCreateInfo.stageCount = vkPipelineShaderStageCreateInfos.GetLength();
	vkRayTracingPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfos.begin();
	
	vkRayTracingPipelineCreateInfo.layout = createInfo.PipelineLayout->GetVkPipelineLayout();

	vkRayTracingPipelineCreateInfo.groupCount = createInfo.Groups.ElementCount();
	vkRayTracingPipelineCreateInfo.pGroups = reinterpret_cast<const VkRayTracingShaderGroupCreateInfoKHR*>(createInfo.Groups.begin());
	vkRayTracingPipelineCreateInfo.libraries.sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
	vkRayTracingPipelineCreateInfo.pLibraryInterface;
	
	if (createInfo.ParentPipeline)
	{
		vkRayTracingPipelineCreateInfo.basePipelineIndex = createInfo.ParentPipeline ? 0 : -1;
		vkRayTracingPipelineCreateInfo.basePipelineHandle = createInfo.ParentPipeline->GetVkPipeline();
	}
	else
	{
		vkRayTracingPipelineCreateInfo.basePipelineIndex = -1;
		vkRayTracingPipelineCreateInfo.basePipelineHandle = nullptr;
	}
	
	createInfo.RenderDevice->vkCreateRayTracingPipelinesKHR(createInfo.RenderDevice->GetVkDevice(), nullptr, 1, &vkRayTracingPipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline);
	SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);
}

void GAL::VulkanRaytracingPipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}
