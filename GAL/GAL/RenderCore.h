#pragma once

#include <GTSL/Core.h>

namespace GAL
{
	constexpr GTSL::uint8 MAX_SHADER_STAGES = 8;

	class RenderDevice;
	
	struct RenderInfo
	{
		const RenderDevice* RenderDevice = nullptr;
	};

	enum class PipelineStage : GTSL::uint32
	{
		TOP_OF_PIPE = 1,
		DRAW_INDIRECT = 2,
		VERTEX_INPUT = 4,
		VERTEX_SHADER = 8,
		TESSELLATION_CONTROL_SHADER = 16,
		TESSELLATION_EVALUATION_SHADER = 32,
		GEOMETRY_SHADER = 64,
		FRAGMENT_SHADER = 128,
		EARLY_FRAGMENT_TESTS = 256,
		LATE_FRAGMENT_TESTS = 512,
		COLOR_ATTACHMENT_OUTPUT = 1024,
		COMPUTE_SHADER = 2048,
		TRANSFER = 4096,
		BOTTOM_OF_PIPE = 8192,
		HOST_BIT = 16384,
		ALL_GRAPHICS = 32768,
		ALL_COMMANDS = 65536,
		TRANSFORM_FEEDBACK_BIT = 131072,
		CONDITIONAL_RENDERING_BIT = 262144,
		RAY_TRACING_SHADER = 524288,
		ACCELERATION_STRUCTURE_BUILD = 1048576,
		SHADING_RATE_IMAGE_BIT = 2097152,
		TASK_SHADER = 4194304,
		MESH_SHADER = 8388608,
		FRAGMENT_DENSITY_PROCESS = 16777216,
		COMMAND_PREPROCESS_BIT = 33554432,
	};

	enum class AccessFlags : GTSL::uint32
	{
		INDIRECT_COMMAND_READ = 0x00000001,
		INDEX_READ = 0x00000002,
		VERTEX_ATTRIBUTE_READ = 0x00000004,
		UNIFORM_READ = 0x00000008,
		INPUT_ATTACHMENT_READ = 0x00000010,
		SHADER_READ = 0x00000020,
		SHADER_WRITE = 0x00000040,
		COLOR_ATTACHMENT_READ = 0x00000080,
		COLOR_ATTACHMENT_WRITE = 0x00000100,
		DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
		DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
		TRANSFER_READ = 0x00000800,
		TRANSFER_WRITE = 0x00001000,
		HOST_READ = 0x00002000,
		HOST_WRITE = 0x00004000,
		MEMORY_READ = 0x00008000,
		MEMORY_WRITE = 0x00010000,
		TRANSFORM_FEEDBACK_WRITE = 0x02000000,
		TRANSFORM_FEEDBACK_COUNTER_READ = 0x04000000,
		TRANSFORM_FEEDBACK_COUNTER_WRITE = 0x08000000,
		CONDITIONAL_RENDERING_READ = 0x00100000,
		COLOR_ATTACHMENT_READ_NONCOHERENT = 0x00080000,
		ACCELERATION_STRUCTURE_READ = 0x00200000,
		ACCELERATION_STRUCTURE_WRITE = 0x00400000,
		SHADING_RATE_IMAGE_READ = 0x00800000,
		FRAGMENT_DENSITY_MAP_READ_EXT = 0x01000000,
		COMMAND_PREPROCESS_READ = 0x00020000,
		COMMAND_PREPROCESS_WRITE = 0x00040000,
	};
	
	// IMAGE

	//Specifies all available image layouts.
	enum class ImageLayout : GTSL::uint8
	{
		UNDEFINED,
		GENERAL,
		COLOR_ATTACHMENT,
		DEPTH_STENCIL_ATTACHMENT,
		DEPTH_STENCIL_READ_ONLY,
		SHADER_READ,
		TRANSFER_SOURCE,
		TRANSFER_DESTINATION,
		PREINITIALIZED,
		PRESENTATION
	};
	
	enum class ImageDimensions : GTSL::uint8
	{
		IMAGE_1D,
		IMAGE_2D,
		IMAGE_3D
	};

	enum class ImageType : GTSL::uint8
	{
		COLOR,
		DEPTH,
		STENCIL,
		DEPTH_STENCIL
	};

	// ATTACHMENTS

	//Describes all possible operations a GAL can perform when loading a render target onto a render pass.
	enum class RenderTargetLoadOperations : GTSL::uint8
	{
		//We don't care about the previous content of the render target. Behavior is unknown.
		UNDEFINED,
		//We want to load the previous content of the render target.
		LOAD,
		//We want the render target to be cleared to black for color attachments and to 0 for depth/stencil attachments.
		CLEAR
	};

	//Describes all possible operations a GAL can perform when saving to a render target from a render pass.
	enum class RenderTargetStoreOperations : GTSL::uint8
	{
		//We don't care about the outcome of the render target.
		UNDEFINED,
		//We want to store the result of the render pass to this render attachment.
		STORE
	};

