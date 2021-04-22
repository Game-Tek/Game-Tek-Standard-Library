#pragma once

#include <GTSL/Core.h>

#include <GTSL/Flags.h>

#include <GTSL/Algorithm.h>

namespace GAL
{
	template<typename T>
	constexpr void debugClear(T& handle) { if constexpr (_DEBUG) { handle = reinterpret_cast<T>(0x000000CACA000000); } }
	
	constexpr GTSL::uint8 MAX_SHADER_STAGES = 8;

	enum class RenderAPI : GTSL::uint8
	{
		VULKAN,
		DIRECTX12
	};

	struct MemoryRequirements
	{
		GTSL::uint32 Size{ 0 };
		GTSL::uint32 Alignment{ 0 };
		GTSL::uint32 MemoryTypes{ 0 };
	};

	constexpr GTSL::uint8 RAY_GEN_TABLE_INDEX = 0, HIT_TABLE_INDEX = 1, MISS_TABLE_INDEX = 2, CALLABLE_TABLE_INDEX = 3;

	enum class ComponentType : GTSL::uint8 { INT, UINT, FLOAT };
	enum class TextureType : GTSL::uint8 { COLOR, DEPTH };

	struct ShaderHandle
	{
		ShaderHandle() = default;
		ShaderHandle(void* data, GTSL::uint32 size, GTSL::uint32 alignedSize) : Data(data), Size(size), AlignedSize(alignedSize) {}
		void* Data; GTSL::uint32 Size, AlignedSize;
	};
	
	inline constexpr GTSL::uint32 MakeFormatFlag(GTSL::uint8 compCount, ComponentType compType, GTSL::uint8 bitDepth, TextureType type, GTSL::uint8 a, GTSL::uint8 b, GTSL::uint8 c, GTSL::uint8 d)
	{
		return GTSL::UnderlyingType<ComponentType>(compType) | compCount << 4 | (a | b << 2 | c << 4 | d << 6) << 8 | bitDepth << 16 | GTSL::UnderlyingType<TextureType>(type) << 22;
	}

	struct FormatDescriptor
	{
		FormatDescriptor() = default;
		
		constexpr FormatDescriptor(ComponentType compType, GTSL::uint8 compCount, GTSL::uint8 bitDepth, TextureType type, GTSL::uint8 a, GTSL::uint8 b, GTSL::uint8 c, GTSL::uint8 d) :
		ComponentType(compType), ComponentCount(compCount), A(a), B(b), C(c), D(d), BitDepth(bitDepth), Type(type)
		{}
		
		ComponentType ComponentType : 4; //0
		GTSL::uint8 ComponentCount : 4;  //4
		GTSL::uint8 A : 2;               //8
		GTSL::uint8 B : 2;               //10
		GTSL::uint8 C : 2;               //12
		GTSL::uint8 D : 2;               //14
		GTSL::uint8 BitDepth : 6;        //16
		TextureType Type : 2;            //22

		GTSL::uint8 GetSize() const { return BitDepth / 8 * ComponentCount; }
	};

	namespace FORMATS
	{
		static constexpr auto RGB_I8 = FormatDescriptor(ComponentType::INT, 3, 8, TextureType::COLOR, 0, 1, 2, 3);
		static constexpr auto BGRA_I8 = FormatDescriptor(ComponentType::INT, 4, 8, TextureType::COLOR, 2, 1, 0, 3);
		static constexpr auto RGBA_F16 = FormatDescriptor(ComponentType::FLOAT, 4, 16, TextureType::COLOR, 0, 1, 2, 3);
		static constexpr auto RGBA_I8 = FormatDescriptor(ComponentType::INT, 4, 8, TextureType::COLOR, 0, 1, 2, 3);
	}
		
	inline constexpr GTSL::uint32 MakeFormatFlag(FormatDescriptor formatDescriptor)
	{
		return  MakeFormatFlag(formatDescriptor.ComponentCount, formatDescriptor.ComponentType, formatDescriptor.BitDepth, formatDescriptor.Type, formatDescriptor.A, formatDescriptor.B, formatDescriptor.C, formatDescriptor.D);
	}
	
