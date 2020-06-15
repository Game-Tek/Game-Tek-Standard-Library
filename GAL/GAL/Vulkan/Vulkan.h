#pragma once

#include <vulkan/vulkan.h>

#if (_DEBUG)
#define VK_CHECK(func) { if ((func) != VK_SUCCESS) { __debugbreak(); } }
#else
#define VK_CHECK(func) func
#endif // (_DEBUG)

#include "GAL/RenderCore.h"

#include <GTSL/Extent.h>

#include "GAL/RenderDevice.h"

inline GAL::ImageFormat VkFormatToImageFormat(const VkFormat format)
{
	switch (format)
	{
	case VK_FORMAT_R8_UNORM: return GAL::ImageFormat::R_I8;
	case VK_FORMAT_R16_UNORM: return GAL::ImageFormat::R_I16;
	case VK_FORMAT_R32_UINT: return GAL::ImageFormat::R_I32;
	case VK_FORMAT_R64_UINT: return GAL::ImageFormat::R_I64;
	case VK_FORMAT_R8G8_UNORM: return GAL::ImageFormat::RG_I8;
	case VK_FORMAT_R16G16_UNORM: return GAL::ImageFormat::RG_I16;
	case VK_FORMAT_R32G32_UINT: return GAL::ImageFormat::RG_I32;
	case VK_FORMAT_R64G64_UINT: return GAL::ImageFormat::RG_I64;
	case VK_FORMAT_R8G8B8_UNORM: return GAL::ImageFormat::RGB_I8;
	case VK_FORMAT_R16G16B16_UNORM: return GAL::ImageFormat::RGB_I16;
	case VK_FORMAT_R32G32B32_UINT: return GAL::ImageFormat::RGB_I32;
	case VK_FORMAT_R64G64B64_UINT: return GAL::ImageFormat::RGB_I64;
	case VK_FORMAT_R8G8B8A8_UNORM: return GAL::ImageFormat::RGBA_I8;
	case VK_FORMAT_R16G16B16A16_UNORM: return GAL::ImageFormat::RGBA_I16;
	case VK_FORMAT_R32G32B32A32_UINT: return GAL::ImageFormat::RGBA_I32;
	case VK_FORMAT_R64G64B64A64_UINT: return GAL::ImageFormat::RGBA_I64;
	case VK_FORMAT_B8G8R8_UNORM: return GAL::ImageFormat::BGR_I8;
	case VK_FORMAT_B8G8R8A8_UNORM: return GAL::ImageFormat::BGRA_I8;
	case VK_FORMAT_R16_SFLOAT: return GAL::ImageFormat::R_F16;
	case VK_FORMAT_R32_SFLOAT: return GAL::ImageFormat::R_F32;
	case VK_FORMAT_R64_SFLOAT: return GAL::ImageFormat::R_F64;
	case VK_FORMAT_R16G16_SFLOAT: return GAL::ImageFormat::RG_F16;
	case VK_FORMAT_R32G32_SFLOAT: return GAL::ImageFormat::RG_F32;
	case VK_FORMAT_R64G64_SFLOAT: return GAL::ImageFormat::RG_F64;
	case VK_FORMAT_R16G16B16_SFLOAT: return GAL::ImageFormat::RGB_F16;
	case VK_FORMAT_R32G32B32_SFLOAT: return GAL::ImageFormat::RGB_F32;
	case VK_FORMAT_R64G64B64_SFLOAT: return GAL::ImageFormat::RGB_F64;
	case VK_FORMAT_R16G16B16A16_SFLOAT: return GAL::ImageFormat::RGBA_F16;
	case VK_FORMAT_R32G32B32A32_SFLOAT: return GAL::ImageFormat::RGBA_F32;
	case VK_FORMAT_R64G64B64A64_SFLOAT: return GAL::ImageFormat::RGBA_F64;
	case VK_FORMAT_D16_UNORM: return GAL::ImageFormat::DEPTH16;
	case VK_FORMAT_D32_SFLOAT: return GAL::ImageFormat::DEPTH32;
	case VK_FORMAT_D16_UNORM_S8_UINT: return GAL::ImageFormat::DEPTH16_STENCIL8;
	case VK_FORMAT_D24_UNORM_S8_UINT: return GAL::ImageFormat::DEPTH24_STENCIL8;
	case VK_FORMAT_D32_SFLOAT_S8_UINT: return GAL::ImageFormat::DEPTH32_STENCIL8;
	default: return GAL::ImageFormat::R_I8;
	}
}

