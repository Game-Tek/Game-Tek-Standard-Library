#pragma once

#include <GAL/ext/vulkan/vulkan.h>

#if (_DEBUG)
#define COMBINE(x, y) x ## y
#define COMBINE2(x, y) COMBINE(x, y)
#define VK_CHECK(func) VkResult COMBINE2(result, __LINE__) = VK_RESULT_MAX_ENUM; COMBINE2(result, __LINE__) = func; { if ((COMBINE2(result, __LINE__)) != VK_SUCCESS) { __debugbreak(); } }
#else
#define VK_CHECK(func) func
#endif // (_DEBUG)

#include "GAL/RenderCore.h"

#include <GTSL/Extent.h>

#include "GTSL/Flags.h"

#define MAKE_VK_HANDLE(object) typedef struct object##_T* (object);
#undef OPAQUE

namespace GAL
{
	using VulkanDeviceAddress = GTSL::uint64;
	
	struct VulkanRenderInfo
	{
		const class VulkanRenderDevice* RenderDevice = nullptr;
	};
	
	inline VkAttachmentLoadOp RenderTargetLoadOperationsToVkAttachmentLoadOp(const RenderTargetLoadOperations renderTargetLoadOperations)
	{
		switch (renderTargetLoadOperations)
		{
		case RenderTargetLoadOperations::UNDEFINED: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case RenderTargetLoadOperations::LOAD: return VK_ATTACHMENT_LOAD_OP_LOAD;
		case RenderTargetLoadOperations::CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
		default: return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
		}
	}

	inline VkAttachmentStoreOp RenderTargetStoreOperationsToVkAttachmentStoreOp(const RenderTargetStoreOperations renderTargetStoreOperations)
	{
		switch (renderTargetStoreOperations)
		{
		case RenderTargetStoreOperations::UNDEFINED: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case RenderTargetStoreOperations::STORE: return VK_ATTACHMENT_STORE_OP_STORE;
		default: return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
		}
	}

	inline VkImageLayout ImageLayoutToVkImageLayout(const ImageLayout imageLayout)
	{
		switch (imageLayout)
		{
		case ImageLayout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
		case ImageLayout::SHADER_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case ImageLayout::GENERAL: return VK_IMAGE_LAYOUT_GENERAL;
		case ImageLayout::COLOR_ATTACHMENT: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case ImageLayout::DEPTH_STENCIL_ATTACHMENT: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::DEPTH_STENCIL_READ_ONLY: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::TRANSFER_SOURCE: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case ImageLayout::TRANSFER_DESTINATION: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case ImageLayout::PREINITIALIZED: return VK_IMAGE_LAYOUT_PREINITIALIZED;
		case ImageLayout::PRESENTATION: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		default: return VK_IMAGE_LAYOUT_MAX_ENUM;
		}
	}

	inline VkPipelineStageFlags PipelineStageToVkPipelineStageFlags(const GTSL::uint32 pipeline)
	{
		return static_cast<VkPipelineStageFlags>(pipeline);
	}

	inline VkAccessFlags AccessFlagsToVkAccessFlags(const AccessFlags accessFlags)
	{
		return static_cast<VkPipelineStageFlags>(accessFlags);
	}
	
