#include "GAL/Vulkan/VulkanRenderContext.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#if (_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GAL/ext/vulkan/vulkan_win32.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include "GAL/Vulkan/VulkanSynchronization.h"

GAL::VulkanRenderContext::VulkanRenderContext(const CreateInfo& createInfo)
{
	VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKhr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vkSwapchainCreateInfoKhr.surface = static_cast<VkSurfaceKHR>(static_cast<const VulkanSurface*>(createInfo.Surface)->GetVkSurface());
	vkSwapchainCreateInfoKhr.minImageCount = createInfo.DesiredFramesInFlight;
	vkSwapchainCreateInfoKhr.imageFormat = static_cast<VkFormat>(createInfo.Format);
	vkSwapchainCreateInfoKhr.imageColorSpace = static_cast<VkColorSpaceKHR>(createInfo.ColorSpace);
	vkSwapchainCreateInfoKhr.imageExtent = Extent2DToVkExtent2D(createInfo.SurfaceArea);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vkSwapchainCreateInfoKhr.imageArrayLayers = 1;
	vkSwapchainCreateInfoKhr.imageUsage = createInfo.TextureUses;
	vkSwapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkSwapchainCreateInfoKhr.queueFamilyIndexCount = 0;
	vkSwapchainCreateInfoKhr.pQueueFamilyIndices = nullptr;
	vkSwapchainCreateInfoKhr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vkSwapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vkSwapchainCreateInfoKhr.presentMode = static_cast<VkPresentModeKHR>(createInfo.PresentMode);
	vkSwapchainCreateInfoKhr.clipped = VK_TRUE;
	vkSwapchainCreateInfoKhr.oldSwapchain = nullptr;

	VK_CHECK(vkCreateSwapchainKHR(createInfo.RenderDevice->GetVkDevice(), &vkSwapchainCreateInfoKhr, createInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<VkSwapchainKHR*>(&swapchain)));
	SET_NAME(swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, createInfo);
}

void GAL::VulkanRenderContext::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroySwapchainKHR(renderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), renderDevice->GetVkAllocationCallbacks());
	debugClear(swapchain);
}

void GAL::VulkanRenderContext::Recreate(const RecreateInfo& resizeInfo)
{
	VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKhr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vkSwapchainCreateInfoKhr.surface = static_cast<VkSurfaceKHR>(resizeInfo.Surface->GetVkSurface());
	vkSwapchainCreateInfoKhr.minImageCount = resizeInfo.DesiredFramesInFlight;
	vkSwapchainCreateInfoKhr.imageFormat = static_cast<VkFormat>(resizeInfo.Format);
	vkSwapchainCreateInfoKhr.imageColorSpace = static_cast<VkColorSpaceKHR>(resizeInfo.ColorSpace);
	vkSwapchainCreateInfoKhr.imageExtent = Extent2DToVkExtent2D(resizeInfo.SurfaceArea);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vkSwapchainCreateInfoKhr.imageArrayLayers = 1;
	vkSwapchainCreateInfoKhr.imageUsage = resizeInfo.TextureUses;
	vkSwapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkSwapchainCreateInfoKhr.queueFamilyIndexCount = 0;
	vkSwapchainCreateInfoKhr.pQueueFamilyIndices = nullptr;
	vkSwapchainCreateInfoKhr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vkSwapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vkSwapchainCreateInfoKhr.presentMode = static_cast<VkPresentModeKHR>(resizeInfo.PresentMode);
	vkSwapchainCreateInfoKhr.clipped = VK_TRUE;
	vkSwapchainCreateInfoKhr.oldSwapchain = static_cast<VkSwapchainKHR>(swapchain);

	VK_CHECK(vkCreateSwapchainKHR(resizeInfo.RenderDevice->GetVkDevice(), &vkSwapchainCreateInfoKhr, resizeInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<VkSwapchainKHR*>(&swapchain)));
	SET_NAME(swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, resizeInfo);
}

