#pragma once

#include "GTSL/Core.h"
#include "GTSL/Range.h"

#include <d3d12.h>

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
			handle->SetPrivateData(WKPDID_D3DDebugObjectName, createInfo.Name.ElementCount() - 1, createInfo.Name.begin());
		}
	}

	enum class DX12QueueType
	{
		GRAPHICS = 0,
		COMPUTE = 2,
		TRANSFER = 3,
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
		//DEPTH16 = ,
		////A depth-only format with a 32 (4 byte) bit size.
		//DEPTH32 = ,
		////A depth/stencil format with a 16 bit (2 byte) size depth part and an 8 bit (1 byte) size stencil part.
		//DEPTH16_STENCIL8 = ,
		////A depth/stencil format with a 24 bit (3 byte) size depth part and an 8 bit (1 byte) size stencil part.
		//DEPTH24_STENCIL8 = ,
		////A depth/stencil format with a 32 bit (4 byte) size depth part and an 8 bit (1 byte) size stencil part.
		//DEPTH32_STENCIL8 = 
	};
}