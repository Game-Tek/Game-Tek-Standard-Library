#include "GAL/Vulkan/VulkanPipelines.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include <GAL/ext/shaderc/shaderc.hpp>

#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
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

GAL::VulkanGraphicsPipeline::VulkanGraphicsPipeline(const CreateInfo& createInfo)
{
	//  VERTEX INPUT STATE

	GTSL::Array<GTSL::uint8, MAX_VERTEX_ELEMENTS> offsets;
	
	GTSL::Array<VkVertexInputBindingDescription, 4> vk_vertex_input_binding_descriptions(1);
	vk_vertex_input_binding_descriptions[0].binding = 0;
	vk_vertex_input_binding_descriptions[0].stride = GetVertexSizeAndOffsetsToMembers(createInfo.VertexDescriptor, offsets);
	vk_vertex_input_binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	GTSL::Array<VkVertexInputAttributeDescription, MAX_VERTEX_ELEMENTS> vk_vertex_input_attribute_descriptions(createInfo.VertexDescriptor.ElementCount());
	for (GTSL::uint8 i = 0; i < vk_vertex_input_attribute_descriptions.GetLength(); ++i)
	{
		vk_vertex_input_attribute_descriptions[i].binding = 0;
		vk_vertex_input_attribute_descriptions[i].location = i;
		vk_vertex_input_attribute_descriptions[i].format = VkFormat(createInfo.VertexDescriptor[i]);
		vk_vertex_input_attribute_descriptions[i].offset = offsets[i];
	}

	VkPipelineVertexInputStateCreateInfo vk_pipeline_vertex_input_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	vk_pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = vk_vertex_input_binding_descriptions.GetLength();
	vk_pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = vk_vertex_input_binding_descriptions.begin();
	vk_pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = vk_vertex_input_attribute_descriptions.GetLength();
	vk_pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = vk_vertex_input_attribute_descriptions.begin();

	//  INPUT ASSEMBLY STATE
	VkPipelineInputAssemblyStateCreateInfo vk_pipeline_input_assembly_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	vk_pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vk_pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

	//  TESSELLATION STATE
	VkPipelineTessellationStateCreateInfo vk_pipeline_tessellation_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO	};

	//  VIEWPORT STATE
	VkViewport vk_viewport;
	vk_viewport.x = 0;
	vk_viewport.y = 0;
	auto window_extent = createInfo.SurfaceExtent;
	vk_viewport.width = window_extent.Width;
	vk_viewport.height = window_extent.Height;
	vk_viewport.minDepth = 0.0f;
	vk_viewport.maxDepth = 1.0f;

	VkRect2D vk_scissor = { { 0, 0 }, { window_extent.Width, window_extent.Height } };

	VkPipelineViewportStateCreateInfo vk_pipeline_viewport_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	vk_pipeline_viewport_state_create_info.viewportCount = 1;
	vk_pipeline_viewport_state_create_info.pViewports = &vk_viewport;
	vk_pipeline_viewport_state_create_info.scissorCount = 1;
	vk_pipeline_viewport_state_create_info.pScissors = &vk_scissor;

	//  RASTERIZATION STATE
	VkPipelineRasterizationStateCreateInfo vk_pipeline_rasterization_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	vk_pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
	vk_pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
	vk_pipeline_rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	vk_pipeline_rasterization_state_create_info.lineWidth = 1.0f;
	vk_pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	vk_pipeline_rasterization_state_create_info.cullMode = CullModeToVkCullModeFlagBits(createInfo.PipelineDescriptor.CullMode);
	vk_pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
	vk_pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.0f; // Optional
	vk_pipeline_rasterization_state_create_info.depthBiasClamp = 0.0f; // Optional
	vk_pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.0f; // Optional

	//  MULTISAMPLE STATE
	VkPipelineMultisampleStateCreateInfo vk_pipeline_multisample_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	vk_pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
	vk_pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	vk_pipeline_multisample_state_create_info.minSampleShading = 1.0f; // Optional
	vk_pipeline_multisample_state_create_info.pSampleMask = nullptr; // Optional
	vk_pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE; // Optional
	vk_pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE; // Optional

	//  DEPTH STENCIL STATE
	VkPipelineDepthStencilStateCreateInfo vk_pipeline_depthstencil_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	vk_pipeline_depthstencil_state_create_info.depthTestEnable = VK_TRUE;
	vk_pipeline_depthstencil_state_create_info.depthWriteEnable = VK_TRUE;
	vk_pipeline_depthstencil_state_create_info.depthCompareOp = CompareOperationToVkCompareOp(createInfo.PipelineDescriptor.DepthCompareOperation);
	vk_pipeline_depthstencil_state_create_info.depthBoundsTestEnable = VK_FALSE;
	vk_pipeline_depthstencil_state_create_info.minDepthBounds = 0.0f; // Optional
	vk_pipeline_depthstencil_state_create_info.maxDepthBounds = 1.0f; // Optional
	vk_pipeline_depthstencil_state_create_info.stencilTestEnable = VK_FALSE;
	vk_pipeline_depthstencil_state_create_info.front = {}; // Optional
	vk_pipeline_depthstencil_state_create_info.back = {}; // Optional

	//  COLOR BLEND STATE
	VkPipelineColorBlendAttachmentState vk_pipeline_colorblend_attachment_state{};
	vk_pipeline_colorblend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT	| VK_COLOR_COMPONENT_A_BIT;
	vk_pipeline_colorblend_attachment_state.blendEnable = createInfo.PipelineDescriptor.BlendEnable;
	vk_pipeline_colorblend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	vk_pipeline_colorblend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	vk_pipeline_colorblend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	vk_pipeline_colorblend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	vk_pipeline_colorblend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	vk_pipeline_colorblend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo vk_pipeline_colorblend_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	vk_pipeline_colorblend_state_create_info.logicOpEnable = VK_FALSE;
	vk_pipeline_colorblend_state_create_info.logicOp = VK_LOGIC_OP_COPY; // Optional
	vk_pipeline_colorblend_state_create_info.attachmentCount = 1;
	vk_pipeline_colorblend_state_create_info.pAttachments = &vk_pipeline_colorblend_attachment_state;
	vk_pipeline_colorblend_state_create_info.blendConstants[0] = 0.0f; // Optional
	vk_pipeline_colorblend_state_create_info.blendConstants[1] = 0.0f; // Optional
	vk_pipeline_colorblend_state_create_info.blendConstants[2] = 0.0f; // Optional
	vk_pipeline_colorblend_state_create_info.blendConstants[3] = 0.0f; // Optional

	//  DYNAMIC STATE
	VkPipelineDynamicStateCreateInfo vk_pipeline_dynamic_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	GTSL::Array<VkDynamicState, 1> vk_dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT };
	vk_pipeline_dynamic_state_create_info.dynamicStateCount = vk_dynamic_states.GetCapacity();
	vk_pipeline_dynamic_state_create_info.pDynamicStates = vk_dynamic_states.begin();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	GTSL::Array<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGES> vk_pipeline_shader_stage_create_infos(createInfo.Stages.ElementCount());

	for (GTSL::uint8 i = 0; i < createInfo.Stages.ElementCount(); ++i)
	{		
		vk_pipeline_shader_stage_create_infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk_pipeline_shader_stage_create_infos[i].pNext = nullptr;
		vk_pipeline_shader_stage_create_infos[i].flags = 0;
		vk_pipeline_shader_stage_create_infos[i].stage = VulkanShaderTypeToVkShaderStageFlagBits(createInfo.Stages[i].Type);
		vk_pipeline_shader_stage_create_infos[i].pName = "main";
		vk_pipeline_shader_stage_create_infos[i].module = createInfo.Stages[i].Shader->GetVkShaderModule();
		vk_pipeline_shader_stage_create_infos[i].pSpecializationInfo = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkPipelineLayoutCreateInfo vk_pipeline_layout_create_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

	VkPushConstantRange vk_push_constant_range{};
	if (createInfo.PushConstant)
	{
		vk_push_constant_range.size = static_cast<GTSL::uint32>(createInfo.PushConstant->Size);
		vk_push_constant_range.offset = 0;
		vk_push_constant_range.stageFlags = ShaderTypeToVkShaderStageFlagBits(createInfo.PushConstant->Stage);

		vk_pipeline_layout_create_info.pushConstantRangeCount = 1;
		vk_pipeline_layout_create_info.pPushConstantRanges = &vk_push_constant_range;
	}
	else
	{
		vk_pipeline_layout_create_info.pushConstantRangeCount = 0;
		vk_pipeline_layout_create_info.pPushConstantRanges = nullptr;
	}

	GTSL::Array<VkDescriptorSetLayout, 16> vk_descriptor_set_layouts(createInfo.BindingsSetLayouts.ElementCount());
	for (auto& e : vk_descriptor_set_layouts)
	{
		e = createInfo.BindingsSetLayouts[RangeForIndex(e, vk_descriptor_set_layouts)].GetVkDescriptorSetLayout();
	}

	vk_pipeline_layout_create_info.setLayoutCount = vk_descriptor_set_layouts.GetLength();
	//What sets this pipeline layout uses.
	vk_pipeline_layout_create_info.pSetLayouts = vk_descriptor_set_layouts.begin();

	VK_CHECK(vkCreatePipelineLayout(createInfo.RenderDevice->GetVkDevice(), &vk_pipeline_layout_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipelineLayout));
	{
		SET_NAME(pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, createInfo);
	}
	
	VkGraphicsPipelineCreateInfo vk_graphics_pipeline_create_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	vk_graphics_pipeline_create_info.flags = createInfo.IsInheritable ? VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT : 0;
	vk_graphics_pipeline_create_info.flags |= createInfo.ParentPipeline ? VK_PIPELINE_CREATE_DERIVATIVE_BIT : 0;
	vk_graphics_pipeline_create_info.stageCount = vk_pipeline_shader_stage_create_infos.GetLength();
	vk_graphics_pipeline_create_info.pStages = vk_pipeline_shader_stage_create_infos.begin();
	vk_graphics_pipeline_create_info.pVertexInputState = &vk_pipeline_vertex_input_state_create_info;
	vk_graphics_pipeline_create_info.pInputAssemblyState = &vk_pipeline_input_assembly_state_create_info;
	vk_graphics_pipeline_create_info.pTessellationState = &vk_pipeline_tessellation_state_create_info;
	vk_graphics_pipeline_create_info.pViewportState = &vk_pipeline_viewport_state_create_info;
	vk_graphics_pipeline_create_info.pRasterizationState = &vk_pipeline_rasterization_state_create_info;
	vk_graphics_pipeline_create_info.pMultisampleState = &vk_pipeline_multisample_state_create_info;
	vk_graphics_pipeline_create_info.pDepthStencilState = &vk_pipeline_depthstencil_state_create_info;
	vk_graphics_pipeline_create_info.pColorBlendState = &vk_pipeline_colorblend_state_create_info;
	vk_graphics_pipeline_create_info.pDynamicState = &vk_pipeline_dynamic_state_create_info;
	vk_graphics_pipeline_create_info.layout = pipelineLayout;
	vk_graphics_pipeline_create_info.renderPass = createInfo.RenderPass->GetVkRenderPass();
	vk_graphics_pipeline_create_info.subpass = 0;
	vk_graphics_pipeline_create_info.basePipelineHandle = createInfo.ParentPipeline ? createInfo.ParentPipeline->pipeline : nullptr; // Optional
	vk_graphics_pipeline_create_info.basePipelineIndex = createInfo.ParentPipeline ? 0 : -1;

	if(createInfo.PipelineCache)
	{
		VK_CHECK(vkCreateGraphicsPipelines(createInfo.RenderDevice->GetVkDevice(), 
			createInfo.PipelineCache->GetVkPipelineCache(), 1, &vk_graphics_pipeline_create_info,
			createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline));

		SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);
		return;
	}
	
	VK_CHECK(vkCreateGraphicsPipelines(createInfo.RenderDevice->GetVkDevice(), nullptr, 1, &vk_graphics_pipeline_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline));
	SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);
	
	{
		SET_NAME(pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, createInfo);
	}
}