GTSL::uint8 GAL::VulkanRenderContext::AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo)
{
	GTSL::uint32 image_index = 0;

	VK_CHECK(vkAcquireNextImageKHR(acquireNextImageInfo.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain),	~0ULL, acquireNextImageInfo.SignalSemaphore->GetVkSemaphore(), acquireNextImageInfo.Fence ? acquireNextImageInfo.Fence->GetVkFence() : nullptr, &image_index));

	return static_cast<GTSL::uint8>(image_index);
}

void GAL::VulkanRenderContext::Present(const PresentInfo& presentInfo)
{	
	VkPresentInfoKHR vkPresentInfoKhr{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	{
		vkPresentInfoKhr.waitSemaphoreCount = presentInfo.WaitSemaphores.ElementCount();
		vkPresentInfoKhr.pWaitSemaphores = reinterpret_cast<const VkSemaphore*>(presentInfo.WaitSemaphores.begin());
		vkPresentInfoKhr.swapchainCount = 1;
		vkPresentInfoKhr.pSwapchains = reinterpret_cast<VkSwapchainKHR*>(&swapchain);
		vkPresentInfoKhr.pImageIndices = &presentInfo.ImageIndex;
		vkPresentInfoKhr.pResults = nullptr;
	}

	VK_CHECK(vkQueuePresentKHR(static_cast<const VulkanQueue*>(presentInfo.Queue)->GetVkQueue(), &vkPresentInfoKhr));
}

GTSL::Array<GAL::VulkanTextureView, 8> GAL::VulkanRenderContext::GetTextureViews(const GetTextureViewsInfo& getTextureViewsInfo) const
{
	GTSL::Array<VulkanTextureView, 8> vulkanTextureViews;
	
	GTSL::uint32 swapchainImageCount = 0;
	VK_CHECK(vkGetSwapchainImagesKHR(getTextureViewsInfo.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), &swapchainImageCount, nullptr))
	vulkanTextureViews.Resize(swapchainImageCount);

	GTSL::Array<VkImage, 8> vkImages(swapchainImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(getTextureViewsInfo.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), &swapchainImageCount, vkImages.begin()))

	GTSL::uint32 i = 0;
	
	for(auto& e : vulkanTextureViews)
	{		
		VkImageViewCreateInfo vkImageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		vkImageViewCreateInfo.image = vkImages[&e - vulkanTextureViews.begin()];
		vkImageViewCreateInfo.format = static_cast<VkFormat>(getTextureViewsInfo.TextureViewCreateInfos[i].Format);
		vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		vkImageViewCreateInfo.subresourceRange.layerCount = 1;
		vkImageViewCreateInfo.subresourceRange.levelCount = 1;
		
		VK_CHECK(vkCreateImageView(getTextureViewsInfo.RenderDevice->GetVkDevice(), &vkImageViewCreateInfo, getTextureViewsInfo.RenderDevice->GetVkAllocationCallbacks(), &e.imageView))

		if constexpr (_DEBUG)
		{
			VulkanCreateInfo vulkanCreateInfo;
			vulkanCreateInfo.RenderDevice = getTextureViewsInfo.RenderDevice;
			vulkanCreateInfo.Name = getTextureViewsInfo.TextureViewCreateInfos[i].Name;
			SET_NAME(e.GetVkImageView(), VK_OBJECT_TYPE_IMAGE_VIEW, vulkanCreateInfo)
		}

		++i;
	}
	
	return vulkanTextureViews;
}

GTSL::Array<GAL::VulkanTexture, 8> GAL::VulkanRenderContext::GetTextures(const GetTexturesInfo& info) const
{
	GTSL::uint32 swapchainImageCount = 0;
	VK_CHECK(vkGetSwapchainImagesKHR(info.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), &swapchainImageCount, nullptr))

	GTSL::Array<VulkanTexture, 8> vkImages(swapchainImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(info.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), &swapchainImageCount, reinterpret_cast<VkImage*>(vkImages.begin())))

	return vkImages;
}

