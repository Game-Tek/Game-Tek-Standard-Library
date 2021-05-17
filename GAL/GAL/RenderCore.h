#pragma once

#include <GTSL/Core.h>

#include <GTSL/Flags.h>

#include <GTSL/Algorithm.h>

#include "GTSL/Bitman.h"
#include "GTSL/Extent.h"
#include "GTSL/RGB.h"

#undef OPAQUE

namespace GAL
{
	class Texture;

	template<typename T>
	constexpr void debugClear(T& handle) { if constexpr (_DEBUG) { handle = reinterpret_cast<T>(0); } }
	
	constexpr GTSL::uint8 MAX_SHADER_STAGES = 8;

	template<GTSL::uint32 FV, GTSL::uint32 TV, typename FVR, typename TVR>
	void TranslateMask(const FVR fromVar, TVR& toVar) {
		GTSL::SetBitAs(GTSL::FindFirstSetBit(TV), static_cast<GTSL::uint32>(fromVar) & FV, static_cast<GTSL::uint32&>(toVar));
	}

	template<GTSL::uint32 FV, GTSL::uint32 TV, typename FVR, typename T>
	void TranslateMask(const FVR fromVar, GTSL::Flags<GTSL::uint8, T>& toVar) {
		GTSL::SetBitAs(GTSL::FindFirstSetBit(TV), static_cast<GTSL::uint8>(static_cast<GTSL::uint32>(fromVar) & FV), static_cast<GTSL::uint8&>(toVar));
	}
	
	template<typename FV, typename TV, typename FVR, typename TVR>
	void TranslateMask(const FV fromValue, const TV toValue, const FVR fromVar, TVR& toVar) {
		GTSL::SetBitAs(GTSL::FindFirstSetBit(toValue), fromVar & fromValue, toVar);
	}	
	
	enum class RenderAPI : GTSL::uint8 {
		VULKAN,
		DIRECTX12
	};

	using MemoryType = GTSL::Flags<GTSL::uint8, struct MemoryTypeTag>;

	namespace MemoryTypes {
		static constexpr MemoryType GPU = 1;
		static constexpr MemoryType HOST_VISIBLE = 2;
		static constexpr MemoryType HOST_COHERENT = 4;
		static constexpr MemoryType HOST_CACHED = 8;
	}
	
	struct MemoryRequirements {
		GTSL::uint32 Size{ 0 };
		GTSL::uint32 Alignment{ 0 };
		MemoryType MemoryTypes;
	};
	
	using PipelineStage = GTSL::Flags<GTSL::uint32, struct PipelineStageTag>;

	namespace PipelineStages {
		static constexpr PipelineStage TOP_OF_PIPE = 1,
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
			HOST = 16384,
			ALL_GRAPHICS = 32768,
			ALL_COMMANDS = 0x00010000,
			RAY_TRACING_SHADER = 0x00200000,
			ACCELERATION_STRUCTURE_BUILD = 0x02000000,
			SHADING_RATE_IMAGE = 0x00400000,
			TASK_SHADER = 0x00080000,
			MESH_SHADER = 0x00100000;
	}

	constexpr GTSL::uint8 RAY_GEN_TABLE_INDEX = 0, HIT_TABLE_INDEX = 1, MISS_TABLE_INDEX = 2, CALLABLE_TABLE_INDEX = 3;

	enum class ComponentType : GTSL::uint8 { INT, UINT, FLOAT };
	enum class TextureType : GTSL::uint8 { COLOR, DEPTH };

	using DeviceAddress = GTSL::uint64;
	
	struct ShaderHandle {
		ShaderHandle() = default;
		ShaderHandle(void* data, GTSL::uint32 size, GTSL::uint32 alignedSize) : Data(data), Size(size), AlignedSize(alignedSize) {}
		void* Data; GTSL::uint32 Size, AlignedSize;
	};

	constexpr GTSL::uint32 MakeFormatFlag(GTSL::uint8 compCount, ComponentType compType, GTSL::uint8 bitDepth, TextureType type, GTSL::uint8 a, GTSL::uint8 b, GTSL::uint8 c, GTSL::uint8 d)
	{
		return GTSL::UnderlyingType<ComponentType>(compType) | compCount << 4 | (a | b << 2 | c << 4 | d << 6) << 8 | bitDepth << 16 | GTSL::UnderlyingType<TextureType>(type) << 22;
	}