	enum class SampleCount : GTSL::uint8
	{
		SAMPLE_COUNT_1,
		SAMPLE_COUNT_2,
		SAMPLE_COUNT_4,
		SAMPLE_COUNT_8,
		SAMPLE_COUNT_16,
		SAMPLE_COUNT_32,
		SAMPLE_COUNT_64
	};

	// SHADERS

	enum class ShaderLanguage : GTSL::uint8
	{
		GLSL, HLSL
	};
	
	enum class ShaderType : GTSL::uint8
	{
		ALL_STAGES,

		VERTEX_SHADER,
		TESSELLATION_CONTROL_SHADER,
		TESSELLATION_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,

		COMPUTE_SHADER
	};

	enum class ShaderDataTypes : GTSL::uint8
	{
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,

		INT,
		INT2,
		INT3,
		INT4,

		BOOL,

		MAT3,
		MAT4,
	};

	// PIPELINE

	enum class CullMode : GTSL::uint8
	{
		CULL_NONE,
		CULL_FRONT,
		CULL_BACK
	};

	enum class BlendOperation : GTSL::uint8
	{
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX
	};

	enum class CompareOperation : GTSL::uint8
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_OR_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_OR_EQUAL,
		ALWAYS
	};

	enum class StencilCompareOperation : GTSL::uint8
	{
		KEEP,
		ZERO,
		REPLACE,
		INCREMENT_AND_CLAMP,
		DECREMENT_AND_CLAMP,
		INVERT,
		INCREMENT_AND_WRAP,
		DECREMENT_AND_WRAP
	};

	enum class BindingType : GTSL::uint8
	{
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,

		INT,
		INT2,
		INT3,
		INT4,

		BOOL,

		MAT3,
		MAT4,

		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,

		TEXTURE_2D_CUBE,
		
		SAMPLER,
		COMBINED_IMAGE_SAMPLER,
		SAMPLED_IMAGE,
		STORAGE_IMAGE,
		UNIFORM_TEXEL_BUFFER,
		STORAGE_TEXEL_BUFFER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER,
		UNIFORM_BUFFER_DYNAMIC,
		STORAGE_BUFFER_DYNAMIC,
		INPUT_ATTACHMENT
	};

	enum class ImageFormat
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

	inline GTSL::uint8 ShaderDataTypesSize(const ShaderDataTypes _SDT)
	{
		switch (_SDT)
		{
		case ShaderDataTypes::FLOAT: return 4;
		case ShaderDataTypes::FLOAT2: return 4 * 2;
		case ShaderDataTypes::FLOAT3: return 4 * 3;
		case ShaderDataTypes::FLOAT4: return 4 * 4;
		case ShaderDataTypes::INT: return 4;
		case ShaderDataTypes::INT2: return 4 * 2;
		case ShaderDataTypes::INT3: return 4 * 3;
		case ShaderDataTypes::INT4: return 4 * 4;
		case ShaderDataTypes::BOOL: return 4;
		case ShaderDataTypes::MAT3: return 4 * 3 * 3;
		case ShaderDataTypes::MAT4: return 4 * 4 * 4;
		default: return 0;
		}
	}

	inline GTSL::uint8 ImageFormatChannelSize(const ImageFormat imageFormat)
	{
		switch (imageFormat)
		{
		case ImageFormat::RGBA_I8: return 1;
		case ImageFormat::RGB_I8: return 1;
		default: __debugbreak();
		}
		return 0;
	}

	inline GTSL::uint8 ImageFormatChannelCount(const ImageFormat imageFormat)
	{
		switch (imageFormat)
		{
		case ImageFormat::RGBA_I8: return 4;
		case ImageFormat::RGB_I8: return 3;
		default: __debugbreak();
		}
		return 0;
	}

	inline GTSL::uint8 ImageFormatSize(const  ImageFormat imageFormat)
	{
		switch (imageFormat)
		{
		case ImageFormat::R_I8: break;
		case ImageFormat::R_I16: break;
		case ImageFormat::R_I32: break;
		case ImageFormat::R_I64: break;
		case ImageFormat::RG_I8: break;
		case ImageFormat::RG_I16: break;
		case ImageFormat::RG_I32: break;
		case ImageFormat::RG_I64: break;
		case ImageFormat::RGB_I8: break;
		case ImageFormat::RGB_I16: break;
		case ImageFormat::RGB_I32: break;
		case ImageFormat::RGB_I64: break;
		case ImageFormat::RGBA_I8: return 1 * 4;
		case ImageFormat::RGBA_I16: break;
		case ImageFormat::RGBA_I32: break;
		case ImageFormat::RGBA_I64: break;
		case ImageFormat::BGRA_I8: break;
		case ImageFormat::BGR_I8: break;
		case ImageFormat::DEPTH16: break;
		case ImageFormat::DEPTH32: break;
		case ImageFormat::DEPTH16_STENCIL8: break;
		case ImageFormat::DEPTH24_STENCIL8: break;
		case ImageFormat::DEPTH32_STENCIL8: break;
		default: __debugbreak();
		}
		return 0;
	}

#if (_WIN32)
#define GAL_DEBUG_BREAK __debugbreak();
#endif
	
	class GALObject
	{
	protected:
		GALObject() = default;
		~GALObject() = default;
	};
}