inline VkFormat ImageFormatToVkFormat(const GAL::ImageFormat imageFormat)
{
	switch (imageFormat)
	{
	case GAL::ImageFormat::R_I8: return VK_FORMAT_R8_UNORM;
	case GAL::ImageFormat::R_I16: return VK_FORMAT_R16_UNORM;
	case GAL::ImageFormat::R_I32: return VK_FORMAT_R32_UINT;
	case GAL::ImageFormat::R_I64: return VK_FORMAT_R64_UINT;
	case GAL::ImageFormat::RG_I8: return VK_FORMAT_R8G8_UNORM;
	case GAL::ImageFormat::RG_I16: return VK_FORMAT_R16G16_UNORM;
	case GAL::ImageFormat::RG_I32: return VK_FORMAT_R32G32_UINT;
	case GAL::ImageFormat::RG_I64: return VK_FORMAT_R64G64_UINT;
	case GAL::ImageFormat::RGB_I8: return VK_FORMAT_R8G8B8_UNORM;
	case GAL::ImageFormat::RGB_I16: return VK_FORMAT_R16G16B16_UNORM;
	case GAL::ImageFormat::RGB_I32: return VK_FORMAT_R32G32B32_UINT;
	case GAL::ImageFormat::RGB_I64: return VK_FORMAT_R64G64B64_UINT;
	case GAL::ImageFormat::RGBA_I8: return VK_FORMAT_R8G8B8A8_UNORM;
	case GAL::ImageFormat::RGBA_I16: return VK_FORMAT_R16G16B16A16_UNORM;
	case GAL::ImageFormat::RGBA_I32: return VK_FORMAT_R32G32B32A32_UINT;
	case GAL::ImageFormat::RGBA_I64: return VK_FORMAT_R64G64B64A64_UINT;
	case GAL::ImageFormat::BGRA_I8: return VK_FORMAT_B8G8R8A8_UNORM;
	case GAL::ImageFormat::BGR_I8: return VK_FORMAT_B8G8R8_UNORM;
	case GAL::ImageFormat::R_F16: return VK_FORMAT_R16_SFLOAT;
	case GAL::ImageFormat::R_F32: return VK_FORMAT_R32_SFLOAT;
	case GAL::ImageFormat::R_F64: return VK_FORMAT_R64_SFLOAT;
	case GAL::ImageFormat::RG_F16: return VK_FORMAT_R16G16_SFLOAT;
	case GAL::ImageFormat::RG_F32: return VK_FORMAT_R32G32_SFLOAT;
	case GAL::ImageFormat::RG_F64: return VK_FORMAT_R64G64_SFLOAT;
	case GAL::ImageFormat::RGB_F16: return VK_FORMAT_R16G16B16_SFLOAT;
	case GAL::ImageFormat::RGB_F32: return VK_FORMAT_R32G32B32_SFLOAT;
	case GAL::ImageFormat::RGB_F64: return VK_FORMAT_R64G64B64_SFLOAT;
	case GAL::ImageFormat::RGBA_F16: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case GAL::ImageFormat::RGBA_F32: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case GAL::ImageFormat::RGBA_F64: return VK_FORMAT_R64G64B64A64_SFLOAT;
	case GAL::ImageFormat::DEPTH16: return VK_FORMAT_D16_UNORM;
	case GAL::ImageFormat::DEPTH32: return VK_FORMAT_D32_SFLOAT;
	case GAL::ImageFormat::DEPTH16_STENCIL8: return VK_FORMAT_D16_UNORM_S8_UINT;
	case GAL::ImageFormat::DEPTH24_STENCIL8: return VK_FORMAT_D24_UNORM_S8_UINT;
	case GAL::ImageFormat::DEPTH32_STENCIL8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
	default: return VK_FORMAT_UNDEFINED;
	}
}

