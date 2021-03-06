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
		stringResult.CopyBytes(shaderc_module.GetErrorMessage().size(), reinterpret_cast<const GTSL::byte*>(shaderc_module.GetErrorMessage().c_str()));
		return false;
	}

	result.CopyBytes((shaderc_module.end() - shaderc_module.begin()) * sizeof(GTSL::uint32), reinterpret_cast<const GTSL::byte*>(shaderc_module.begin()));

	auto test = shaderc_module.end() - shaderc_module.begin();
	auto test2 = (shaderc_module.end() - shaderc_module.begin()) * sizeof(GTSL::uint32);
	
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

VkStencilOp StencilCompareOperationToVkStencilCompareOp(const GAL::StencilCompareOperation stencilCompareOperation)
{
	switch (stencilCompareOperation)
	{
	case GAL::StencilCompareOperation::KEEP: return VK_STENCIL_OP_KEEP;
	case GAL::StencilCompareOperation::ZERO: return VK_STENCIL_OP_ZERO;
	case GAL::StencilCompareOperation::REPLACE: return VK_STENCIL_OP_REPLACE;
	case GAL::StencilCompareOperation::INCREMENT_AND_CLAMP: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case GAL::StencilCompareOperation::DECREMENT_AND_CLAMP: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case GAL::StencilCompareOperation::INVERT: return VK_STENCIL_OP_INVERT;
	case GAL::StencilCompareOperation::INCREMENT_AND_WRAP: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case GAL::StencilCompareOperation::DECREMENT_AND_WRAP: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
	default: return VK_STENCIL_OP_MAX_ENUM;
	}
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

	VkRect2D vkScissor = { { 0, 0 }, { windowExtent.Width, windowExtent.Height } };

	VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	vkPipelineViewportStateCreateInfo.viewportCount = 1;
	vkPipelineViewportStateCreateInfo.pViewports = &vkViewport;
	vkPipelineViewportStateCreateInfo.scissorCount = 1;
	vkPipelineViewportStateCreateInfo.pScissors = &vkScissor;

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
	vkPipelineDepthStencilStateCreateInfo.depthTestEnable = createInfo.PipelineDescriptor.DepthTest;
	vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = createInfo.PipelineDescriptor.DepthWrite;
	vkPipelineDepthStencilStateCreateInfo.depthCompareOp = CompareOperationToVkCompareOp(createInfo.PipelineDescriptor.DepthCompareOperation);
	vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	vkPipelineDepthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
	vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
	vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = createInfo.PipelineDescriptor.StencilTest;
	{
		vkPipelineDepthStencilStateCreateInfo.front.failOp = StencilCompareOperationToVkStencilCompareOp(createInfo.PipelineDescriptor.StencilOperations.Front.FailOperation);
		vkPipelineDepthStencilStateCreateInfo.front.passOp = StencilCompareOperationToVkStencilCompareOp(createInfo.PipelineDescriptor.StencilOperations.Front.PassOperation);
		vkPipelineDepthStencilStateCreateInfo.front.depthFailOp = StencilCompareOperationToVkStencilCompareOp(createInfo.PipelineDescriptor.StencilOperations.Front.DepthFailOperation);
		vkPipelineDepthStencilStateCreateInfo.front.compareOp = CompareOperationToVkCompareOp(createInfo.PipelineDescriptor.StencilOperations.Front.CompareOperation);
		vkPipelineDepthStencilStateCreateInfo.front.compareMask = createInfo.PipelineDescriptor.StencilOperations.Front.CompareMask;
		vkPipelineDepthStencilStateCreateInfo.front.writeMask = createInfo.PipelineDescriptor.StencilOperations.Front.WriteMask;
		vkPipelineDepthStencilStateCreateInfo.front.reference = createInfo.PipelineDescriptor.StencilOperations.Front.Reference;
	}
	{
		vkPipelineDepthStencilStateCreateInfo.back.failOp = StencilCompareOperationToVkStencilCompareOp(createInfo.PipelineDescriptor.StencilOperations.Back.FailOperation);
		vkPipelineDepthStencilStateCreateInfo.back.passOp = StencilCompareOperationToVkStencilCompareOp(createInfo.PipelineDescriptor.StencilOperations.Back.PassOperation);
		vkPipelineDepthStencilStateCreateInfo.back.depthFailOp = StencilCompareOperationToVkStencilCompareOp(createInfo.PipelineDescriptor.StencilOperations.Back.DepthFailOperation);
		vkPipelineDepthStencilStateCreateInfo.back.compareOp = CompareOperationToVkCompareOp(createInfo.PipelineDescriptor.StencilOperations.Back.CompareOperation);
		vkPipelineDepthStencilStateCreateInfo.back.compareMask = createInfo.PipelineDescriptor.StencilOperations.Back.CompareMask;
		vkPipelineDepthStencilStateCreateInfo.back.writeMask = createInfo.PipelineDescriptor.StencilOperations.Back.WriteMask;
		vkPipelineDepthStencilStateCreateInfo.back.reference = createInfo.PipelineDescriptor.StencilOperations.Back.Reference;
	}

	//  COLOR BLEND STATE

	GTSL::Array<VkPipelineColorBlendAttachmentState, 12> colorBlendAttachmentStates;
	for(GTSL::uint8 i = 0; i < createInfo.AttachmentCount; ++i)
	{
		VkPipelineColorBlendAttachmentState vkPipelineColorblendAttachmentState;
		vkPipelineColorblendAttachmentState.blendEnable = createInfo.PipelineDescriptor.BlendEnable;
		vkPipelineColorblendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		vkPipelineColorblendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		vkPipelineColorblendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		vkPipelineColorblendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		vkPipelineColorblendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vkPipelineColorblendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vkPipelineColorblendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentStates.EmplaceBack(vkPipelineColorblendAttachmentState);
	}

	VkPipelineColorBlendStateCreateInfo vkPipelineColorblendStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	vkPipelineColorblendStateCreateInfo.logicOpEnable = VK_FALSE;
	vkPipelineColorblendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	vkPipelineColorblendStateCreateInfo.attachmentCount = colorBlendAttachmentStates.GetLength();
	vkPipelineColorblendStateCreateInfo.pAttachments = colorBlendAttachmentStates.begin();
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
		vkPipelineShaderStageCreateInfos[i].pSpecializationInfo = nullptr;

		VkShaderModuleCreateInfo shaderModuleCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderModuleCreateInfo.codeSize = createInfo.Stages[i].Blob.Bytes();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const GTSL::uint32*>(createInfo.Stages[i].Blob.begin());
		vkCreateShaderModule(createInfo.RenderDevice->GetVkDevice(), &shaderModuleCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &vkPipelineShaderStageCreateInfos[i].module);
	}
	
	VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
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
	vkGraphicsPipelineCreateInfo.layout = createInfo.PipelineLayout.GetVkPipelineLayout();
	vkGraphicsPipelineCreateInfo.renderPass = createInfo.RenderPass.GetVkRenderPass();
	vkGraphicsPipelineCreateInfo.subpass = createInfo.SubPass;
	vkGraphicsPipelineCreateInfo.basePipelineIndex = -1;
	
	VK_CHECK(vkCreateGraphicsPipelines(createInfo.RenderDevice->GetVkDevice(), createInfo.PipelineCache.GetVkPipelineCache(), 1, &vkGraphicsPipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline));
	SET_NAME(pipeline, VK_OBJECT_TYPE_PIPELINE, createInfo);

	for (auto& e : vkPipelineShaderStageCreateInfos) { vkDestroyShaderModule(createInfo.RenderDevice->GetVkDevice(), e.module, createInfo.RenderDevice->GetVkAllocationCallbacks()); }
}

