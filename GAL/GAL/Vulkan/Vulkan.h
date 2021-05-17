#pragma once

#if (_WIN64)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#define VK_NO_PROTOTYPES
#include <GAL/ext/vulkan/vulkan.h>


#if (_DEBUG)
#define COMBINE2(x, y) x ## y
#define COMBINE(x, y) COMBINE2(x, y)
#define VK_CHECK(func) VkResult COMBINE(result, __LINE__) = VK_ERROR_UNKNOWN; COMBINE(result, __LINE__) = func; { if ((COMBINE(result, __LINE__)) != VK_SUCCESS) { __debugbreak(); } }
#else
#define VK_CHECK(func) func;
#endif // (_DEBUG)

#include "GAL/RenderCore.h"

#include <GTSL/Extent.h>

#include "GTSL/Flags.h"
#include "GTSL/Range.h"

#undef OPAQUE

namespace GAL
{	
	using VulkanHandle = void*;
	
	inline VkAttachmentLoadOp ToVkAttachmentLoadOp(const Operations operations) {
		switch (operations)
		{
		case Operations::UNDEFINED: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case Operations::DO: return VK_ATTACHMENT_LOAD_OP_LOAD;
		case Operations::CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
		default: return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
		}
	}

	inline VkAttachmentStoreOp ToVkAttachmentStoreOp(const Operations operations) {
		switch (operations)
		{
		case Operations::UNDEFINED: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case Operations::DO: return VK_ATTACHMENT_STORE_OP_STORE;
		case Operations::CLEAR: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		default: return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
		}
	}
	
	inline VkAccessFlags ToVulkan(const AccessType access, const PipelineStage pipelineStage) {
		VkAccessFlags vkAccessFlags = 0;
		if(access) {
			TranslateMask(PipelineStages::TRANSFER, VK_ACCESS_TRANSFER_WRITE_BIT, pipelineStage, vkAccessFlags);
		} else {
			TranslateMask(PipelineStages::TRANSFER, VK_ACCESS_TRANSFER_READ_BIT, pipelineStage, vkAccessFlags);
		}
		return vkAccessFlags;
	}
	
	inline VkAccessFlags ToVulkan(const AccessType access, const PipelineStage pipelineStage, const FormatDescriptor formatDescriptor) {
		VkAccessFlags vkAccessFlags = 0;
		
		if(access) {
			if(pipelineStage & PipelineStages::TRANSFER) {
				vkAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
			}
		} else {
			if(pipelineStage & PipelineStages::TRANSFER) {
				vkAccessFlags |= VK_ACCESS_TRANSFER_READ_BIT;
			}
		}

		return vkAccessFlags;
	}
	
	inline VkAccessFlags ToVulkan(const AccessType access, const FormatDescriptor formatDescriptor) {
		if(access) { //write
			switch (formatDescriptor.Type) {
			case TextureType::COLOR: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			case TextureType::DEPTH: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}
		} else { //read
			switch (formatDescriptor.Type) {
			case TextureType::COLOR: return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
			case TextureType::DEPTH: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			}
		}
	}

	inline VkQueueFlags ToVulkan(const QueueType queueType) {
		VkQueueFlags vkQueueFlags = 0;
		TranslateMask(QueueTypes::GRAPHICS, VK_QUEUE_GRAPHICS_BIT, queueType, vkQueueFlags);
		TranslateMask(QueueTypes::COMPUTE, VK_QUEUE_COMPUTE_BIT, queueType, vkQueueFlags);
		TranslateMask(QueueTypes::TRANSFER, VK_QUEUE_TRANSFER_BIT, queueType, vkQueueFlags);
		return vkQueueFlags;
	}
		
	inline VkImageTiling ToVulkan(const Tiling tiling) {
		switch (tiling) {
		case Tiling::OPTIMAL: return VK_IMAGE_TILING_OPTIMAL;
		case Tiling::LINEAR: return VK_IMAGE_TILING_LINEAR;
		default: VK_IMAGE_TILING_MAX_ENUM;
		}
	}
	