void GAL::VulkanGraphicsPipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	vkDestroyPipelineLayout(renderDevice->GetVkDevice(), pipelineLayout, renderDevice->GetVkAllocationCallbacks());

	debugClear(pipeline); debugClear(pipelineLayout);
}

void GAL::VulkanComputePipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}

GAL::VulkanRaytracingPipeline::VulkanRaytracingPipeline(const CreateInfo& createInfo)
{
	VkRayTracingPipelineCreateInfoKHR vk_ray_tracing_pipeline_create_info{VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
	vk_ray_tracing_pipeline_create_info.maxRecursionDepth = createInfo.MaxRecursionDepth;

	GTSL::Array<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGES> vk_pipeline_shader_stage_create_infos(createInfo.Stages.ElementCount());

	for (GTSL::uint64 i = 0; i < createInfo.Stages.ElementCount(); ++i)
	{
		vk_pipeline_shader_stage_create_infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vk_pipeline_shader_stage_create_infos[i].pNext = nullptr;
		vk_pipeline_shader_stage_create_infos[i].flags = 0;
		vk_pipeline_shader_stage_create_infos[i].stage = static_cast<VkShaderStageFlagBits>(createInfo.Stages[i].Type);
		vk_pipeline_shader_stage_create_infos[i].pName = "main";
		vk_pipeline_shader_stage_create_infos[i].module = createInfo.Stages[i].Shader->GetVkShaderModule();
		vk_pipeline_shader_stage_create_infos[i].pSpecializationInfo = nullptr;
	}
	
	vk_ray_tracing_pipeline_create_info.stageCount = vk_pipeline_shader_stage_create_infos.GetLength();
	vk_ray_tracing_pipeline_create_info.pStages = vk_pipeline_shader_stage_create_infos.begin();

	{
		VkPipelineLayoutCreateInfo vk_pipeline_layout_create_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

		GTSL::Array<VkDescriptorSetLayout, 16> vk_descriptor_set_layouts(createInfo.BindingsSetLayouts.ElementCount());
		for (auto& e : vk_descriptor_set_layouts)
		{
			e = createInfo.BindingsSetLayouts[RangeForIndex(e, vk_descriptor_set_layouts)].GetVkDescriptorSetLayout();
		}
		vk_pipeline_layout_create_info.setLayoutCount = vk_descriptor_set_layouts.GetLength();
		vk_pipeline_layout_create_info.pSetLayouts = vk_descriptor_set_layouts.begin();

		vkCreatePipelineLayout(createInfo.RenderDevice->GetVkDevice(), &vk_pipeline_layout_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipelineLayout);
		SET_NAME(pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, createInfo);
	}
	
	vk_ray_tracing_pipeline_create_info.layout = pipelineLayout;

	if (createInfo.ParentPipeline)
	{
		vk_ray_tracing_pipeline_create_info.basePipelineIndex = createInfo.ParentPipeline ? 0 : -1;
		vk_ray_tracing_pipeline_create_info.basePipelineHandle = createInfo.ParentPipeline->GetVkPipeline();
	}
	else
	{
		vk_ray_tracing_pipeline_create_info.basePipelineIndex = -1;
		vk_ray_tracing_pipeline_create_info.basePipelineHandle = nullptr;
	}
	
	createInfo.RenderDevice->vkCreateRayTracingPipelinesKHR(createInfo.RenderDevice->GetVkDevice(), nullptr, 1, &vk_ray_tracing_pipeline_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline);
	SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);

	{
		SET_NAME(pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, createInfo);
	}
}
