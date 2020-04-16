#include "GAL/RenderDevice.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

using namespace GAL;

void RenderDevice::GetAvailableRenderAPIs(GTSL::Array<RenderAPI, 16>& renderApis)
{
#if (_WIN32) // PLATFORMS WHICH SUPPORT VULKAN
	if (VulkanRenderDevice::IsVulkanSupported())
	{
		renderApis.EmplaceBack(RenderAPI::VULKAN);
	}
#endif // PLATFORMS WHICH SUPPORT VULKAN

}

RenderDevice* GAL::RenderDevice::CreateRenderDevice(const RenderDeviceCreateInfo& renderDeviceCreateInfo)
{
	//BE_ASSERT(renderDeviceCreateInfo.RenderingAPI == RenderAPI::NONE, "renderApi is RenderAPI::NONE, which is not a valid API, please select another option preferably one of those returned by RenderDevice::GetAvailableRenderAPIs()")

#if (_DEBUG)
	GTSL::Array<RenderAPI, 16> available_render_apis;
	GetAvailableRenderAPIs(available_render_apis);

	auto supported = false;
	for (auto& e : available_render_apis)
	{
		if (e == renderDeviceCreateInfo.RenderingAPI)
		{
			supported = true;
			break;
		}
	}

	//BE_ASSERT(supported, "Chosen Render API is not available. Please query supported APIs with RenderDevice::GetAvailableRenderAPIs()")
#endif


	switch (renderDeviceCreateInfo.RenderingAPI)
	{
	case RenderAPI::VULKAN: return new VulkanRenderDevice(renderDeviceCreateInfo);
	default: return nullptr;
	}
}

void RenderDevice::DestroyRenderDevice(const RenderDevice* renderDevice)
{
	delete renderDevice;
}