#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include <GAL/ext/shaderc/shaderc.hpp>

//#include <GAL/ext/glslang/public/ShaderLang.h>
//#include <GAL/ext/glslang/SPIRV/GlslangToSpv.h>

#include "GTSL/Bitman.h"
#include "GTSL/Buffer.hpp"

bool GAL::CompileShader(GTSL::Range<const GTSL::UTF8*> code, GTSL::Range<const GTSL::UTF8*> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::BufferInterface result, GTSL::BufferInterface stringResult)
{
	shaderc_shader_kind shaderc_stage;

	switch (shaderType)
	{
	case ShaderType::VERTEX_SHADER: shaderc_stage = shaderc_vertex_shader;	break;
	case ShaderType::TESSELLATION_CONTROL_SHADER: shaderc_stage = shaderc_tess_control_shader;	break;
	case ShaderType::TESSELLATION_EVALUATION_SHADER: shaderc_stage = shaderc_tess_evaluation_shader; break;
	case ShaderType::GEOMETRY_SHADER: shaderc_stage = shaderc_geometry_shader;	break;
	case ShaderType::FRAGMENT_SHADER: shaderc_stage = shaderc_fragment_shader;	break;
	case ShaderType::COMPUTE_SHADER: shaderc_stage = shaderc_compute_shader; break;
	case ShaderType::RAY_GEN: shaderc_stage = shaderc_raygen_shader; break;
	case ShaderType::CLOSEST_HIT: shaderc_stage = shaderc_closesthit_shader; break;
	case ShaderType::ANY_HIT: shaderc_stage = shaderc_anyhit_shader; break;
	case ShaderType::INTERSECTION: shaderc_stage = shaderc_intersection_shader; break;
	case ShaderType::MISS: shaderc_stage = shaderc_miss_shader; break;
	case ShaderType::CALLABLE: shaderc_stage = shaderc_callable_shader; break;
	default: GAL_DEBUG_BREAK;
	}

	const shaderc::Compiler shaderc_compiler;
	shaderc::CompileOptions shaderc_compile_options;
	shaderc_compile_options.SetTargetSpirv(shaderc_spirv_version_1_5);
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
		auto errorString = shaderc_module.GetErrorMessage();
		stringResult.CopyBytes(errorString.size() + 1, reinterpret_cast<const GTSL::byte*>(errorString.c_str()));
		*(stringResult.end() - 1) = '\0';
		return false;
	}

	result.CopyBytes((shaderc_module.end() - shaderc_module.begin()) * sizeof(GTSL::uint32), reinterpret_cast<const GTSL::byte*>(shaderc_module.begin()));
	
	return true;
}

//static bool glslLangInitialized = false;

