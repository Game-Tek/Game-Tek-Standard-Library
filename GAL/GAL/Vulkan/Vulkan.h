#pragma once

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


#include "GTSL/Bitman.h"
#include "GTSL/Flags.h"

#define MAKE_VK_HANDLE(object) typedef struct object##_T* (object);
#undef OPAQUE

#if (_DEBUG)

#define SET_NAME(handle, type, createInfo) VkDebugUtilsObjectNameInfoEXT debug_utils_object_name_info_ext{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };\
debug_utils_object_name_info_ext.objectHandle = reinterpret_cast<GTSL::uint64>(handle);\
debug_utils_object_name_info_ext.objectType = type;\
debug_utils_object_name_info_ext.pObjectName = createInfo.Name;\
createInfo.RenderDevice->vkSetDebugUtilsObjectNameEXT(createInfo.RenderDevice->GetVkDevice(), &debug_utils_object_name_info_ext);

#else
#define SET_NAME(handle, type, createInfo)
#endif

namespace GAL
{
	template<typename T>
	constexpr void debugClear(T& handle) { if constexpr (_DEBUG) { handle = reinterpret_cast<T>(0x000000CACA000000); } }
	
	using VulkanDeviceAddress = GTSL::uint64;
	using VulkanHandle = void*;
	
	struct VulkanRenderInfo
	{
		const class VulkanRenderDevice* RenderDevice = nullptr;

		VulkanRenderInfo() = default;
	};

	struct VulkanCreateInfo : VulkanRenderInfo
	{
		const char* Name = nullptr;

		VulkanCreateInfo() = default;
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

	//inline VkImageLayout ImageLayoutToVkImageLayout(const TextureLayout textureLayout)
	//{
	//	switch (textureLayout)
	//	{
	//	case TextureLayout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
	//	case TextureLayout::SHADER_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//	case TextureLayout::GENERAL: return VK_IMAGE_LAYOUT_GENERAL;
	//	case TextureLayout::COLOR_ATTACHMENT: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//	case TextureLayout::DEPTH_STENCIL_ATTACHMENT: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//	case TextureLayout::DEPTH_STENCIL_READ_ONLY: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
	//	case TextureLayout::TRANSFER_SOURCE: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	//	case TextureLayout::TRANSFER_DESTINATION: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	//	case TextureLayout::PREINITIALIZED: return VK_IMAGE_LAYOUT_PREINITIALIZED;
	//	case TextureLayout::PRESENTATION: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//	default: return VK_IMAGE_LAYOUT_MAX_ENUM;
	//	}
	//}
	
	enum class VulkanShaderGroupType
	{
		GENERAL, TRIANGLES, PROCEDURAL
	};

	struct VulkanBindingFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type UPDATE_AFTER_BIND = 0x00000001, UPDATE_UNUSED_WHILE_PENDING = 0x00000002, PARTIALLY_BOUND = 0x00000004, VARIABLE_DESCRIPTOR_COUNT = 0x00000008;
	};
	