	inline VkMemoryAllocateFlags ToVulkan(const AllocationFlag allocationFlag) {
		VkMemoryAllocateFlags vkMemoryAllocateFlags = 0;
		TranslateMask(AllocationFlags::DEVICE_ADDRESS, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, allocationFlag, vkMemoryAllocateFlags);
		TranslateMask(AllocationFlags::DEVICE_ADDRESS_CAPTURE_REPLAY, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_CAPTURE_REPLAY_BIT, allocationFlag, vkMemoryAllocateFlags);
		return vkMemoryAllocateFlags;
	}
	
	inline VkAccessFlags ToVulkanBufferAccessFlags(const AccessFlag accessFlag) {
		VkAccessFlags vkAccessFlags = 0;
		TranslateMask(AccessFlags::INDIRECT_COMMAND_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::INDEX_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::VERTEX_ATTRIBUTE_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::UNIFORM_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::INPUT_ATTACHMENT_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::SHADER_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::SHADER_WRITE, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::ATTACHMENT_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::ATTACHMENT_WRITE, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::TRANSFER_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::TRANSFER_WRITE, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::HOST_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::HOST_WRITE, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::MEMORY_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::MEMORY_WRITE, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::ACCELERATION_STRUCTURE_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::ACCELERATION_STRUCTURE_WRITE, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		TranslateMask(AccessFlags::SHADING_RATE_IMAGE_READ, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, accessFlag, vkAccessFlags);
		return vkAccessFlags;
	}
	
	inline VkImageLayout ToVulkan(const TextureLayout layout, FormatDescriptor formatDescriptor) {
		switch (layout) {
		case TextureLayout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
		case TextureLayout::GENERAL: return VK_IMAGE_LAYOUT_GENERAL;
		case TextureLayout::ATTACHMENT:
		{
			switch (formatDescriptor.Type) {
			case TextureType::COLOR: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case TextureType::DEPTH: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			default: return VK_IMAGE_LAYOUT_MAX_ENUM;
			}
		}
		case TextureLayout::SHADER_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case TextureLayout::TRANSFER_SOURCE: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case TextureLayout::TRANSFER_DESTINATION: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case TextureLayout::PREINITIALIZED: return VK_IMAGE_LAYOUT_PREINITIALIZED;
		case TextureLayout::PRESENTATION: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		default: return VK_IMAGE_LAYOUT_MAX_ENUM;
		}
	}
	
	inline VkFormat ToVulkan(const Format format) {
		switch (format) {
		case Format::RGBA_I8: return VK_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA_F16: return VK_FORMAT_R16G16B16A16_SFLOAT;
		case Format::BGRA_I8: return VK_FORMAT_B8G8R8A8_UNORM;
		case Format::RGBA_F32: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Format::F32: return VK_FORMAT_D32_SFLOAT;
		}

		GAL_DEBUG_BREAK;
	}

	inline VkImageAspectFlags TextureAspectToVkImageAspectFlags(const TextureType textureType) {
		switch (textureType) {
		case TextureType::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
		case TextureType::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
		}
	}
	
	enum class VulkanAccelerationStructureType : GTSL::uint8 {
		TOP_LEVEL = 0, BOTTOM_LEVEL = 1
	};
	
	enum class VulkanShaderGroupType {
		GENERAL, TRIANGLES, PROCEDURAL
	};
	
	using VulkanGeometryInstanceFlag = GTSL::Flags<GTSL::uint32, struct VulkanGeometryInstanceFlagTag>;
	
	namespace VulkanGeometryInstanceFlags {
		static constexpr VulkanGeometryInstanceFlag DISABLE_CULLING = 0x00000001, FRONT_COUNTERCLOCKWISE = 0x00000002, OPAQUE = 0x00000004, NOT_OPAQUE = 0x00000008;
	}
	
	using VulkanBindingFlag = GTSL::Flags<GTSL::uint32, struct VulkanBindingFlagTag>;
	
	namespace VulkanBindingFlags {
		static constexpr VulkanBindingFlag UPDATE_AFTER_BIND = 0x00000001, UPDATE_UNUSED_WHILE_PENDING = 0x00000002, PARTIALLY_BOUND = 0x00000004, VARIABLE_DESCRIPTOR_COUNT = 0x00000008;
	}

	inline VkAccelerationStructureCreateFlagsKHR ToVulkan(const AccelerationStructureFlag accelerationStructureFlag) {
		VkAccelerationStructureCreateFlagsKHR vkAccelerationStructureCreateFlagsKhr = 0;
		TranslateMask(AccelerationStructureFlags::ALLOW_COMPACTION, VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR, accelerationStructureFlag, vkAccelerationStructureCreateFlagsKhr);
		TranslateMask(AccelerationStructureFlags::ALLOW_UPDATE, VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR, accelerationStructureFlag, vkAccelerationStructureCreateFlagsKhr);
		TranslateMask(AccelerationStructureFlags::LOW_MEMORY, VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR, accelerationStructureFlag, vkAccelerationStructureCreateFlagsKhr);
		TranslateMask(AccelerationStructureFlags::PREFER_FAST_BUILD, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR, accelerationStructureFlag, vkAccelerationStructureCreateFlagsKhr);
		TranslateMask(AccelerationStructureFlags::PREFER_FAST_TRACE, VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR, accelerationStructureFlag, vkAccelerationStructureCreateFlagsKhr);
		return vkAccelerationStructureCreateFlagsKhr;
		
	}
	
	inline VkPipelineStageFlags ToVulkan(const PipelineStage pipelineStage) {
		VkPipelineStageFlags vkPipelineStageFlags = 0;
		TranslateMask(PipelineStages::TOP_OF_PIPE, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::DRAW_INDIRECT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::VERTEX_INPUT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::VERTEX_SHADER, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::TESSELLATION_CONTROL_SHADER, VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::TESSELLATION_EVALUATION_SHADER, VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::GEOMETRY_SHADER, VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::FRAGMENT_SHADER, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::EARLY_FRAGMENT_TESTS, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::LATE_FRAGMENT_TESTS, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::COLOR_ATTACHMENT_OUTPUT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::COMPUTE_SHADER, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::TRANSFER, VK_PIPELINE_STAGE_TRANSFER_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::BOTTOM_OF_PIPE, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::HOST, VK_PIPELINE_STAGE_HOST_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::ALL_GRAPHICS, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::ALL_COMMANDS, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::RAY_TRACING_SHADER, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::ACCELERATION_STRUCTURE_BUILD, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::TASK_SHADER, VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV, pipelineStage, vkPipelineStageFlags);
		TranslateMask(PipelineStages::MESH_SHADER, VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV, pipelineStage, vkPipelineStageFlags);
		return vkPipelineStageFlags;
	}
	
	inline VkDescriptorType ToVulkan(const BindingType bindingType) {
		switch (bindingType) {

		case BindingType::SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
		case BindingType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case BindingType::SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case BindingType::STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case BindingType::UNIFORM_TEXEL_BUFFER: break;
		case BindingType::STORAGE_TEXEL_BUFFER: break;
		case BindingType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case BindingType::STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case BindingType::UNIFORM_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case BindingType::STORAGE_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case BindingType::INPUT_ATTACHMENT: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case BindingType::ACCELERATION_STRUCTURE: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}
	
	inline VkShaderStageFlagBits ToVulkan(const ShaderType shaderType) {
		switch (shaderType) {
		case ShaderType::VERTEX_SHADER: return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderType::TESSELLATION_CONTROL_SHADER: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderType::TESSELLATION_EVALUATION_SHADER: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ShaderType::GEOMETRY_SHADER: return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderType::FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderType::COMPUTE_SHADER: return VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderType::RAY_GEN: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case ShaderType::CLOSEST_HIT: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case ShaderType::ANY_HIT: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case ShaderType::INTERSECTION: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case ShaderType::MISS: return VK_SHADER_STAGE_MISS_BIT_KHR;
		case ShaderType::CALLABLE: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		default: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}
	}
	
	inline VkExtent2D ToVulkan(const GTSL::Extent2D extent) {
		return { extent.Width, extent.Height };
	}

	inline VkExtent3D ToVulkan(const GTSL::Extent3D extent) {
		return { extent.Width, extent.Height, extent.Depth };
	}

	inline VkRayTracingShaderGroupTypeKHR ToVulkan(const ShaderGroupType type) {
		switch (type)
		{
		case ShaderGroupType::GENERAL: return VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		case ShaderGroupType::TRIANGLES: return VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		case ShaderGroupType::PROCEDURAL: return VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
		default: return VK_RAY_TRACING_SHADER_GROUP_TYPE_MAX_ENUM_KHR;
		}
	}
	
	inline VkPresentModeKHR ToVulkan(const PresentModes presentModes)
	{
		switch (presentModes) {
		case GAL::PresentModes::FIFO: return VK_PRESENT_MODE_FIFO_KHR;
		case GAL::PresentModes::SWAP: return VK_PRESENT_MODE_MAILBOX_KHR;
		default: return VK_PRESENT_MODE_MAX_ENUM_KHR;
		}
	}
	
	inline GTSL::uint32 ImageTypeToVkImageAspectFlagBits(const TextureType imageType) {
		switch (imageType) {
		case TextureType::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
		case TextureType::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
		default: return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
		}
	}

	inline VkBufferUsageFlags ToVulkan(const BufferUse bufferUses) {
		VkBufferUsageFlags vkBufferUsageFlags = 0;
		TranslateMask(BufferUses::STORAGE, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::TRANSFER_SOURCE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::TRANSFER_DESTINATION, VK_BUFFER_USAGE_TRANSFER_DST_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::ADDRESS, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::ACCELERATION_STRUCTURE, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::UNIFORM, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::INDEX, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::SHADER_BINDING_TABLE, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR, bufferUses, vkBufferUsageFlags);
		TranslateMask(BufferUses::BUILD_INPUT_READ, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, bufferUses, vkBufferUsageFlags);
		return vkBufferUsageFlags;
	}
	
	inline VkFormat ToVulkan(const ShaderDataType shaderDataTypes) {
		switch (shaderDataTypes) {
		case ShaderDataType::FLOAT: return VK_FORMAT_R32_SFLOAT;
		case ShaderDataType::FLOAT2: return VK_FORMAT_R32G32_SFLOAT;
		case ShaderDataType::FLOAT3: return VK_FORMAT_R32G32B32_SFLOAT;
		case ShaderDataType::FLOAT4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case ShaderDataType::INT: return VK_FORMAT_R32_SINT;
		case ShaderDataType::INT2: return VK_FORMAT_R32G32_SINT;
		case ShaderDataType::INT3: return VK_FORMAT_R32G32B32_SINT;
		case ShaderDataType::INT4: return VK_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::BOOL: return VK_FORMAT_R32_SINT;
		default: return VK_FORMAT_MAX_ENUM;
		}
	}

	inline VkQueryType ToVulkan(const QueryType queryType) {
		switch (queryType) {
		case QueryType::COMPACT_ACCELERATION_STRUCTURE_SIZE: return VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
		default: return VK_QUERY_TYPE_MAX_ENUM;
		}
	}
	
	inline VkDescriptorType UniformTypeToVkDescriptorType(const BindingType uniformType) {
		switch (uniformType) {
		case BindingType::SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
		case BindingType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case BindingType::SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case BindingType::STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case BindingType::UNIFORM_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		case BindingType::STORAGE_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		case BindingType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case BindingType::STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case BindingType::UNIFORM_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case BindingType::STORAGE_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case BindingType::INPUT_ATTACHMENT: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	};

	inline VkAccelerationStructureBuildTypeKHR ToVulkan(const BuildDevice buildDevice) {
		switch (buildDevice) {
		case BuildDevice::GPU: return VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;
		case BuildDevice::HOST: return VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR;
		case BuildDevice::GPU_OR_HOST: return VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_OR_DEVICE_KHR;
		default: return VK_ACCELERATION_STRUCTURE_BUILD_TYPE_MAX_ENUM_KHR;
		}
	}

	
	inline VkCullModeFlagBits ToVulkan(const CullMode cullMode) {
		switch (cullMode) {
		case CullMode::CULL_BACK: return VK_CULL_MODE_BACK_BIT;
		case CullMode::CULL_FRONT: return VK_CULL_MODE_FRONT_BIT;
		case CullMode::CULL_NONE: return VK_CULL_MODE_NONE;
		default: return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
		}
	}	
	
	inline VkFrontFace ToVulkan(const WindingOrder windingOrder) {
		switch (windingOrder) {
		case WindingOrder::CLOCKWISE: return VK_FRONT_FACE_CLOCKWISE;
		case WindingOrder::COUNTER_CLOCKWISE: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		default: return VK_FRONT_FACE_MAX_ENUM;
		}
	}

	inline VkCompareOp ToVulkan(const CompareOperation compareOperation) {
		switch (compareOperation) {
		case CompareOperation::NEVER: return VK_COMPARE_OP_NEVER;
		case CompareOperation::LESS: return VK_COMPARE_OP_LESS;
		case CompareOperation::EQUAL: return VK_COMPARE_OP_EQUAL;
		case CompareOperation::LESS_OR_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
		case CompareOperation::GREATER: return VK_COMPARE_OP_GREATER;
		case CompareOperation::NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
		case CompareOperation::GREATER_OR_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case CompareOperation::ALWAYS: return VK_COMPARE_OP_ALWAYS;
		default: return VK_COMPARE_OP_MAX_ENUM;
		}
	}

	inline VkImageType ToVulkanType(const GTSL::Extent3D extent) {
		if(extent.Height != 1) {
			if (extent.Depth != 1) { return VK_IMAGE_TYPE_3D; }
			return VK_IMAGE_TYPE_2D;
		}

		return VK_IMAGE_TYPE_1D;
	}

	inline VkImageViewType ToVkImageViewType(const GTSL::Extent3D extent) {
		if(extent.Height != 1) {
			if (extent.Depth != 1) { return VK_IMAGE_VIEW_TYPE_3D; }
			return VK_IMAGE_VIEW_TYPE_2D;
		}

		return VK_IMAGE_VIEW_TYPE_1D;
	}

	inline VkImageAspectFlags ToVulkan(const TextureType textureType) {
		switch (textureType) {
		case TextureType::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
		case TextureType::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
		}
	}

	inline VkIndexType ToVulkan(const IndexType indexType) {
		switch (indexType) {
		case IndexType::UINT8: return VK_INDEX_TYPE_UINT8_EXT;
		case IndexType::UINT16: return VK_INDEX_TYPE_UINT16;
		case IndexType::UINT32: return VK_INDEX_TYPE_UINT32;
		}
	}

	inline VkImageUsageFlags ToVulkan(const TextureUse uses, const FormatDescriptor formatDescriptor) {
		VkImageUsageFlags vkUsage = 0;
		
		if(uses & TextureUses::ATTACHMENT) {
			switch (formatDescriptor.Type) {
			case TextureType::COLOR: vkUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; break;
			case TextureType::DEPTH: vkUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; break;
			}
		}
		
		TranslateMask(TextureUses::INPUT_ATTACHMENT, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, uses, vkUsage);
		TranslateMask(TextureUses::SAMPLE, VK_IMAGE_USAGE_SAMPLED_BIT, uses, vkUsage);
		TranslateMask(TextureUses::STORAGE, VK_IMAGE_USAGE_STORAGE_BIT, uses, vkUsage);
		TranslateMask(TextureUses::TRANSFER_DESTINATION, VK_IMAGE_USAGE_TRANSFER_DST_BIT, uses, vkUsage);
		TranslateMask(TextureUses::TRANSFER_SOURCE, VK_IMAGE_USAGE_TRANSFER_SRC_BIT, uses, vkUsage);
		TranslateMask(TextureUses::TRANSIENT_ATTACHMENT, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, uses, vkUsage);

		return vkUsage;
	}

	inline VkStencilOp ToVulkan(const StencilCompareOperation stencilCompareOperation) {
		switch (stencilCompareOperation) {
		case StencilCompareOperation::KEEP: return VK_STENCIL_OP_KEEP;
		case StencilCompareOperation::ZERO: return VK_STENCIL_OP_ZERO;
		case StencilCompareOperation::REPLACE: return VK_STENCIL_OP_REPLACE;
		case StencilCompareOperation::INCREMENT_AND_CLAMP: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case StencilCompareOperation::DECREMENT_AND_CLAMP: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		case StencilCompareOperation::INVERT: return VK_STENCIL_OP_INVERT;
		case StencilCompareOperation::INCREMENT_AND_WRAP: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case StencilCompareOperation::DECREMENT_AND_WRAP: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		default: return VK_STENCIL_OP_MAX_ENUM;
		}
	}
	
	inline VkShaderStageFlags ToVulkan(const ShaderStage shaderStage) {
		VkShaderStageFlags vkShaderStageFlags = 0;
		TranslateMask(ShaderStages::VERTEX, VK_SHADER_STAGE_VERTEX_BIT, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::FRAGMENT, VK_SHADER_STAGE_FRAGMENT_BIT, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::COMPUTE, VK_SHADER_STAGE_COMPUTE_BIT, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::RAY_GEN, VK_SHADER_STAGE_RAYGEN_BIT_KHR, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::CLOSEST_HIT, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::ANY_HIT, VK_SHADER_STAGE_ANY_HIT_BIT_KHR, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::MISS, VK_SHADER_STAGE_MISS_BIT_KHR, shaderStage, vkShaderStageFlags);
		TranslateMask(ShaderStages::CALLABLE, VK_SHADER_STAGE_CALLABLE_BIT_KHR, shaderStage, vkShaderStageFlags);
		return vkShaderStageFlags;
	}
	
	inline VkDescriptorBindingFlags ToVulkan(const BindingFlag bindingFlag) {
		VkDescriptorBindingFlags vkDescriptorBindingFlags = 0;
		TranslateMask(BindingFlags::PARTIALLY_BOUND, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT, bindingFlag, vkDescriptorBindingFlags);
		return vkDescriptorBindingFlags;
	}

	inline VkGeometryFlagsKHR ToVulkan(const GeometryFlag geometryFlag) {
		VkGeometryFlagsKHR vkGeometryFlagsKhr = 0;
		TranslateMask(GeometryFlags::OPAQUE, VK_GEOMETRY_OPAQUE_BIT_KHR, geometryFlag, vkGeometryFlagsKhr);
		return vkGeometryFlagsKhr;
	}

	inline VkColorSpaceKHR ToVulkan(const ColorSpace colorSpace) {
		switch (colorSpace) {
		case ColorSpace::SRGB: return VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		default: return VK_COLOR_SPACE_MAX_ENUM_KHR;
		}
	}
	
	//TO GAL

	inline PresentModes ToGAL(const VkPresentModeKHR presentModes) {
		switch (presentModes) {
		case VK_PRESENT_MODE_FIFO_KHR: return PresentModes::FIFO;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return PresentModes::FIFO;
		case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR: return PresentModes::SWAP;
		case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: return PresentModes::SWAP;
		case VK_PRESENT_MODE_MAX_ENUM_KHR: return PresentModes::SWAP;
		case VK_PRESENT_MODE_IMMEDIATE_KHR: return PresentModes::FIFO;
		case VK_PRESENT_MODE_MAILBOX_KHR:  return PresentModes::SWAP;
		default: return PresentModes::SWAP;
		}
	}

	inline MemoryType ToGAL(const VkMemoryPropertyFlags memoryPropertyFlags) {
		MemoryType memoryType = 0;
		TranslateMask(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, MemoryTypes::GPU, memoryPropertyFlags, memoryType);
		TranslateMask(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, MemoryTypes::HOST_VISIBLE, memoryPropertyFlags, memoryType);
		TranslateMask(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, MemoryTypes::HOST_COHERENT, memoryPropertyFlags, memoryType);
		TranslateMask(VK_MEMORY_PROPERTY_HOST_CACHED_BIT, MemoryTypes::HOST_CACHED, memoryPropertyFlags, memoryType);
		TranslateMask(VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, 0, memoryPropertyFlags, memoryType);
		return memoryType;
	}

	inline Format ToGAL(const VkFormat format) {
		switch (format) {
		case VK_FORMAT_R8G8B8A8_UNORM: return Format::RGBA_I8;
		case VK_FORMAT_B8G8R8A8_UNORM: return Format::BGRA_I8;
		}
	}
	
	inline ColorSpace ToGAL(const VkColorSpaceKHR colorSpace) {
		switch (colorSpace) {
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return ColorSpace::SRGB;
		case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT: break;
		case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT: break;
		case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT: break;
		case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT: break;
		case VK_COLOR_SPACE_BT709_LINEAR_EXT: break;
		case VK_COLOR_SPACE_BT709_NONLINEAR_EXT: break;
		case VK_COLOR_SPACE_BT2020_LINEAR_EXT: break;
		case VK_COLOR_SPACE_HDR10_ST2084_EXT: break;
		case VK_COLOR_SPACE_DOLBYVISION_EXT: break;
		case VK_COLOR_SPACE_HDR10_HLG_EXT: break;
		case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT: break;
		case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT: break;
		case VK_COLOR_SPACE_PASS_THROUGH_EXT: break;
		case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT: break;
		case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD: break;
		case VK_COLOR_SPACE_MAX_ENUM_KHR: break;
		}
	}
}