//bool GAL::VulkanShader::CompileShader(GTSL::Range<const GTSL::UTF8> code, GTSL::Range<const GTSL::UTF8> shaderName, ShaderType shaderType, ShaderLanguage shaderLanguage, GTSL::Buffer& result, GTSL::Buffer& stringError)
//{
//	EShLanguage shaderc_stage;
//	
//	switch (ShaderTypeToVkShaderStageFlagBits(shaderType))
//	{
//	case VK_SHADER_STAGE_VERTEX_BIT: shaderc_stage = EShLangVertex;	break;
//	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT: shaderc_stage = EShLangTessControl;	break;
//	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: shaderc_stage = EShLangTessEvaluation; break;
//	case VK_SHADER_STAGE_GEOMETRY_BIT: shaderc_stage = EShLangGeometry;	break;
//	case VK_SHADER_STAGE_FRAGMENT_BIT: shaderc_stage = EShLangFragment;	break;
//	case VK_SHADER_STAGE_COMPUTE_BIT: shaderc_stage = EShLangCompute; break;
//	case VK_SHADER_STAGE_RAYGEN_BIT_KHR: shaderc_stage = EShLangRayGen; break;
//	case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR: shaderc_stage = EShLangClosestHit; break;
//	case VK_SHADER_STAGE_ANY_HIT_BIT_KHR: shaderc_stage = EShLangAnyHit; break;
//	case VK_SHADER_STAGE_MISS_BIT_KHR: shaderc_stage = EShLangMiss; break;
//	case VK_SHADER_STAGE_INTERSECTION_BIT_KHR: shaderc_stage = EShLangIntersect; break;
//	case VK_SHADER_STAGE_CALLABLE_BIT_KHR: shaderc_stage = EShLangCallable; break;
//	default: GAL_DEBUG_BREAK;
//	}
//
//	const TBuiltInResource DefaultTBuiltInResource{};
//
//	if(glslLangInitialized)
//	{
//		glslang::InitializeProcess();
//		glslLangInitialized = true;
//	}
//
//	glslang::TShader shader(shaderc_stage);
//	GTSL::int32 length = code.ElementCount();
//	auto* string = code.begin();
//	shader.setStringsWithLengths(&string, &length, 1);
//
//	int ClientInputSemanticsVersion = 460; // maps to, say, #define VULKAN 100
//	glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_2;
//	glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_5;
//
//	shader.setEnvInput(glslang::EShSourceGlsl, shaderc_stage, glslang::EShClientVulkan, ClientInputSemanticsVersion);
//	shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
//	shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);
//
//	TBuiltInResource resources;
//	resources = DefaultTBuiltInResource;
//	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
//
//	const GTSL::uint32 defaultVersion = 460;
//
//	if (!shader.parse(&resources, defaultVersion, false, messages))
//	{
//		const char* textBegin = "GLSL Parsing Failed for: ";
//		stringError.WriteBytes(26, reinterpret_cast<const GTSL::byte*>(textBegin));
//		stringError.WriteBytes(shaderName.ElementCount(), reinterpret_cast<const GTSL::byte*>(shaderName.begin()));
//		stringError.WriteBytes(GTSL::StringLength(shader.getInfoLog()), reinterpret_cast<const GTSL::byte*>(shader.getInfoLog()));
//		stringError.WriteBytes(GTSL::StringLength(shader.getInfoDebugLog()), reinterpret_cast<const GTSL::byte*>(shader.getInfoDebugLog()));
//
//		return false;
//	}
//
//	glslang::TProgram Program;
//	Program.addShader(&shader);
//
//	if (!Program.link(messages))
//	{
//		const char* textBegin = "GLSL Linking Failed for: ";
//		stringError.WriteBytes(26, reinterpret_cast<const GTSL::byte*>(textBegin));
//		stringError.WriteBytes(shaderName.ElementCount(), reinterpret_cast<const GTSL::byte*>(shaderName.begin()));
//		stringError.WriteBytes(GTSL::StringLength(shader.getInfoLog()), reinterpret_cast<const GTSL::byte*>(shader.getInfoLog()));
//		stringError.WriteBytes(GTSL::StringLength(shader.getInfoDebugLog()), reinterpret_cast<const GTSL::byte*>(shader.getInfoDebugLog()));
//
//		return false;
//	}
//
//	std::vector<GTSL::uint32> spirv;
//	spv::SpvBuildLogger logger;
//	glslang::SpvOptions spvOptions;
//	glslang::GlslangToSpv(*Program.getIntermediate(shaderc_stage), spirv, &logger, &spvOptions);
//
//	result.WriteBytes(spirv.size() * sizeof(GTSL::uint32), reinterpret_cast<const GTSL::byte*>(spirv.data()));
//
//	return true;
//}

GAL::VulkanPipelineCache::VulkanPipelineCache(const CreateInfo& createInfo)
{
	VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
	GTSL::SetBitAs(0, createInfo.ExternallySync, vkPipelineCacheCreateInfo.flags);
	vkPipelineCacheCreateInfo.initialDataSize = createInfo.Data.Bytes();
	vkPipelineCacheCreateInfo.pInitialData = createInfo.Data.begin();

	VK_CHECK(vkCreatePipelineCache(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vkPipelineCacheCreateInfo, static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &pipelineCache))
}

