#pragma once

#include "GTSL/Core.h"
#include "GTSL/Range.h"

#include <d3d12.h>

#if (_DEBUG)
#define DX_CHECK(func) if (func < 0) { __debugbreak(); }
#else
#define DX_CHECK(func) ;
#endif

struct DX12RenderInfo
{
	class DX12RenderDevice* RenderDevice = nullptr;
};

struct DX12CreateInfo : DX12RenderInfo
{
	GTSL::Range<GTSL::UTF8*> Name;
};

template<typename T>
void setName(T* handle, const DX12CreateInfo& createInfo)
{
	if constexpr (_DEBUG)
	{
		handle->SetPrivateData(WKPDID_D3DDebugObjectName, createInfo.Name.ElementCount() - 1, createInfo.Name.begin());
	}
}