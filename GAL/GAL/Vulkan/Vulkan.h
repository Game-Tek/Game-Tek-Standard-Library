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

#define MAKE_VK_HANDLE(object) typedef struct object##_T* object;

namespace GAL
{
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
		case ShaderType::ALL_STAGES: return VK_SHADER_STAGE_ALL_GRAPHICS;
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

	inline VkFormat ShaderDataTypesToVkFormat(const ShaderDataTypes shaderDataTypes)
	{
		switch (shaderDataTypes)
		{
		case ShaderDataTypes::FLOAT: return VK_FORMAT_R32_SFLOAT;
		case ShaderDataTypes::FLOAT2: return VK_FORMAT_R32G32_SFLOAT;
		case ShaderDataTypes::FLOAT3: return VK_FORMAT_R32G32B32_SFLOAT;
		case ShaderDataTypes::FLOAT4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case ShaderDataTypes::INT: return VK_FORMAT_R32_SINT;
		case ShaderDataTypes::INT2: return VK_FORMAT_R32G32_SINT;
		case ShaderDataTypes::INT3: return VK_FORMAT_R32G32B32_SINT;
		case ShaderDataTypes::INT4: return VK_FORMAT_R32G32B32A32_SINT;
		case ShaderDataTypes::BOOL: return VK_FORMAT_R32_SINT;
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
	
	enum class VulkanImageUse : GTSL::uint32
	{
		TRANSFER_SOURCE = 1,
		TRANSFER_DESTINATION = 2,
		SAMPLE = 4,
		STORAGE = 8,
		COLOR_ATTACHMENT = 16,
		DEPTH_STENCIL_ATTACHMENT = 32,
		TRANSIENT_ATTACHMENT = 64,
		INPUT_ATTACHMENT = 128
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
	
	enum class VulkanBufferType : GTSL::uint32
	{
		TRANSFER_SOURCE = 1,
		TRANSFER_DESTINATION = 2,
		UNIFORM = 16,
		INDEX = 64,
		VERTEX = 128,
	};
	
	enum class VulkanQueueCapabilities : GTSL::uint32
	{
		GRAPHICS = 1,
		COMPUTE = 2,
		TRANSFER = 4
	};
	
	enum class VulkanMemoryType : GTSL::uint32
	{
		GPU = 0x00000001,
		SHARED = 0x00000002,
		COHERENT = 0x00000004,
		CACHED = 0x00000008,
	};
}