GAL::VulkanPipelineCache::VulkanPipelineCache(const CreateFromMultipleInfo& createInfo)
{
	VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
	
	VK_CHECK(vkCreatePipelineCache(createInfo.RenderDevice->GetVkDevice(), &vkPipelineCacheCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipelineCache))
	
	vkMergePipelineCaches(createInfo.RenderDevice->GetVkDevice(), pipelineCache, static_cast<GTSL::uint32>(createInfo.Caches.ElementCount()), reinterpret_cast<const VkPipelineCache*>(createInfo.Caches.begin()));
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

void GAL::VulkanPipelineCache::GetCache(const VulkanRenderDevice* renderDevice, GTSL::BufferInterface buffer) const
{
	GTSL::uint64 data_size;
	VK_CHECK(vkGetPipelineCacheData(renderDevice->GetVkDevice(), pipelineCache, &data_size, buffer.begin()));
	buffer.Resize(data_size);
}

void GAL::VulkanShader::Initialize(const VulkanRenderDevice* renderDevice, GTSL::Range<const GTSL::byte*> blob)
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	shaderModuleCreateInfo.codeSize = blob.Bytes();
	shaderModuleCreateInfo.pCode = reinterpret_cast<const GTSL::uint32*>(blob.begin());
	vkCreateShaderModule(renderDevice->GetVkDevice(), &shaderModuleCreateInfo, renderDevice->GetVkAllocationCallbacks(), &vkShaderModule);
}

void GAL::VulkanShader::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyShaderModule(renderDevice->GetVkDevice(), vkShaderModule, renderDevice->GetVkAllocationCallbacks());
	debugClear(vkShaderModule);
}

