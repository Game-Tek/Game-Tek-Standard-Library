#pragma once

#include "GTSL/Core.h"
#include "GTSL/Range.h"

#include <d3d12.h>

#include "GTSL/Flags.h"

#if (_DEBUG)
#define DX_CHECK(func) if (func < 0) { __debugbreak(); }
#else
#define DX_CHECK(func) func;
#endif

namespace GAL
{
	struct DX12RenderInfo
	{
		class DX12RenderDevice* RenderDevice = nullptr;
	};

	struct DX12CreateInfo : DX12RenderInfo
	{
		GTSL::Range<const GTSL::UTF8*> Name;
	};

	template<typename T>
	void setName(T* handle, const DX12CreateInfo& createInfo)
	{
		if constexpr (_DEBUG)
		{
			if (createInfo.Name.ElementCount() != 0)
			{
				handle->SetPrivateData(WKPDID_D3DDebugObjectName, createInfo.Name.ElementCount() - 1, createInfo.Name.begin());
			}
		}
	}

	struct DX12BufferType : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type	TRANSFER_SOURCE = 0x800, TRANSFER_DESTINATION = 0x400, UNIFORM = 16;
		static constexpr value_type STORAGE = 0x8, INDEX = 0x2, VERTEX = 0x1, ADDRESS = 0/*DX12 has no equivalent, so we set it to 0 so it doesn't affect flags*/;
		static constexpr value_type	RAY_TRACING = 0x400000, INDIRECT = 0x200;
	};

	struct DX12AllocateFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type DEVICE_MASK = 0, DEVICE_ADDRESS = 0, DEVICE_ADDRESS_CAPTURE_REPLAY = 0;
	};

	struct DX12ShaderStage : GTSL::Flags<GTSL::uint32>
	{
		
	};

	struct DX12BindingFlags : GTSL::Flags<GTSL::uint32>
	{
		
	};

	enum class DX12IndexType : GTSL::uint8
	{
		UINT8 = 62, UINT16 = 57, UINT32 = 42
	};

	enum class DX12PipelineType
	{
		RASTER, COMPUTE, RAY_TRACING
	};
	
	enum class DX12TextureLayout : GTSL::uint32
	{
		UNDEFINED = 0xFFFFFFFF,
		GENERAL = 0,
		//D3D12_RESOURCE_STATE_RENDER_TARGET = 0x4,
		COLOR_ATTACHMENT = 0x4,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
		DEPTH_STENCIL_ATTACHMENT = 0x10,
		DEPTH_STENCIL_READ_ONLY = 0x20,
		//D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
		//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
		//D3D12_RESOURCE_STATE_STREAM_OUT = 0x100,
		TRANSFER_DST = 0x400,
		TRANSFER_SRC = 0x800,
		//D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE = 0x1000000,
		PRESENTATION = 0,
		D3D12_RESOURCE_STATE_PREDICATION = 0x200,
	};

	struct DX12AccessFlags : GTSL::Flags<GTSL::uint32>
	{
		static constexpr value_type D3D12_RESOURCE_STATE_COMMON = 0,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
		D3D12_RESOURCE_STATE_INDEX_BUFFER = 0x2,
		D3D12_RESOURCE_STATE_RENDER_TARGET = 0x4,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
		D3D12_RESOURCE_STATE_DEPTH_WRITE = 0x10,
		D3D12_RESOURCE_STATE_DEPTH_READ = 0x20,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
		D3D12_RESOURCE_STATE_STREAM_OUT = 0x100,
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
		D3D12_RESOURCE_STATE_COPY_DEST = 0x400,
		D3D12_RESOURCE_STATE_COPY_SOURCE = 0x800,
		D3D12_RESOURCE_STATE_RESOLVE_DEST = 0x1000,
		D3D12_RESOURCE_STATE_RESOLVE_SOURCE = 0x2000,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x400000,
		D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE = 0x1000000,
		D3D12_RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
		D3D12_RESOURCE_STATE_PRESENT = 0,
		D3D12_RESOURCE_STATE_PREDICATION = 0x200,
		D3D12_RESOURCE_STATE_VIDEO_DECODE_READ = 0x10000,
		D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE = 0x20000,
		D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ = 0x40000,
		D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE = 0x80000,
		D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ = 0x200000,
		D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE = 0x800000;
	};

	enum class DX12BindingType
	{
		
	};
	
	enum class DX12QueueType
	{
		GRAPHICS = 0,
		COMPUTE = 2,
		TRANSFER = 3,
	};

	enum class DX12Dimension
	{
		LINEAR = 2, SQUARE = 3, CUBE = 4
	};

	enum class DX12ShaderDataType
	{
		FLOAT = 41,
		FLOAT2 = 16,
		FLOAT3 = 6,
		FLOAT4 = 2,
		INT = 39,
		INT2 = 48,
		INT3 = 5,
		INT4 = 1,
		BOOL = 60
	};
	
	enum class DX12PresentMode
	{
		/**
		* \brief The last rendered image is the one which will be presented. Best for when latency is important and energy consumption is not.
		*/
		SWAP = 2048,
		/**
		* \brief All rendered images are queued in FIFO fashion and presented at V-BLANK. Best for when latency is not that important and energy consumption is.
		*/
		FIFO = 2048,
	};
	
	enum class DX12TextureFormat
	{
		UNDEFINED = 0,

		//INTEGER

		//R
		R_I8 = 61,
		//R_I16 = ,
		//R_I32 = ,
		//R_I64 = ,
		
		//RG
		RG_I8 = 49,
		//RG_I16 = ,
		//RG_I32 = ,
		//RG_I64 = ,
		
		//RBG
		//RGB_I8 = ,
		//RGB_I16 = ,
		//RGB_I32 = ,
		//RGB_I64 = ,
		
		//RGBA
		RGBA_I8 = 28,
		//RGBA_I16 = ,
		//RGBA_I32 = ,
		//RGBA_I64 = ,
		
		//RGBA
		BGRA_I8 = 87,

		//BGR_I8 = ,

		//  DEPTH STENCIL

		//STENCIL_8 = ,
		//
		////A depth-only format with a 16 bit (2 byte) size.
		DEPTH16 = 54,
		////A depth-only format with a 32 (4 byte) bit size.
		DEPTH32 = 40,
		////A depth/stencil format with a 16 bit (2 byte) size depth part and an 8 bit (1 byte) size stencil part.
		//DEPTH16_STENCIL8 = ,
		////A depth/stencil format with a 24 bit (3 byte) size depth part and an 8 bit (1 byte) size stencil part.
		DEPTH24_STENCIL8 = 45,
		////A depth/stencil format with a 32 bit (4 byte) size depth part and an 8 bit (1 byte) size stencil part.
		//DEPTH32_STENCIL8 = 
	};
}