	inline VkShaderStageFlagBits ShaderTypeToVkShaderStageFlagBits(const ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderType::VERTEX_SHADER: return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderType::TESSELLATION_CONTROL_SHADER: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderType::TESSELLATION_EVALUATION_SHADER: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ShaderType::GEOMETRY_SHADER: return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderType::FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderType::COMPUTE_SHADER: return VK_SHADER_STAGE_COMPUTE_BIT;
		default: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}
	}

	inline VkExtent2D Extent2DToVkExtent2D(const GTSL::Extent2D extent)
	{
		return { extent.Width, extent.Height };
	}

	inline VkExtent3D Extent3DToVkExtent3D(const GTSL::Extent3D extent)
	{
		return { extent.Width, extent.Height, extent.Depth };
	}

	inline VkImageViewType ImageDimensionsToVkImageViewType(const ImageDimensions imageDimensions)
	{
		switch (imageDimensions)
		{
		case ImageDimensions::IMAGE_1D: return VK_IMAGE_VIEW_TYPE_1D;
		case ImageDimensions::IMAGE_2D: return VK_IMAGE_VIEW_TYPE_2D;
		case ImageDimensions::IMAGE_3D: return VK_IMAGE_VIEW_TYPE_3D;
		default: return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
		}
	}

	inline VkImageType ImageDimensionsToVkImageType(const ImageDimensions imageDimensions)
	{
		switch (imageDimensions)
		{
		case ImageDimensions::IMAGE_1D: return VK_IMAGE_TYPE_1D;
		case ImageDimensions::IMAGE_2D: return VK_IMAGE_TYPE_2D;
		case ImageDimensions::IMAGE_3D: return VK_IMAGE_TYPE_3D;
		default: return VK_IMAGE_TYPE_MAX_ENUM;
		}
	}

	inline GTSL::uint32 ImageTypeToVkImageAspectFlagBits(const ImageType imageType)
	{
		switch (imageType)
		{
		case ImageType::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
		case ImageType::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
		case ImageType::STENCIL: return VK_IMAGE_ASPECT_STENCIL_BIT;
		case ImageType::DEPTH_STENCIL: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		default: return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
		}
	}

	inline VkFormat ShaderDataTypesToVkFormat(const ShaderDataType shaderDataTypes)
	{
		switch (shaderDataTypes)
		{
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

	inline VkDescriptorType UniformTypeToVkDescriptorType(const BindingType uniformType)
	{
		switch (uniformType)
		{
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

	inline VkCullModeFlagBits CullModeToVkCullModeFlagBits(const CullMode cullMode)
	{
		switch (cullMode)
		{
		case CullMode::CULL_BACK: return VK_CULL_MODE_BACK_BIT;
		case CullMode::CULL_FRONT: return VK_CULL_MODE_FRONT_BIT;
		case CullMode::CULL_NONE: return VK_CULL_MODE_NONE;
		default: return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
		}
	}

	inline VkCompareOp CompareOperationToVkCompareOp(const CompareOperation compareOperation)
	{
		switch (compareOperation)
		{
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
	
	enum class VulkanImageTiling : GTSL::uint32
	{
		OPTIMAL = 0,
		LINEAR = 1,
	};
	
	struct VulkanImageUse : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type TRANSFER_SOURCE = 1, TRANSFER_DESTINATION = 2, SAMPLE = 4, STORAGE = 8, COLOR_ATTACHMENT = 16, DEPTH_STENCIL_ATTACHMENT = 32, TRANSIENT_ATTACHMENT = 64, INPUT_ATTACHMENT = 128;
	};

	enum class VulkanBindingType : GTSL::uint32
	{
		SAMPLER = 0,
		COMBINED_IMAGE_SAMPLER = 1,
		SAMPLED_IMAGE = 2,
		STORAGE_IMAGE = 3,
		UNIFORM_TEXEL_BUFFER = 4,
		STORAGE_TEXEL_BUFFER = 5,
		UNIFORM_BUFFER = 6,
		STORAGE_BUFFER = 7,
		UNIFORM_BUFFER_DYNAMIC = 8,
		STORAGE_BUFFER_DYNAMIC = 9,
		INPUT_ATTACHMENT = 10,
		ACCELERATION_STRUCTURE  = 1000165000
	};
	
	enum class VulkanShaderType : GTSL::uint32
	{
		VERTEX,
		TESSELLATION_CONTROL,
		TESSELLATION_EVALUATION,
		GEOMETRY,
		FRAGMENT,

		COMPUTE,

		RAYGEN, ANY_HIT, CLOSEST_HIT, MISS, INTERSECTION, CALLABLE
	};

	enum class VulkanIndexType
	{
		UINT16 = 0, UINT32 = 1
	};

	enum class VulkanGeometryType
	{
		TRIANGLES = 0, AABB = 1
	};

	struct VulkanGeometryFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type OPAQUE = 1, NO_DUPLICATE_ANY_HIT = 2;
	};
	
	struct VulkanShaderStage : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type VERTEX = 1, TESSELLATION_CONTROL = 2;
		static constexpr value_type TESSELLATION_EVALUATION = 4, GEOMETRY = 8, FRAGMENT = 16, COMPUTE = 32, ALL = 0x7FFFFFFF;
		static constexpr value_type RAYGEN = 0x00000100, ANY_HIT = 0x00000200, CLOSEST_HIT = 0x00000400, MISS = 0x00000800, INTERSECTION = 0x00001000, CALLABLE = 0x00002000;
	};

	enum class VulkanQueryType
	{
		OCCLUSION = 0,
		PIPELINE_STATISTICS = 1,
		TIMESTAMP = 2,
		// Provided by VK_EXT_transform_feedback
		TRANSFORM_FEEDBACK_STREAM = 1000028004,
		// Provided by VK_KHR_performance_query
		PERFORMANCE_QUERY = 1000116000,
		// Provided by VK_KHR_ray_tracing
		ACCELERATION_STRUCTURE_COMPACTED_SIZE = 1000165000,
		// Provided by VK_KHR_ray_tracing
		ACCELERATION_STRUCTURE_SERIALIZATION_SIZE = 1000150000,
	};

	enum class VulkanAccelerationStructureBuildType
	{
		GPU_LOCAL = 0, HOST = 1, GPU_OR_HOST = 2
	};

	enum class VulkanAccelerationStructureMemoryRequirementsType
	{
		OBJECT = 0,
		BUILD_SCRATCH = 1,
		UPDATE_SCRATCH = 2,
	};
	
	enum class VulkanColorSpace : GTSL::uint32
	{
		//The non linear SRGB color space is the most commonly used color space to display things on screen. Use this when you are not developing an HDR application.
		NONLINEAR_SRGB = 0,
	};
	
	enum class VulkanFormat : GTSL::uint32
	{
		UNDEFINED = 0,
		
		//INTEGER

		//R
		R_I8 = 9,
		R_I16 = 70,
		R_I32 = 98,
		R_I64 = 110,
		//RG
		RG_I8 = 16,
		RG_I16 = 77,
		RG_I32 = 101,
		RG_I64 = 113,
		//RBG
		RGB_I8 = 23,
		RGB_I16 = 84,
		RGB_I32 = 104,
		RGB_I64 = 116,
		//RGBA
		RGBA_I8 = 37,
		RGBA_I16 = 91,
		RGBA_I32 = 107,
		RGBA_I64 = 119,
		//RGBA
		BGRA_I8 = 44,

		BGR_I8 = 30,

		//  DEPTH STENCIL

		//A depth-only format with a 16 bit (2 byte) size.
		DEPTH16 = 124,
		//A depth-only format with a 32 (4 byte) bit size.
		DEPTH32 = 126,
		//A depth/stencil format with a 16 bit (2 byte) size depth part and an 8 bit (1 byte) size stencil part.
		DEPTH16_STENCIL8 = 128,
		//A depth/stencil format with a 24 bit (3 byte) size depth part and an 8 bit (1 byte) size stencil part.
		DEPTH24_STENCIL8 = 129,
		//A depth/stencil format with a 32 bit (4 byte) size depth part and an 8 bit (1 byte) size stencil part.
		DEPTH32_STENCIL8 = 130
	};
	
	/**
	* \brief Enumeration of all possible presentation modes, which define the order at which the rendered images are presented to the screen.
	*/
	enum class VulkanPresentMode : GTSL::uint32
	{
		/**
		* \brief The last rendered image is the one which will be presented. Best for when latency is important and energy consumption is not.
		*/
		SWAP = 1,
		/**
		* \brief All rendered images are queued in FIFO fashion and presented at V-BLANK. Best for when latency is not that important and energy consumption is.
		*/
		FIFO = 2,
	};

	struct VulkanBufferType : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type	TRANSFER_SOURCE = 1, TRANSFER_DESTINATION = 2, UNIFORM = 16, INDEX = 64, VERTEX = 128;
	};
	
	struct VulkanQueueCapabilities : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type GRAPHICS = 1, COMPUTE = 2, TRANSFER = 4;
	};
	
	struct VulkanMemoryType : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type GPU = 1, SHARED = 2, COHERENT = 4, CACHED = 8;
	};

	enum class VulkanPipelineType : GTSL::uint32
	{
		GRAPHICS = 0, COMPUTE = 1, RAY_TRACING = 1000165000
	};
	
	enum class VulkanShaderDataType
	{
		FLOAT = VK_FORMAT_R32_SFLOAT,
		FLOAT2 = VK_FORMAT_R32G32_SFLOAT,
		FLOAT3 = VK_FORMAT_R32G32B32_SFLOAT,
		FLOAT4 = VK_FORMAT_R32G32B32A32_SFLOAT,
		INT = VK_FORMAT_R32_SINT,
		INT2 = VK_FORMAT_R32G32_SINT,
		INT3 = VK_FORMAT_R32G32B32_SINT,
		INT4 = VK_FORMAT_R32G32B32A32_SINT,
		BOOL = VK_FORMAT_R32_SINT
	};
	
	inline VulkanBindingType BindingTypeToVulkanBindingType(const BindingType binding)
	{
		switch (binding)
		{
		case BindingType::SAMPLER: return VulkanBindingType::SAMPLER;
		case BindingType::COMBINED_IMAGE_SAMPLER: return VulkanBindingType::COMBINED_IMAGE_SAMPLER;
		case BindingType::SAMPLED_IMAGE: return VulkanBindingType::SAMPLED_IMAGE;
		case BindingType::STORAGE_IMAGE: return VulkanBindingType::STORAGE_IMAGE;
		case BindingType::UNIFORM_TEXEL_BUFFER: return VulkanBindingType::UNIFORM_TEXEL_BUFFER;
		case BindingType::STORAGE_TEXEL_BUFFER: return VulkanBindingType::STORAGE_TEXEL_BUFFER;
		case BindingType::UNIFORM_BUFFER: return VulkanBindingType::UNIFORM_BUFFER;
		case BindingType::STORAGE_BUFFER: return VulkanBindingType::STORAGE_BUFFER;
		case BindingType::UNIFORM_BUFFER_DYNAMIC: return VulkanBindingType::UNIFORM_BUFFER_DYNAMIC;
		case BindingType::STORAGE_BUFFER_DYNAMIC: return VulkanBindingType::STORAGE_BUFFER_DYNAMIC;
		case BindingType::INPUT_ATTACHMENT: return VulkanBindingType::INPUT_ATTACHMENT;
		case BindingType::ACCELERATION_STRUCTURE: return VulkanBindingType::ACCELERATION_STRUCTURE;
		default: GAL_DEBUG_BREAK;
		}

		return VulkanBindingType::UNIFORM_BUFFER_DYNAMIC;
	}

	inline VulkanShaderType ShaderTypeToVulkanShaderType(const ShaderType shader)
	{
		switch (shader) {
		case ShaderType::VERTEX_SHADER: return VulkanShaderType::VERTEX;
		case ShaderType::TESSELLATION_CONTROL_SHADER: return VulkanShaderType::TESSELLATION_CONTROL;
		case ShaderType::TESSELLATION_EVALUATION_SHADER: return VulkanShaderType::TESSELLATION_EVALUATION;
		case ShaderType::GEOMETRY_SHADER: return VulkanShaderType::GEOMETRY;
		case ShaderType::FRAGMENT_SHADER: return VulkanShaderType::FRAGMENT;
		case ShaderType::COMPUTE_SHADER: return VulkanShaderType::COMPUTE;
		default: GAL_DEBUG_BREAK;
		}
	}

	inline VulkanShaderDataType ShaderDataTypeToVulkanShaderDataType(const ShaderDataType shaderDataType)
	{
		switch (shaderDataType)
		{
		case ShaderDataType::FLOAT: return VulkanShaderDataType::FLOAT;
		case ShaderDataType::FLOAT2: return VulkanShaderDataType::FLOAT2;
		case ShaderDataType::FLOAT3: return VulkanShaderDataType::FLOAT3;
		case ShaderDataType::FLOAT4: return VulkanShaderDataType::FLOAT4;
		case ShaderDataType::INT: return VulkanShaderDataType::INT;
		case ShaderDataType::INT2: return VulkanShaderDataType::INT2;
		case ShaderDataType::INT3: return VulkanShaderDataType::INT3;
		case ShaderDataType::INT4: return VulkanShaderDataType::INT4;
		case ShaderDataType::BOOL: return VulkanShaderDataType::BOOL;
		case ShaderDataType::MAT3: break;
		case ShaderDataType::MAT4: break;
		default: GAL_DEBUG_BREAK;
		}

		return VulkanShaderDataType::FLOAT;
	}
	
	inline GTSL::uint8 VulkanShaderDataTypeSize(const VulkanShaderDataType sdt)
	{
		switch (sdt)
		{
		case VulkanShaderDataType::FLOAT: return 4;
		case VulkanShaderDataType::FLOAT2: return 4 * 2;
		case VulkanShaderDataType::FLOAT3: return 4 * 3;
		case VulkanShaderDataType::FLOAT4: return 4 * 4;
		case VulkanShaderDataType::INT: return 4;
		case VulkanShaderDataType::INT2: return 4 * 2;
		case VulkanShaderDataType::INT3: return 4 * 3;
		case VulkanShaderDataType::INT4: return 4 * 4;
		default: GAL_DEBUG_BREAK;
		}

		return 0;
	}

	inline VkShaderStageFlagBits VulkanShaderTypeToVkShaderStageFlagBits(const VulkanShaderType shader)
	{
		switch (shader)
		{
		case VulkanShaderType::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
		case VulkanShaderType::TESSELLATION_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case VulkanShaderType::TESSELLATION_EVALUATION: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case VulkanShaderType::GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
		case VulkanShaderType::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
		case VulkanShaderType::COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
		case VulkanShaderType::RAYGEN: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case VulkanShaderType::ANY_HIT: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case VulkanShaderType::CLOSEST_HIT: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case VulkanShaderType::MISS: return VK_SHADER_STAGE_MISS_BIT_KHR;
		case VulkanShaderType::INTERSECTION: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case VulkanShaderType::CALLABLE: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		default: GAL_DEBUG_BREAK;
		}

		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}