inline VkImageTiling ImageTilingToVkImageTiling(const GAL::ImageTiling imageTiling)
{
	switch (imageTiling)
	{
	case GAL::ImageTiling::LINEAR: return VK_IMAGE_TILING_LINEAR;
	case GAL::ImageTiling::OPTIMAL: return VK_IMAGE_TILING_OPTIMAL;
	default: return VK_IMAGE_TILING_MAX_ENUM;
	}
}

inline VkFormatFeatureFlagBits ImageUseToVkFormatFeatureFlagBits(const GAL::ImageUse imageUse)
{
	switch (imageUse)
	{
	case GAL::ImageUse::TRANSFER_SOURCE: return VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
	case GAL::ImageUse::TRANSFER_DESTINATION: return VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
	case GAL::ImageUse::SAMPLE: return VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
	case GAL::ImageUse::STORAGE: return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
	case GAL::ImageUse::COLOR_ATTACHMENT: return VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	case GAL::ImageUse::DEPTH_STENCIL_ATTACHMENT: return VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
	case GAL::ImageUse::TRANSIENT_ATTACHMENT: return VK_FORMAT_FEATURE_FLAG_BITS_MAX_ENUM;
	case GAL::ImageUse::INPUT_ATTACHMENT: return VK_FORMAT_FEATURE_FLAG_BITS_MAX_ENUM;
	default: return VK_FORMAT_FEATURE_FLAG_BITS_MAX_ENUM;
	}
}