GAL::VulkanSurface::VulkanSurface(const CreateInfo& createInfo)
{
	VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfoKhr{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	vkWin32SurfaceCreateInfoKhr.hwnd = static_cast<HWND>(createInfo.SystemData->WindowHandle);
	vkWin32SurfaceCreateInfoKhr.hinstance = static_cast<HINSTANCE>(createInfo.SystemData->InstanceHandle);
	VK_CHECK(vkCreateWin32SurfaceKHR(createInfo.RenderDevice->GetVkInstance(), &vkWin32SurfaceCreateInfoKhr, createInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<VkSurfaceKHR*>(&surface)));
	SET_NAME(surface, VK_OBJECT_TYPE_SURFACE_KHR, createInfo);
}

void GAL::VulkanSurface::Destroy(VulkanRenderDevice* renderDevice)
{
	vkDestroySurfaceKHR(renderDevice->GetVkInstance(), static_cast<VkSurfaceKHR>(surface), renderDevice->GetVkAllocationCallbacks());
	debugClear(surface);
}

GTSL::Array<GTSL::Pair<GAL::VulkanColorSpace, GAL::VulkanTextureFormat>, 16> GAL::VulkanSurface::GetSupportedFormatsAndColorSpaces(const VulkanRenderDevice* renderDevice) const
{
	GTSL::uint32 surfaceFormatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(renderDevice->GetVkPhysicalDevice(), static_cast<VkSurfaceKHR>(surface), &surfaceFormatsCount, nullptr);

	GTSL::Array<VkSurfaceFormatKHR, 32> surfaceFormats(surfaceFormatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(renderDevice->GetVkPhysicalDevice(), static_cast<VkSurfaceKHR>(surface), &surfaceFormatsCount, surfaceFormats.begin());

	GTSL::Array<GTSL::Pair<VulkanColorSpace, VulkanTextureFormat>, 16> result;
	
	for (auto e : surfaceFormats) {
		result.EmplaceBack(static_cast<VulkanColorSpace>(e.colorSpace), static_cast<VulkanTextureFormat>(e.format));
	}

	return result;
}

GTSL::Array<GAL::VulkanPresentMode, 4> GAL::VulkanSurface::GetSupportedPresentModes(VulkanRenderDevice* renderDevice) const
{
	GTSL::uint32 presentModesCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(renderDevice->GetVkPhysicalDevice(), static_cast<VkSurfaceKHR>(surface), &presentModesCount, nullptr);
	GTSL::Array<VkPresentModeKHR, 8> vkPresentModes(presentModesCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(renderDevice->GetVkPhysicalDevice(), static_cast<VkSurfaceKHR>(surface), &presentModesCount, vkPresentModes.begin());

	GTSL::Array<GAL::VulkanPresentMode, 4> result;
	
	for (auto presentMode : vkPresentModes) {
		result.EmplaceBack(static_cast<VulkanPresentMode>(presentMode));
	}

	return result;
}

bool GAL::VulkanSurface::IsSupported(class VulkanRenderDevice* renderDevice, SurfaceCapabilities* surfaceCapabilities)
{
	VkBool32 supported = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(renderDevice->GetVkPhysicalDevice(), 0, static_cast<VkSurfaceKHR>(surface), &supported);

	VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKhr;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderDevice->GetVkPhysicalDevice(), static_cast<VkSurfaceKHR>(surface), &vkSurfaceCapabilitiesKhr);

	auto vkExtentToExtent = [](const VkExtent2D vkExtent) { return GTSL::Extent2D(vkExtent.width, vkExtent.height); };
	
	surfaceCapabilities->CurrentExtent = vkExtentToExtent(vkSurfaceCapabilitiesKhr.currentExtent);
	surfaceCapabilities->MinImageExtent = vkExtentToExtent(vkSurfaceCapabilitiesKhr.minImageExtent);
	surfaceCapabilities->MaxImageExtent = vkExtentToExtent(vkSurfaceCapabilitiesKhr.maxImageExtent);
	surfaceCapabilities->MinImageCount = vkSurfaceCapabilitiesKhr.minImageCount;
	surfaceCapabilities->MaxImageCount = vkSurfaceCapabilitiesKhr.maxImageCount;
	surfaceCapabilities->SupportedUsageFlags = vkSurfaceCapabilitiesKhr.supportedUsageFlags;

	return supported;
}