	enum class Format
	{
		RGB_I8 = MakeFormatFlag(FORMATS::RGB_I8),
		RGBA_I8 = MakeFormatFlag(FORMATS::RGBA_I8),
		RGBA_F16 = MakeFormatFlag(FORMATS::RGBA_F16),
		BGRA_I8 = MakeFormatFlag(FORMATS::BGRA_I8),
		RGBA_F32 = MakeFormatFlag(4, ComponentType::FLOAT, 32, TextureType::COLOR, 0, 1, 2, 3),
		F32 = MakeFormatFlag(1, ComponentType::FLOAT, 32, TextureType::DEPTH, 0, 0, 0, 0)
	};
	
	inline constexpr Format MakeFormatFromFormatDescriptor(const FormatDescriptor formatDescriptor)
	{
		return Format(MakeFormatFlag(formatDescriptor));
	}

	inline FormatDescriptor MakeFormatDescriptorFromFormat(const Format format)
	{
		return *reinterpret_cast<const FormatDescriptor*>(&format);
	}
	
	class RenderDevice;

	using MemoryTypes = GTSL::Flags<GTSL::uint32>;

	namespace MemoryType
	{
		static constexpr MemoryTypes GPU = 1;
		static constexpr MemoryTypes HOST_VISIBLE = 2;
		static constexpr MemoryTypes HOST_COHERENT = 4;
		static constexpr MemoryTypes HOST_CACHED = 8;
	}
	
	struct ShaderStage : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type VERTEX = 1;
		static constexpr value_type TESSELLATION_CONTROL = 2;
		static constexpr value_type TESSELLATION_EVALUATION = 4;
		static constexpr value_type GEOMETRY = 8;
		static constexpr value_type FRAGMENT = 16;
		static constexpr value_type COMPUTE = 32;
		static constexpr value_type TASK = 64;
		static constexpr value_type MESH = 128;
		static constexpr value_type RAY_GEN = 256, ANY_HIT = 512, CLOSEST_HIT = 1024, MISS = 2048, INTERSECTION = 4096, CALLABLE = 8192;
	};

	struct TextureUses : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type TRANSFER_SOURCE = 1, TRANSFER_DESTINATION = 2, SAMPLE = 4, STORAGE = 8, COLOR_ATTACHMENT = 16, DEPTH_STENCIL_ATTACHMENT = 32, TRANSIENT_ATTACHMENT = 64, INPUT_ATTACHMENT = 128;
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
	enum class TextureLayout : GTSL::uint8
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
	
	enum class Dimension : GTSL::uint8
	{
		LINEAR,
		SQUARE,
		CUBE
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
		VERTEX_SHADER,
		TESSELLATION_CONTROL_SHADER,
		TESSELLATION_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,

		COMPUTE_SHADER,

		RAY_GEN, ANY_HIT, CLOSEST_HIT, MISS, INTERSECTION, CALLABLE
	};

	enum class ShaderDataType : GTSL::uint8
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

	enum class WindingOrder : GTSL::uint8
	{
		CLOCKWISE,
		COUNTER_CLOCKWISE
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
		ACCELERATION_STRUCTURE = 11
	};

	enum class PresentModes : GTSL::uint8
	{
		/**
		* \brief All rendered images are queued in FIFO fashion and presented at V-BLANK. Best for when latency is not that important and energy consumption is.
		*/
		FIFO = 0,
		
		/**
		* \brief The last rendered image is the one which will be presented. Best for when latency is important and energy consumption is not.
		*/
		SWAP = 1
	};
	
	inline GTSL::uint8 ShaderDataTypesSize(const ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::FLOAT: return 4;
			case ShaderDataType::FLOAT2: return 8;
			case ShaderDataType::FLOAT3: return 12;
			case ShaderDataType::FLOAT4: return 16;
			case ShaderDataType::INT: return 4;
			case ShaderDataType::INT2: return 8;
			case ShaderDataType::INT3: return 12;
			case ShaderDataType::INT4: return 16;
			case ShaderDataType::BOOL: return 1;
			case ShaderDataType::MAT3: return 36;
			case ShaderDataType::MAT4: return 64;
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