inline VkAttachmentLoadOp RenderTargetLoadOperationsToVkAttachmentLoadOp(const GAL::RenderTargetLoadOperations renderTargetLoadOperations)
{
	switch (renderTargetLoadOperations)
	{
	case GAL::RenderTargetLoadOperations::UNDEFINED: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	case GAL::RenderTargetLoadOperations::LOAD: return VK_ATTACHMENT_LOAD_OP_LOAD;
	case GAL::RenderTargetLoadOperations::CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
	default: return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

inline VkAttachmentStoreOp RenderTargetStoreOperationsToVkAttachmentStoreOp(const GAL::RenderTargetStoreOperations renderTargetStoreOperations)
{
	switch (renderTargetStoreOperations)
	{
	case GAL::RenderTargetStoreOperations::UNDEFINED: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	case GAL::RenderTargetStoreOperations::STORE: return VK_ATTACHMENT_STORE_OP_STORE;
	default: return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

inline VkImageLayout ImageLayoutToVkImageLayout(const GAL::ImageLayout imageLayout)
{
	switch (imageLayout)
	{
	case GAL::ImageLayout::UNDEFINED: return VK_IMAGE_LAYOUT_UNDEFINED;
	case GAL::ImageLayout::SHADER_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	case GAL::ImageLayout::GENERAL: return VK_IMAGE_LAYOUT_GENERAL;
	case GAL::ImageLayout::COLOR_ATTACHMENT: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case GAL::ImageLayout::DEPTH_STENCIL_ATTACHMENT: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	case GAL::ImageLayout::DEPTH_STENCIL_READ_ONLY: return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
	case GAL::ImageLayout::TRANSFER_SOURCE: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case GAL::ImageLayout::TRANSFER_DESTINATION: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case GAL::ImageLayout::PREINITIALIZED: return VK_IMAGE_LAYOUT_PREINITIALIZED;
	case GAL::ImageLayout::PRESENTATION: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	default: return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}

inline VkShaderStageFlagBits ShaderTypeToVkShaderStageFlagBits(const GAL::ShaderType shaderType)
{
	switch (shaderType)
	{
	case GAL::ShaderType::ALL_STAGES: return VK_SHADER_STAGE_ALL_GRAPHICS;
	case GAL::ShaderType::VERTEX_SHADER: return VK_SHADER_STAGE_VERTEX_BIT;
	case GAL::ShaderType::TESSELLATION_CONTROL_SHADER: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case GAL::ShaderType::TESSELLATION_EVALUATION_SHADER: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	case GAL::ShaderType::GEOMETRY_SHADER: return VK_SHADER_STAGE_GEOMETRY_BIT;
	case GAL::ShaderType::FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
	case GAL::ShaderType::COMPUTE_SHADER: return VK_SHADER_STAGE_COMPUTE_BIT;
	default: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}

inline VkExtent2D Extent2DToVkExtent2D(const GTSL::Extent2D extent)
{
	return {extent.Width, extent.Height};
}

inline VkExtent3D Extent3DToVkExtent3D(const GTSL::Extent3D extent)
{
	return { extent.Width, extent.Height, extent.Depth };
}

inline VkImageViewType ImageDimensionsToVkImageViewType(const GAL::ImageDimensions imageDimensions)
{
	switch (imageDimensions)
	{
	case GAL::ImageDimensions::IMAGE_1D: return VK_IMAGE_VIEW_TYPE_1D;
	case GAL::ImageDimensions::IMAGE_2D: return VK_IMAGE_VIEW_TYPE_2D;
	case GAL::ImageDimensions::IMAGE_3D: return VK_IMAGE_VIEW_TYPE_3D;
	default: return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}

inline VkImageType ImageDimensionsToVkImageType(const GAL::ImageDimensions imageDimensions)
{
	switch (imageDimensions)
	{
	case GAL::ImageDimensions::IMAGE_1D: return VK_IMAGE_TYPE_1D;
	case GAL::ImageDimensions::IMAGE_2D: return VK_IMAGE_TYPE_2D;
	case GAL::ImageDimensions::IMAGE_3D: return VK_IMAGE_TYPE_3D;
	default: return VK_IMAGE_TYPE_MAX_ENUM;
	}
}

inline GTSL::uint32 ImageTypeToVkImageAspectFlagBits(const GAL::ImageType imageType)
{
	switch (imageType)
	{
	case GAL::ImageType::COLOR: return VK_IMAGE_ASPECT_COLOR_BIT;
	case GAL::ImageType::DEPTH: return VK_IMAGE_ASPECT_DEPTH_BIT;
	case GAL::ImageType::STENCIL: return VK_IMAGE_ASPECT_STENCIL_BIT;
	case GAL::ImageType::DEPTH_STENCIL: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	default: return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
	}
}

inline VkFormat ShaderDataTypesToVkFormat(const GAL::ShaderDataTypes shaderDataTypes)
{
	switch (shaderDataTypes)
	{
	case GAL::ShaderDataTypes::FLOAT: return VK_FORMAT_R32_SFLOAT;
	case GAL::ShaderDataTypes::FLOAT2: return VK_FORMAT_R32G32_SFLOAT;
	case GAL::ShaderDataTypes::FLOAT3: return VK_FORMAT_R32G32B32_SFLOAT;
	case GAL::ShaderDataTypes::FLOAT4: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case GAL::ShaderDataTypes::INT: return VK_FORMAT_R32_SINT;
	case GAL::ShaderDataTypes::INT2: return VK_FORMAT_R32G32_SINT;
	case GAL::ShaderDataTypes::INT3: return VK_FORMAT_R32G32B32_SINT;
	case GAL::ShaderDataTypes::INT4: return VK_FORMAT_R32G32B32A32_SINT;
	case GAL::ShaderDataTypes::BOOL: return VK_FORMAT_R32_SINT;
	default: return VK_FORMAT_UNDEFINED;
	}
}

inline VkImageUsageFlagBits ImageUseToVkImageUsageFlagBits(const GAL::ImageUse imageUse)
{
	switch (imageUse)
	{
	case GAL::ImageUse::TRANSFER_SOURCE: return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	case GAL::ImageUse::TRANSFER_DESTINATION: return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	case GAL::ImageUse::SAMPLE: return VK_IMAGE_USAGE_SAMPLED_BIT;
	case GAL::ImageUse::STORAGE: return VK_IMAGE_USAGE_STORAGE_BIT;
	case GAL::ImageUse::COLOR_ATTACHMENT: return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	case GAL::ImageUse::DEPTH_STENCIL_ATTACHMENT: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	case GAL::ImageUse::TRANSIENT_ATTACHMENT: return VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	case GAL::ImageUse::INPUT_ATTACHMENT: return VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	default: return VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
	}
}

inline VkDescriptorType UniformTypeToVkDescriptorType(const GAL::BindingType uniformType)
{
	switch (uniformType)
	{
	case GAL::BindingType::SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
	case GAL::BindingType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case GAL::BindingType::SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case GAL::BindingType::STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case GAL::BindingType::UNIFORM_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	case GAL::BindingType::STORAGE_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	case GAL::BindingType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case GAL::BindingType::STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case GAL::BindingType::UNIFORM_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	case GAL::BindingType::STORAGE_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	case GAL::BindingType::INPUT_ATTACHMENT: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	default: return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
};

inline VkCullModeFlagBits CullModeToVkCullModeFlagBits(const GAL::CullMode cullMode)
{
	switch (cullMode)
	{
	case GAL::CullMode::CULL_BACK: return VK_CULL_MODE_BACK_BIT;
	case GAL::CullMode::CULL_FRONT: return VK_CULL_MODE_FRONT_BIT;
	default: return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

inline VkCompareOp CompareOperationToVkCompareOp(const GAL::CompareOperation compareOperation)
{
	switch (compareOperation)
	{
	case GAL::CompareOperation::NEVER: return VK_COMPARE_OP_NEVER;
	case GAL::CompareOperation::LESS: return VK_COMPARE_OP_LESS;
	case GAL::CompareOperation::EQUAL: return VK_COMPARE_OP_EQUAL;
	case GAL::CompareOperation::LESS_OR_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
	case GAL::CompareOperation::GREATER: return VK_COMPARE_OP_GREATER;
	case GAL::CompareOperation::NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
	case GAL::CompareOperation::GREATER_OR_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case GAL::CompareOperation::ALWAYS: return VK_COMPARE_OP_ALWAYS;
	default: ;
	}
	return {};
}

inline VkPresentModeKHR PresentModeToVkPresentModeKHR(const GAL::PresentMode presentMode)
{
	switch (presentMode)
	{
	case GAL::PresentMode::FIFO: return VK_PRESENT_MODE_FIFO_KHR;
	case GAL::PresentMode::SWAP: return VK_PRESENT_MODE_MAILBOX_KHR;
	default: return VK_PRESENT_MODE_MAX_ENUM_KHR;
	}
}

inline VkBufferUsageFlags BufferTypeToVkBufferUsageFlags(const GAL::BufferType bufferType)
{
	switch (bufferType)
	{
	case GAL::BufferType::BUFFER_VERTEX: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	case GAL::BufferType::BUFFER_INDEX: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	case GAL::BufferType::BUFFER_UNIFORM: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	default: ;
	}
}

inline VkQueueFlags QueueCapabilitiesToVkQueueFlags(const GAL::QueueCapabilities capabilities)
{
	switch (capabilities)
	{
	case GAL::QueueCapabilities::GRAPHICS: return VK_QUEUE_GRAPHICS_BIT;
	case GAL::QueueCapabilities::COMPUTE: return VK_QUEUE_COMPUTE_BIT;
	case GAL::QueueCapabilities::TRANSFER: return VK_QUEUE_TRANSFER_BIT;
	default: ;
	}
}