	inline GTSL::uint32 SizeFromExtent(const GTSL::Extent3D extent) {
		return extent.Width * extent.Height * extent.Depth;
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

	constexpr GTSL::uint32 MakeFormatFlag(FormatDescriptor formatDescriptor)
	{
		return  MakeFormatFlag(formatDescriptor.ComponentCount, formatDescriptor.ComponentType, formatDescriptor.BitDepth, formatDescriptor.Type, formatDescriptor.A, formatDescriptor.B, formatDescriptor.C, formatDescriptor.D);
	}

	enum class ColorSpace : GTSL::uint8 {
		SRGB
	};
	
	enum class Format
	{
		RGB_I8 = MakeFormatFlag(FORMATS::RGB_I8),
		RGBA_I8 = MakeFormatFlag(FORMATS::RGBA_I8),
		RGBA_F16 = MakeFormatFlag(FORMATS::RGBA_F16),
		BGRA_I8 = MakeFormatFlag(FORMATS::BGRA_I8),
		RGBA_F32 = MakeFormatFlag(4, ComponentType::FLOAT, 32, TextureType::COLOR, 0, 1, 2, 3),
		F32 = MakeFormatFlag(1, ComponentType::FLOAT, 32, TextureType::DEPTH, 0, 0, 0, 0)
	};

	constexpr Format MakeFormatFromFormatDescriptor(const FormatDescriptor formatDescriptor) {
		return static_cast<Format>(MakeFormatFlag(formatDescriptor));
	}

	inline FormatDescriptor MakeFormatDescriptorFromFormat(const Format format) {
		return *reinterpret_cast<const FormatDescriptor*>(&format);
	}
	
	class RenderDevice;
	
	using BindingFlag = GTSL::Flags<GTSL::uint8, struct BindingFlagTag>;
	namespace BindingFlags {
		static constexpr BindingFlag PARTIALLY_BOUND = 1 << 0;
	}
	
	using ShaderStage = GTSL::Flags<GTSL::uint16, struct ShaderStageTag>;
	namespace ShaderStages {
		static constexpr ShaderStage VERTEX = 1,
			TESSELLATION_CONTROL = 2,
			TESSELLATION_EVALUATION = 4,
			GEOMETRY = 8,
			FRAGMENT = 16,
			COMPUTE = 32,
			TASK = 64,
			MESH = 128,
			RAY_GEN = 256, ANY_HIT = 512, CLOSEST_HIT = 1024, MISS = 2048, INTERSECTION = 4096, CALLABLE = 8192;
	};

	using TextureUse = GTSL::Flags<GTSL::uint32, struct TextureUseTag>;
	namespace TextureUses {
		static constexpr TextureUse TRANSFER_SOURCE = 1, TRANSFER_DESTINATION = 2, SAMPLE = 4, STORAGE = 8, ATTACHMENT = 16, TRANSIENT_ATTACHMENT = 32, INPUT_ATTACHMENT = 64;
	}
	
	using QueueType = GTSL::Flags<GTSL::uint8, struct QueueTypeTag>;
	namespace QueueTypes {
		static constexpr QueueType GRAPHICS = 1 << 0, COMPUTE = 1 << 1, TRANSFER = 1 << 2;
	}

	using BufferUse = GTSL::Flags< GTSL::uint32, struct BufferUseFlag>;
	namespace BufferUses {
		static constexpr BufferUse TRANSFER_SOURCE = 1 << 0, TRANSFER_DESTINATION = 1 << 1, STORAGE = 1 << 2, ACCELERATION_STRUCTURE = 1 << 3, ADDRESS = 1 << 4, UNIFORM = 1 << 5, VERTEX = 1 << 6, INDEX = 1 << 7, SHADER_BINDING_TABLE = 1 << 8, BUILD_INPUT_READ = 1 << 9;
	};

	using AllocationFlag = GTSL::Flags<GTSL::uint8, struct AllocationFlagTag>;
	namespace AllocationFlags {
		static constexpr AllocationFlag DEVICE_ADDRESS = 1, DEVICE_ADDRESS_CAPTURE_REPLAY = 2;
	}

	using AccessType = bool;
	
	using AccessFlag = GTSL::Flags<GTSL::uint32, struct AccessFlagTag>;	
	namespace AccessFlags {
		static constexpr AccessFlag INDIRECT_COMMAND_READ = 1 << 0,
		INDEX_READ = 1 << 1,
		VERTEX_ATTRIBUTE_READ = 1 << 2,
		UNIFORM_READ = 1 << 3,
		INPUT_ATTACHMENT_READ = 1 << 4,
		SHADER_READ = 1 << 5,
		SHADER_WRITE = 1 << 6,
		ATTACHMENT_READ = 1 << 7,
		ATTACHMENT_WRITE = 1 << 8,
		TRANSFER_READ = 1 << 11,
		TRANSFER_WRITE = 1 << 12,
		HOST_READ = 1 << 13,
		HOST_WRITE = 1 << 14,
		MEMORY_READ = 1 << 15,
		MEMORY_WRITE = 1 << 16,
		ACCELERATION_STRUCTURE_READ = 1 << 17,
		ACCELERATION_STRUCTURE_WRITE = 1 << 18,
		SHADING_RATE_IMAGE_READ = 1 << 19;
	}
	
	// IMAGE

	//Specifies all available image layouts.
	enum class TextureLayout : GTSL::uint8 {
		UNDEFINED,
		GENERAL,
		ATTACHMENT,
		SHADER_READ,
		TRANSFER_SOURCE,
		TRANSFER_DESTINATION,
		PREINITIALIZED,
		PRESENTATION
	};

	enum class GeometryType {
		TRIANGLES, AABB, INSTANCES
	};

	enum class QueryType {
		COMPACT_ACCELERATION_STRUCTURE_SIZE
	};
	
	using GeometryFlag = GTSL::Flags<GTSL::uint8, struct GeometryFlagTag>;
	namespace GeometryFlags {
		static constexpr GeometryFlag OPAQUE = 1 << 0;
	}
	
	using AccelerationStructureFlag = GTSL::Flags<GTSL::uint8, struct AccelerationStructureFlagTag>;
	namespace AccelerationStructureFlags {
		static constexpr AccelerationStructureFlag ALLOW_UPDATE = 1 << 0, ALLOW_COMPACTION = 1 << 1, PREFER_FAST_TRACE = 1 << 2, PREFER_FAST_BUILD = 1 << 3, LOW_MEMORY = 1 << 4;
	}
	
	enum class Tiling {
		OPTIMAL, LINEAR
	};

	enum class BuildDevice : GTSL::uint8 {
		GPU, HOST, GPU_OR_HOST
	};
	
	// ATTACHMENTS

	//Describes all possible operations a GAL can perform when loading a render target onto a render pass.
	enum class Operations : GTSL::uint8 {
		//We don't care about the previous content of the render target. Behavior is unknown.
		UNDEFINED,
		//We want to load the previous content of the render target.
		DO,
		//We want the render target to be cleared to black for color attachments and to 0 for depth/stencil attachments.
		CLEAR
	};

	enum class SampleCount : GTSL::uint8 {
		SAMPLE_COUNT_1,
		SAMPLE_COUNT_2,
		SAMPLE_COUNT_4,
		SAMPLE_COUNT_8,
		SAMPLE_COUNT_16,
		SAMPLE_COUNT_32,
		SAMPLE_COUNT_64
	};

	// SHADERS

	enum class ShaderLanguage : GTSL::uint8 {
		GLSL, HLSL
	};
	
	enum class ShaderType : GTSL::uint8 {
		VERTEX_SHADER,
		TESSELLATION_CONTROL_SHADER,
		TESSELLATION_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,

		COMPUTE_SHADER,

		RAY_GEN, ANY_HIT, CLOSEST_HIT, MISS, INTERSECTION, CALLABLE
	};

	enum class ShaderDataType : GTSL::uint8 {
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

	enum class CullMode : GTSL::uint8 {
		CULL_NONE,
		CULL_FRONT,
		CULL_BACK
	};

	enum class WindingOrder : GTSL::uint8 {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	enum class BlendOperation : GTSL::uint8 {
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX
	};

	enum class CompareOperation : GTSL::uint8 {
		NEVER,
		LESS,
		EQUAL,
		LESS_OR_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_OR_EQUAL,
		ALWAYS
	};

	enum class StencilCompareOperation : GTSL::uint8 {
		KEEP,
		ZERO,
		REPLACE,
		INCREMENT_AND_CLAMP,
		DECREMENT_AND_CLAMP,
		INVERT,
		INCREMENT_AND_WRAP,
		DECREMENT_AND_WRAP
	};

	enum class BindingType : GTSL::uint8 {
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

	enum class PresentModes : GTSL::uint8 {
		/**
		* \brief All rendered images are queued in FIFO fashion and presented at V-BLANK. Best for when latency is not that important and energy consumption is.
		*/
		FIFO = 0,
		
		/**
		* \brief The last rendered image is the one which will be presented. Best for when latency is important and energy consumption is not.
		*/
		SWAP = 1
	};

	enum class ShaderGroupType {
		GENERAL, TRIANGLES, PROCEDURAL
	};

	enum class IndexType { UINT8, UINT16, UINT32 };

	struct RenderPassTargetDescription {
		Operations LoadOperation, StoreOperation;
		TextureLayout Start, End;
		FormatDescriptor FormatDescriptor;
		const Texture* Texture = nullptr;
		GTSL::RGBA ClearValue;
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

	inline IndexType SizeToIndexType(const GTSL::uint8 size)
	{
		switch (size)
		{
		case 1: return IndexType::UINT8;
		case 2: return IndexType::UINT16;
		case 4: return IndexType::UINT32;
		}
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