	struct VulkanTextureType : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type COLOR = 1, DEPTH = 2, STENCIL = 4;
	};

	struct VulkanAccelerationStructureFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type ALLOW_UPDATE = 0x00000001, ALLOW_COMPACTION = 0x00000002, PREFER_FAST_TRACE = 0x00000004, PREFER_FAST_BUILD = 0x00000008, LOW_MEMORY = 0x00000010;
	};
	
	struct VulkanPipelineStage : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type TOP_OF_PIPE = 1;
		static constexpr value_type DRAW_INDIRECT = 2;
		static constexpr value_type VERTEX_INPUT = 4;
		static constexpr value_type VERTEX_SHADER = 8;
		static constexpr value_type TESSELLATION_CONTROL_SHADER = 16;
		static constexpr value_type TESSELLATION_EVALUATION_SHADER = 32;
		static constexpr value_type GEOMETRY_SHADER = 64;
		static constexpr value_type FRAGMENT_SHADER = 128;
		static constexpr value_type EARLY_FRAGMENT_TESTS = 256;
		static constexpr value_type LATE_FRAGMENT_TESTS = 512;
		static constexpr value_type COLOR_ATTACHMENT_OUTPUT = 1024;
		static constexpr value_type COMPUTE_SHADER = 2048;
		static constexpr value_type TRANSFER = 4096;
		static constexpr value_type BOTTOM_OF_PIPE = 8192;
		static constexpr value_type HOST_BIT = 16384;
		static constexpr value_type ALL_GRAPHICS = 32768;
		static constexpr value_type ALL_COMMANDS = 65536;
		static constexpr value_type TRANSFORM_FEEDBACK = 131072;
		static constexpr value_type CONDITIONAL_RENDERING = 262144;
		static constexpr value_type RAY_TRACING_SHADER = 524288;
		static constexpr value_type ACCELERATION_STRUCTURE_BUILD = 1048576;
		static constexpr value_type SHADING_RATE_IMAGE = 2097152;
		static constexpr value_type TASK_SHADER = 4194304;
		static constexpr value_type MESH_SHADER = 8388608;
		static constexpr value_type FRAGMENT_DENSITY_PROCESS = 16777216;
		static constexpr value_type COMMAND_PREPROCESS = 33554432;
	};

	struct VulkanAccessFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type INDIRECT_COMMAND_READ = 0x00000001;
		static constexpr value_type INDEX_READ = 0x00000002;
		static constexpr value_type VERTEX_ATTRIBUTE_READ = 0x00000004;
		static constexpr value_type UNIFORM_READ = 0x00000008;
		static constexpr value_type INPUT_ATTACHMENT_READ = 0x00000010;
		static constexpr value_type SHADER_READ = 0x00000020;
		static constexpr value_type SHADER_WRITE = 0x00000040;
		static constexpr value_type COLOR_ATTACHMENT_READ = 0x00000080;
		static constexpr value_type COLOR_ATTACHMENT_WRITE = 0x00000100;
		static constexpr value_type DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200;
		static constexpr value_type DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400;
		static constexpr value_type TRANSFER_READ = 0x00000800;
		static constexpr value_type TRANSFER_WRITE = 0x00001000;
		static constexpr value_type HOST_READ = 0x00002000;
		static constexpr value_type HOST_WRITE = 0x00004000;
		static constexpr value_type MEMORY_READ = 0x00008000;
		static constexpr value_type MEMORY_WRITE = 0x00010000;
		static constexpr value_type TRANSFORM_FEEDBACK_WRITE = 0x02000000;
		static constexpr value_type TRANSFORM_FEEDBACK_COUNTER_READ = 0x04000000;
		static constexpr value_type TRANSFORM_FEEDBACK_COUNTER_WRITE = 0x08000000;
		static constexpr value_type CONDITIONAL_RENDERING_READ = 0x00100000;
		static constexpr value_type COLOR_ATTACHMENT_READ_NONCOHERENT = 0x00080000;
		static constexpr value_type ACCELERATION_STRUCTURE_READ = 0x00200000;
		static constexpr value_type ACCELERATION_STRUCTURE_WRITE = 0x00400000;
		static constexpr value_type SHADING_RATE_IMAGE_READ = 0x00800000;
		static constexpr value_type FRAGMENT_DENSITY_MAP_READ = 0x01000000;
		static constexpr value_type COMMAND_PREPROCESS_READ = 0x00020000;
		static constexpr value_type COMMAND_PREPROCESS_WRITE = 0x00040000;
	};
	
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

	inline GTSL::uint32 ImageTypeToVkImageAspectFlagBits(const TextureType imageType)
	{
		switch (imageType)
		{
		case TextureType::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
		case TextureType::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
		case TextureType::STENCIL: return VK_IMAGE_ASPECT_STENCIL_BIT;
		case TextureType::DEPTH_STENCIL: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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
	
	enum class VulkanTextureTiling : GTSL::uint32
	{
		OPTIMAL = 0,
		LINEAR = 1,
	};
	
	struct VulkanTextureUses : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type TRANSFER_SOURCE = 1, TRANSFER_DESTINATION = 2, SAMPLE = 4, STORAGE = 8, COLOR_ATTACHMENT = 16, DEPTH_STENCIL_ATTACHMENT = 32, TRANSIENT_ATTACHMENT = 64, INPUT_ATTACHMENT = 128;
	};

	enum class VulkanDimensions
	{
		LINEAR = 0, SQUARE = 1, CUBE = 2
	};

	inline VulkanDimensions DimensionsToVulkanDimension(const Dimension dimension)
	{
		switch (dimension)
		{
		case Dimension::LINEAR: return VulkanDimensions::LINEAR;
		case Dimension::SQUARE: return VulkanDimensions::SQUARE;
		case Dimension::CUBE: return VulkanDimensions::CUBE;
		}

		GAL_DEBUG_BREAK;
	}
	
	enum class VulkanTextureLayout
	{
		UNDEFINED = 0,
		GENERAL = 1,
		COLOR_ATTACHMENT = 2,
		DEPTH_STENCIL_ATTACHMENT = 3,
		DEPTH_STENCIL_READ_ONLY = 4,
		SHADER_READ_ONLY = 5,
		TRANSFER_SRC = 6,
		TRANSFER_DST = 7,
		PRE_INITIALIZED = 8,
		DEPTH_READ_ONLY_STENCIL_ATTACHMENT = 1000117000,
		DEPTH_ATTACHMENT_STENCIL_READ_ONLY = 1000117001,
		DEPTH_ATTACHMENT = 1000241000,
		DEPTH_READ_ONLY = 1000241001,
		STENCIL_ATTACHMENT = 1000241002,
		STENCIL_READ_ONLY = 1000241003,
		PRESENTATION = 1000001002
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
		VERTEX = 0x00000001,
		TESSELLATION_CONTROL = 0x00000002,
		TESSELLATION_EVALUATION = 0x00000004,
		GEOMETRY = 0x00000008,
		FRAGMENT = 0x00000010,

		COMPUTE = 0x00000020,

		RAY_GEN = 0x00000100,
		ANY_HIT = 0x00000200,
		CLOSEST_HIT = 0x00000400,
		MISS = 0x00000800,
		INTERSECTION = 0x00001000,
		CALLABLE = 0x00002000
	};
	
	enum class VulkanIndexType
	{
		UINT16 = 0, UINT32 = 1
	};

	enum class VulkanGeometryType
	{
		TRIANGLES = 0, AABB = 1, INSTANCES = 1000150000
	};

	struct VulkanGeometryFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type OPAQUE = 1, NO_DUPLICATE_ANY_HIT = 2;
	};
	
	struct VulkanShaderStage : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type VERTEX = 1, TESSELLATION_CONTROL = 2;
		static constexpr value_type TESSELLATION_EVALUATION = 4, GEOMETRY = 8, FRAGMENT = 16, COMPUTE = 32, ALL = 0x7FFFFFFF;
		static constexpr value_type RAY_GEN = 0x00000100, ANY_HIT = 0x00000200, CLOSEST_HIT = 0x00000400, MISS = 0x00000800, INTERSECTION = 0x00001000, CALLABLE = 0x00002000;
	};

	enum class VulkanQueryType
	{
		OCCLUSION = 0,
		PIPELINE_STATISTICS = 1,
		TIMESTAMP = 2,
		TRANSFORM_FEEDBACK_STREAM = 1000028004,
		PERFORMANCE_QUERY = 1000116000,
		ACCELERATION_STRUCTURE_COMPACTED_SIZE = 1000165000,
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
	
	enum class VulkanTextureFormat : GTSL::uint32
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

		STENCIL_8 = 127,
		
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
		static constexpr value_type	TRANSFER_SOURCE = 1, TRANSFER_DESTINATION = 2, UNIFORM = 16, STORAGE = 0x00000020, INDEX = 64, VERTEX = 128, ADDRESS = 0x00020000;
		static constexpr value_type	RAY_TRACING = 0x00000400, INDIRECT = 0x00000100;
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
		RASTER = 0, COMPUTE = 1, RAY_TRACING = 1000165000
	};
	
	enum class VulkanShaderDataType
	{
		FLOAT = 100,
		FLOAT2 = 103,
		FLOAT3 = 106,
		FLOAT4 = 109,
		INT = 99,
		INT2 = 102,
		INT3 = 105,
		INT4 = 108,
		BOOL = 99
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
		}

		GAL_DEBUG_BREAK;
		
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
		case VulkanShaderType::RAY_GEN: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case VulkanShaderType::ANY_HIT: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case VulkanShaderType::CLOSEST_HIT: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case VulkanShaderType::MISS: return VK_SHADER_STAGE_MISS_BIT_KHR;
		case VulkanShaderType::INTERSECTION: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case VulkanShaderType::CALLABLE: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		default: GAL_DEBUG_BREAK;
		}

		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}

	inline VulkanTextureFormat TextureFormatToVulkanTextureFormat(const TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::UNDEFINED:		return VulkanTextureFormat::UNDEFINED;
		case TextureFormat::R_I8: 		    return VulkanTextureFormat::R_I8;
		case TextureFormat::R_I16: 		    return VulkanTextureFormat::R_I16;
		case TextureFormat::R_I32: 		    return VulkanTextureFormat::R_I32;
		case TextureFormat::R_I64: 		    return VulkanTextureFormat::R_I64;
		case TextureFormat::RG_I8: 		    return VulkanTextureFormat::RG_I8;
		case TextureFormat::RG_I16:		    return VulkanTextureFormat::RG_I16;
		case TextureFormat::RG_I32:		    return VulkanTextureFormat::RG_I32;
		case TextureFormat::RG_I64:		    return VulkanTextureFormat::RG_I64;
		case TextureFormat::RGB_I8:		    return VulkanTextureFormat::RGB_I8;
		case TextureFormat::RGB_I16:		return VulkanTextureFormat::RGB_I16;
		case TextureFormat::RGB_I32:		return VulkanTextureFormat::RGB_I32;
		case TextureFormat::RGB_I64:		return VulkanTextureFormat::RGB_I64;
		case TextureFormat::RGBA_I8:		return VulkanTextureFormat::RGBA_I8;
		case TextureFormat::RGBA_I16:		return VulkanTextureFormat::RGBA_I16;
		case TextureFormat::RGBA_I32:		return VulkanTextureFormat::RGBA_I32;
		case TextureFormat::RGBA_I64:		return VulkanTextureFormat::RGBA_I64;
		case TextureFormat::BGRA_I8: 		return VulkanTextureFormat::BGRA_I8;
		case TextureFormat::BGR_I8:		    return VulkanTextureFormat::BGR_I8;
		case TextureFormat::DEPTH16: 		return VulkanTextureFormat::DEPTH16;
		case TextureFormat::DEPTH32: 		return VulkanTextureFormat::DEPTH32;
		case TextureFormat::DEPTH16_STENCIL8: return VulkanTextureFormat::DEPTH16_STENCIL8;
		case TextureFormat::DEPTH24_STENCIL8: return VulkanTextureFormat::DEPTH24_STENCIL8;
		case TextureFormat::DEPTH32_STENCIL8: return VulkanTextureFormat::DEPTH32_STENCIL8;
		default: break;
		}

		__debugbreak();

		return VulkanTextureFormat::UNDEFINED;
	}
	
	inline VulkanShaderStage ShaderStageToVulkanShaderStage(const ShaderStage::value_type stage)
	{
		VulkanShaderStage shaderStage{};

		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::VERTEX), stage & ShaderStage::VERTEX, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::TESSELLATION_CONTROL), stage & ShaderStage::TESSELLATION_CONTROL, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::TESSELLATION_EVALUATION), stage & ShaderStage::TESSELLATION_EVALUATION, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::GEOMETRY), stage & ShaderStage::GEOMETRY, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::FRAGMENT), stage & ShaderStage::FRAGMENT, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::COMPUTE), stage & ShaderStage::COMPUTE, shaderStage);
		//GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::TASK), stage & ShaderStage::TASK, stage);
		//GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::MESH), stage & ShaderStage::MESH, stage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::RAY_GEN), stage & ShaderStage::RAY_GEN, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::ANY_HIT), stage & ShaderStage::ANY_HIT, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::CLOSEST_HIT), stage & ShaderStage::CLOSEST_HIT, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::MISS), stage & ShaderStage::MISS, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::INTERSECTION), stage & ShaderStage::INTERSECTION, shaderStage);
		GTSL::SetBitAs(GTSL::FindFirstSetBit(VulkanShaderStage::CALLABLE), stage & ShaderStage::CALLABLE, shaderStage);

		return shaderStage;
	}

	inline VulkanTextureType::value_type TextureTypeToVulkanTextureType(const TextureType type)
	{
		switch (type) {
		case TextureType::COLOR: return VulkanTextureType::COLOR;
		case TextureType::DEPTH: return VulkanTextureType::DEPTH;
		case TextureType::STENCIL: return VulkanTextureType::STENCIL;
		case TextureType::DEPTH_STENCIL: return VulkanTextureType::DEPTH | VulkanTextureType::STENCIL;
		}

		return 0;
	}
}