void GAL::VulkanRasterizationPipeline::Destroy(const VulkanRenderDevice* renderDevice)
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

	VkShaderModuleCreateInfo shaderModuleCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	shaderModuleCreateInfo.codeSize = createInfo.ShaderInfo.Blob.Bytes();
	shaderModuleCreateInfo.pCode = reinterpret_cast<const GTSL::uint32*>(createInfo.ShaderInfo.Blob.begin());
	vkCreateShaderModule(createInfo.RenderDevice->GetVkDevice(), &shaderModuleCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &computePipelineCreateInfo.stage.module);
	
	vkCreateComputePipelines(createInfo.RenderDevice->GetVkDevice(), createInfo.PipelineCache.GetVkPipelineCache(), 1, &computePipelineCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &pipeline);

	vkDestroyShaderModule(createInfo.RenderDevice->GetVkDevice(), computePipelineCreateInfo.stage.module, createInfo.RenderDevice->GetVkAllocationCallbacks());
}

void GAL::VulkanComputePipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}

static_assert(sizeof(GAL::VulkanRayTracingPipeline::Group) == sizeof(VkRayTracingShaderGroupCreateInfoKHR), "Size doesn't match!");

void GAL::VulkanRayTracingPipeline::Initialize(const CreateInfo& createInfo)
{
	VkRayTracingPipelineCreateInfoKHR vkRayTracingPipelineCreateInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
	vkRayTracingPipelineCreateInfo.basePipelineIndex = -1;
	vkRayTracingPipelineCreateInfo.maxPipelineRayRecursionDepth = createInfo.MaxRecursionDepth;

	GTSL::Array<VkPipelineShaderStageCreateInfo, 32> vkPipelineShaderStageCreateInfos(static_cast<GTSL::uint32>(createInfo.Stages.ElementCount()));

	for (GTSL::uint32 i = 0; i < vkPipelineShaderStageCreateInfos.GetLength(); ++i)
	{
		vkPipelineShaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vkPipelineShaderStageCreateInfos[i].pNext = nullptr;
		vkPipelineShaderStageCreateInfos[i].flags = 0;
		vkPipelineShaderStageCreateInfos[i].stage = static_cast<VkShaderStageFlagBits>(createInfo.Stages[i].Type);
		vkPipelineShaderStageCreateInfos[i].pName = "main";
		vkPipelineShaderStageCreateInfos[i].pSpecializationInfo = nullptr;

		VkShaderModuleCreateInfo shaderModuleCreateInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderModuleCreateInfo.codeSize = createInfo.Stages[i].Blob.Bytes();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const GTSL::uint32*>(createInfo.Stages[i].Blob.begin());
		vkCreateShaderModule(createInfo.RenderDevice->GetVkDevice(), &shaderModuleCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &vkPipelineShaderStageCreateInfos[i].module);
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

	for (auto& e : vkPipelineShaderStageCreateInfos) { vkDestroyShaderModule(createInfo.RenderDevice->GetVkDevice(), e.module, createInfo.RenderDevice->GetVkAllocationCallbacks()); }
}

void GAL::VulkanRayTracingPipeline::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyPipeline(renderDevice->GetVkDevice(), pipeline, renderDevice->GetVkAllocationCallbacks());
	debugClear(pipeline);
}

void GAL::VulkanRayTracingPipeline::GetShaderGroupHandles(VulkanRenderDevice* renderDevice, GTSL::uint32 firstGroup, GTSL::uint32 groupCount, GTSL::Range<GTSL::byte*> buffer)
{
	renderDevice->vkGetRayTracingShaderGroupHandlesKHR(renderDevice->GetVkDevice(), pipeline, firstGroup, groupCount, buffer.Bytes(), buffer.begin());
}