void GAL::VulkanPipelineLayout::Initialize(const CreateInfo& createInfo)
{
	VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

	VkPushConstantRange vkPushConstantRange;
	if (createInfo.PushConstant)
	{
		vkPushConstantRange.size = static_cast<GTSL::uint32>(createInfo.PushConstant->Size);
		vkPushConstantRange.offset = 0;
		vkPushConstantRange.stageFlags = createInfo.PushConstant->ShaderStages;

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

void GAL::VulkanPipeline::InitializeRasterPipeline(const VulkanRenderDevice* renderDevice, const GTSL::Range<const PipelineStateBlock*> pipelineStates, GTSL::Range<const ShaderInfo*> stages, const VulkanPipelineLayout pipelineLayout, const VulkanPipelineCache pipelineCache)
{
	VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	vkGraphicsPipelineCreateInfo.pTessellationState = nullptr; vkGraphicsPipelineCreateInfo.pColorBlendState = nullptr;
	vkGraphicsPipelineCreateInfo.pVertexInputState = nullptr; vkGraphicsPipelineCreateInfo.pInputAssemblyState = nullptr;
	vkGraphicsPipelineCreateInfo.pViewportState = nullptr; vkGraphicsPipelineCreateInfo.pRasterizationState = nullptr;
	vkGraphicsPipelineCreateInfo.pDepthStencilState = nullptr; vkGraphicsPipelineCreateInfo.pMultisampleState = nullptr;
	vkGraphicsPipelineCreateInfo.layout = pipelineLayout.GetVkPipelineLayout();
	
	GTSL::Buffer<GTSL::StackAllocator<8192>> buffer(8192, 8, GTSL::StackAllocator<8192>());

	for (GTSL::uint8 i = 0; i < pipelineStates.ElementCount(); ++i)
	{
		auto& pipelineState = pipelineStates[i];

		switch (pipelineState.Type)
		{
		case PipelineStateBlock::StateType::VIEWPORT_STATE:
		{
			auto* vkViewport = buffer.AllocateStructure<VkViewport>();
			vkViewport->x = 0;
			vkViewport->y = 1;
			vkViewport->width = 1.0f;
			vkViewport->height = 1.0f;
			vkViewport->minDepth = 0.0f;
			vkViewport->maxDepth = 1.0f;

			auto* vkScissor = buffer.AllocateStructure<VkRect2D>();
			*vkScissor = { { 0, 0 }, { 1, 1 } };

			auto* pointer = buffer.AllocateStructure<VkPipelineViewportStateCreateInfo>();

			VkPipelineViewportStateCreateInfo& vkPipelineViewportStateCreateInfo = *pointer;
			vkPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vkPipelineViewportStateCreateInfo.pNext = nullptr;
			vkPipelineViewportStateCreateInfo.viewportCount = pipelineState.Block.Viewport.ViewportCount;
			vkPipelineViewportStateCreateInfo.pViewports = vkViewport;
			vkPipelineViewportStateCreateInfo.scissorCount = 1;
			vkPipelineViewportStateCreateInfo.pScissors = vkScissor;

			vkGraphicsPipelineCreateInfo.pViewportState = pointer;

			break;
		}
		case PipelineStateBlock::StateType::RASTER_STATE:
		{
			VkPipelineRasterizationStateCreateInfo* pointer = buffer.AllocateStructure<VkPipelineRasterizationStateCreateInfo>();

			VkPipelineRasterizationStateCreateInfo& vkPipelineRasterizationStateCreateInfo = *pointer;
			vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			vkPipelineRasterizationStateCreateInfo.pNext = nullptr;
			vkPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_TRUE;
			vkPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
			vkPipelineRasterizationStateCreateInfo.frontFace = ToVulkan(pipelineState.Block.Raster.WindingOrder);
			vkPipelineRasterizationStateCreateInfo.cullMode = ToVulkan(pipelineState.Block.Raster.CullMode);
			vkPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
			vkPipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
			vkPipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
			vkPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional

			vkGraphicsPipelineCreateInfo.pRasterizationState = pointer;

			break;
		}
		case PipelineStateBlock::StateType::DEPTH_STATE:
		{
			auto* pointer = buffer.AllocateStructure<VkPipelineDepthStencilStateCreateInfo>();

			auto& vkPipelineDepthStencilStateCreateInfo = *pointer;
			vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			vkPipelineDepthStencilStateCreateInfo.pNext = nullptr;
			vkPipelineDepthStencilStateCreateInfo.depthTestEnable = true;
			vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = true;
			vkPipelineDepthStencilStateCreateInfo.depthCompareOp = ToVulkan(pipelineState.Block.Depth.CompareOperation);
			vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
			vkPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
			vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
			vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = false;

			vkGraphicsPipelineCreateInfo.pDepthStencilState = pointer;

			break;
		}
		case PipelineStateBlock::StateType::COLOR_BLEND_STATE:
		{
			auto* pointer = buffer.AllocateStructure<VkPipelineColorBlendStateCreateInfo>();

			auto& vkPipelineColorblendStateCreateInfo = *pointer;
			vkPipelineColorblendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			vkPipelineColorblendStateCreateInfo.pNext = nullptr;
			vkPipelineColorblendStateCreateInfo.logicOpEnable = VK_FALSE;
			vkPipelineColorblendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
			vkPipelineColorblendStateCreateInfo.pAttachments = reinterpret_cast<const VkPipelineColorBlendAttachmentState*>(buffer.GetData() + buffer.GetLength());

			GTSL::uint8 attachmentCount = 0;
			for (; attachmentCount < pipelineState.Block.Context.Attachments.ElementCount(); ++attachmentCount) {
				auto* state = buffer.AllocateStructure<VkPipelineColorBlendAttachmentState>();
				state->blendEnable = pipelineState.Block.Context.Attachments[attachmentCount].BlendEnable;
				state->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				state->srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
				state->dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
				state->colorBlendOp = VK_BLEND_OP_ADD;
				state->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				state->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				state->alphaBlendOp = VK_BLEND_OP_ADD;
			}

			vkPipelineColorblendStateCreateInfo.attachmentCount = attachmentCount;
			vkPipelineColorblendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
			vkPipelineColorblendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
			vkPipelineColorblendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
			vkPipelineColorblendStateCreateInfo.blendConstants[3] = 0.0f; // Optional

			vkGraphicsPipelineCreateInfo.pColorBlendState = pointer;

			vkGraphicsPipelineCreateInfo.renderPass = static_cast<const VulkanRenderPass*>(pipelineState.Block.Context.RenderPass)->GetVkRenderPass();
			vkGraphicsPipelineCreateInfo.subpass = pipelineState.Block.Context.SubPassIndex;

			break;
		}
		case PipelineStateBlock::StateType::VERTEX_STATE:
		{
			auto* pointer = buffer.AllocateStructure<VkPipelineVertexInputStateCreateInfo>();
			auto* binding = buffer.AllocateStructure<VkVertexInputBindingDescription>();

			binding->binding = 0; binding->inputRate = VK_VERTEX_INPUT_RATE_VERTEX; binding->stride = 0;

			pointer->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; pointer->pNext = nullptr;
			pointer->vertexBindingDescriptionCount = 1;
			pointer->pVertexBindingDescriptions = binding;
			pointer->vertexAttributeDescriptionCount = 0;
			pointer->pVertexAttributeDescriptions = reinterpret_cast<const VkVertexInputAttributeDescription*>(buffer.GetData() + buffer.GetLength());;

			GTSL::uint16 offset = 0;

			for (GTSL::uint8 i = 0; i < pipelineState.Block.Vertex.VertexDescriptor.ElementCount(); ++i) {
				auto size = ShaderDataTypesSize(pipelineState.Block.Vertex.VertexDescriptor[i].Type);

				if (pipelineState.Block.Vertex.VertexDescriptor[i].Enabled) {
					auto& vertex = *buffer.AllocateStructure<VkVertexInputAttributeDescription>();
					vertex.binding = 0; vertex.location = i; vertex.format = ToVulkan(pipelineState.Block.Vertex.VertexDescriptor[i].Type);
					vertex.offset = offset;
					offset += size;
					binding->stride += size;
				}

				++pointer->vertexAttributeDescriptionCount;
			}

			vkGraphicsPipelineCreateInfo.pVertexInputState = pointer;

			auto* inputAssemblyState = buffer.AllocateStructure<VkPipelineInputAssemblyStateCreateInfo>();
			inputAssemblyState->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; inputAssemblyState->pNext = nullptr;
			inputAssemblyState->flags = 0; inputAssemblyState->primitiveRestartEnable = false; inputAssemblyState->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			vkGraphicsPipelineCreateInfo.pInputAssemblyState = inputAssemblyState;

			break;
		}
		default:;
		}
	}

	VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	GTSL::Array<VkDynamicState, 4> vkDynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	vkPipelineDynamicStateCreateInfo.dynamicStateCount = vkDynamicStates.GetLength();
	vkPipelineDynamicStateCreateInfo.pDynamicStates = vkDynamicStates.begin();

	GTSL::Array<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGES> vkPipelineShaderStageCreateInfos;

	for (GTSL::uint8 i = 0; i < stages.ElementCount(); ++i)
	{
		auto& stage = vkPipelineShaderStageCreateInfos.EmplaceBack();

		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.pNext = nullptr;
		stage.flags = 0;
		stage.stage = static_cast<VkShaderStageFlagBits>(stages[i].Type);
		stage.pName = "main";
		stage.module = stages[i].Shader.GetVkShaderModule();
		stage.pSpecializationInfo = nullptr;
	}

	vkGraphicsPipelineCreateInfo.stageCount = vkPipelineShaderStageCreateInfos.GetLength();
	vkGraphicsPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfos.begin();
	vkGraphicsPipelineCreateInfo.pDynamicState = &vkPipelineDynamicStateCreateInfo;
	vkGraphicsPipelineCreateInfo.basePipelineIndex = -1;
	vkGraphicsPipelineCreateInfo.basePipelineHandle = nullptr;

	VK_CHECK(vkCreateGraphicsPipelines(renderDevice->GetVkDevice(), pipelineCache.GetVkPipelineCache(), 1, &vkGraphicsPipelineCreateInfo, renderDevice->GetVkAllocationCallbacks(), &pipeline));
	//SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);
}

void GAL::VulkanPipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}

void GAL::VulkanComputePipeline::Initialize(const CreateInfo& createInfo)
{
	VkComputePipelineCreateInfo computePipelineCreateInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	computePipelineCreateInfo.basePipelineIndex = -1;
	computePipelineCreateInfo.layout = createInfo.PipelineLayout.GetVkPipelineLayout();
	computePipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computePipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computePipelineCreateInfo.stage.pName = "main";
	computePipelineCreateInfo.stage.module = createInfo.ShaderInfo.Shader.GetVkShaderModule();
	
	vkCreateComputePipelines(createInfo.RenderDevice->GetVkDevice(), createInfo.PipelineCache.GetVkPipelineCache(), 1, &computePipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline);
}

static_assert(sizeof(GAL::VulkanRayTracingPipeline::Group) == sizeof(VkRayTracingShaderGroupCreateInfoKHR), "Size doesn't match!");

void GAL::VulkanRayTracingPipeline::Initialize(const CreateInfo& createInfo)
{
	VkRayTracingPipelineCreateInfoKHR vkRayTracingPipelineCreateInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
	vkRayTracingPipelineCreateInfo.basePipelineIndex = -1;
	vkRayTracingPipelineCreateInfo.maxPipelineRayRecursionDepth = createInfo.MaxRecursionDepth;

	GTSL::Array<VkPipelineShaderStageCreateInfo, 32> vkPipelineShaderStageCreateInfos;

	for (GTSL::uint32 i = 0; i < static_cast<GTSL::uint32>(createInfo.Stages.ElementCount()); ++i)
	{
		auto& stageCreateInfo = vkPipelineShaderStageCreateInfos.EmplaceBack();
		stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageCreateInfo.pNext = nullptr;
		stageCreateInfo.flags = 0;
		stageCreateInfo.stage = static_cast<VkShaderStageFlagBits>(createInfo.Stages[i].Type);
		stageCreateInfo.pName = "main";
		stageCreateInfo.pSpecializationInfo = nullptr;
		stageCreateInfo.module = createInfo.Stages[i].Shader.GetVkShaderModule();
	}

	for(auto& e : createInfo.Groups)
	{
		reinterpret_cast<VkRayTracingShaderGroupCreateInfoKHR&>(e).sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		reinterpret_cast<VkRayTracingShaderGroupCreateInfoKHR&>(e).pNext = nullptr;
		reinterpret_cast<VkRayTracingShaderGroupCreateInfoKHR&>(e).pShaderGroupCaptureReplayHandle = nullptr;
	}
	
	vkRayTracingPipelineCreateInfo.stageCount = vkPipelineShaderStageCreateInfos.GetLength();
	vkRayTracingPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfos.begin();
	
	vkRayTracingPipelineCreateInfo.layout = createInfo.PipelineLayout.GetVkPipelineLayout();

	vkRayTracingPipelineCreateInfo.groupCount = createInfo.Groups.ElementCount();
	vkRayTracingPipelineCreateInfo.pGroups = reinterpret_cast<const VkRayTracingShaderGroupCreateInfoKHR*>(createInfo.Groups.begin());
	
	createInfo.RenderDevice->vkCreateRayTracingPipelinesKHR(createInfo.RenderDevice->GetVkDevice(), nullptr, createInfo.PipelineCache.GetVkPipelineCache(), 1, &vkRayTracingPipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline);
	SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo)
}

void GAL::VulkanRayTracingPipeline::GetShaderGroupHandles(VulkanRenderDevice* renderDevice, GTSL::uint32 firstGroup, GTSL::uint32 groupCount, GTSL::Range<GTSL::byte*> buffer)
{
	renderDevice->vkGetRayTracingShaderGroupHandlesKHR(renderDevice->GetVkDevice(), pipeline, firstGroup, groupCount, buffer.Bytes(), buffer.begin());
}

