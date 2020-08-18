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
	VkSwapchainCreateInfoKHR vk_swapchain_create_info_khr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vk_swapchain_create_info_khr.surface = static_cast<VkSurfaceKHR>(static_cast<const VulkanSurface*>(createInfo.Surface)->GetVkSurface());
	vk_swapchain_create_info_khr.minImageCount = createInfo.DesiredFramesInFlight;
	vk_swapchain_create_info_khr.imageFormat = static_cast<VkFormat>(createInfo.Format);
	vk_swapchain_create_info_khr.imageColorSpace = static_cast<VkColorSpaceKHR>(createInfo.ColorSpace);
	vk_swapchain_create_info_khr.imageExtent = Extent2DToVkExtent2D(createInfo.SurfaceArea);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vk_swapchain_create_info_khr.imageArrayLayers = 1;
	vk_swapchain_create_info_khr.imageUsage = createInfo.ImageUses;
	vk_swapchain_create_info_khr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_swapchain_create_info_khr.queueFamilyIndexCount = 0;
	vk_swapchain_create_info_khr.pQueueFamilyIndices = nullptr;
	vk_swapchain_create_info_khr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vk_swapchain_create_info_khr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vk_swapchain_create_info_khr.presentMode = static_cast<VkPresentModeKHR>(createInfo.PresentMode);
	vk_swapchain_create_info_khr.clipped = VK_TRUE;
	vk_swapchain_create_info_khr.oldSwapchain = nullptr;

	VK_CHECK(vkCreateSwapchainKHR(createInfo.RenderDevice->GetVkDevice(), &vk_swapchain_create_info_khr, createInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<VkSwapchainKHR*>(&swapchain)));
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
	vkSwapchainCreateInfoKhr.surface = reinterpret_cast<VkSurfaceKHR>(resizeInfo.Surface->GetVkSurface());
	vkSwapchainCreateInfoKhr.minImageCount = resizeInfo.DesiredFramesInFlight;
	vkSwapchainCreateInfoKhr.imageFormat = static_cast<VkFormat>(resizeInfo.Format);
	vkSwapchainCreateInfoKhr.imageColorSpace = static_cast<VkColorSpaceKHR>(resizeInfo.ColorSpace);
	vkSwapchainCreateInfoKhr.imageExtent = Extent2DToVkExtent2D(resizeInfo.SurfaceArea);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vkSwapchainCreateInfoKhr.imageArrayLayers = 1;
	vkSwapchainCreateInfoKhr.imageUsage = resizeInfo.ImageUses;
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
	auto vulkan_semaphores = GTSL::Ranger<const VulkanSemaphore>(presentInfo.WaitSemaphores);
	
	GTSL::Array<VkSemaphore, 16> vkWaitSemaphores(vulkan_semaphores.ElementCount());
	for (const auto& e : vulkan_semaphores)
	{
		vkWaitSemaphores[static_cast<GTSL::uint32>(RangeForIndex(e, vulkan_semaphores))] = e.GetVkSemaphore();
	}
	
	VkPresentInfoKHR vkPresentInfoKhr{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	{
		vkPresentInfoKhr.waitSemaphoreCount = vkWaitSemaphores.GetLength();
		vkPresentInfoKhr.pWaitSemaphores = vkWaitSemaphores.begin();	
		vkPresentInfoKhr.swapchainCount = 1;
		vkPresentInfoKhr.pSwapchains = reinterpret_cast<VkSwapchainKHR*>(&swapchain);
		vkPresentInfoKhr.pImageIndices = &presentInfo.ImageIndex;
		vkPresentInfoKhr.pResults = nullptr;
	}

	VK_CHECK(vkQueuePresentKHR(static_cast<const VulkanQueue*>(presentInfo.Queue)->GetVkQueue(), &vkPresentInfoKhr));
}

GTSL::Array<GAL::VulkanImageView, 5> GAL::VulkanRenderContext::GetImages(const GetImagesInfo& getImagesInfo)
{
	GTSL::Array<VulkanImageView, 5> vulkan_images;
	
	GTSL::uint32 swapchain_image_count = 0;
	VK_CHECK(vkGetSwapchainImagesKHR(getImagesInfo.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), &swapchain_image_count, nullptr));
	vulkan_images.Resize(swapchain_image_count);

	GTSL::Array<VkImage, 5> vk_images(swapchain_image_count);
	VK_CHECK(vkGetSwapchainImagesKHR(getImagesInfo.RenderDevice->GetVkDevice(), static_cast<VkSwapchainKHR>(swapchain), &swapchain_image_count, vk_images.begin()));

	GTSL::uint32 i = 0;
	
	for(auto& e : vulkan_images)
	{
		GTSL::StaticString<128> name(getImagesInfo.ImageViewName); name += i;
		
		VkImageViewCreateInfo vk_image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		vk_image_view_create_info.image = vk_images[&e - vulkan_images.begin()];
		vk_image_view_create_info.format = static_cast<VkFormat>(getImagesInfo.SwapchainImagesFormat);
		vk_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vk_image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
		vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
		vk_image_view_create_info.subresourceRange.layerCount = 1;
		vk_image_view_create_info.subresourceRange.levelCount = 1;
		
		VK_CHECK(vkCreateImageView(static_cast<const VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), &vk_image_view_create_info, static_cast<const VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkAllocationCallbacks(), &e.imageView));

		VulkanCreateInfo create_info;
		create_info.RenderDevice = getImagesInfo.RenderDevice;
		create_info.Name = name.begin();
		SET_NAME(e.GetVkImageView(), VK_OBJECT_TYPE_IMAGE_VIEW, create_info);

		++i;
	}
	
	return vulkan_images;
}

GAL::VulkanSurface::VulkanSurface(const CreateInfo& createInfo)
{
	VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfoKhr{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	vkWin32SurfaceCreateInfoKhr.hwnd = static_cast<HWND>(static_cast<WindowsWindowData*>(createInfo.SystemData)->WindowHandle);
	vkWin32SurfaceCreateInfoKhr.hinstance = static_cast<HINSTANCE>(static_cast<WindowsWindowData*>(createInfo.SystemData)->InstanceHandle);
	VK_CHECK(vkCreateWin32SurfaceKHR(createInfo.RenderDevice->GetVkInstance(), &vkWin32SurfaceCreateInfoKhr, createInfo.RenderDevice->GetVkAllocationCallbacks(), reinterpret_cast<VkSurfaceKHR*>(&surface)));
	SET_NAME(surface, VK_OBJECT_TYPE_SURFACE_KHR, createInfo);
}

void GAL::VulkanSurface::Destroy(VulkanRenderDevice* renderDevice)
{
	vkDestroySurfaceKHR(renderDevice->GetVkInstance(), reinterpret_cast<VkSurfaceKHR>(surface), renderDevice->GetVkAllocationCallbacks());
	debugClear(surface);
}

GTSL::uint32 GAL::VulkanSurface::GetSupportedRenderContextFormat(VulkanRenderDevice* renderDevice, GTSL::Ranger<GTSL::Pair<GTSL::uint32, GTSL::uint32>> formats)
{
	GTSL::uint32 surface_formats_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(renderDevice->GetVkPhysicalDevice(), reinterpret_cast<VkSurfaceKHR>(surface), &surface_formats_count, nullptr);

	GTSL::Array<VkSurfaceFormatKHR, 32> surface_formats(surface_formats_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(renderDevice->GetVkPhysicalDevice(), reinterpret_cast<VkSurfaceKHR>(surface), &surface_formats_count, surface_formats.begin());

	for (auto& e : surface_formats)
	{
		for (auto p : formats) { if (e.colorSpace == p.First && e.format == p.Second) { return &e - surface_formats.begin(); } }
	}


	return 0xFFFFFFFF;
}

GTSL::uint32 GAL::VulkanSurface::GetSupportedPresentMode(VulkanRenderDevice* renderDevice, GTSL::Ranger<GTSL::uint32> presentModes)
{
	GTSL::uint32 present_modes_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(renderDevice->GetVkPhysicalDevice(), reinterpret_cast<VkSurfaceKHR>(surface), &present_modes_count, nullptr);
	GTSL::Array<VkPresentModeKHR, 8> present_modes(present_modes_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(renderDevice->GetVkPhysicalDevice(), reinterpret_cast<VkSurfaceKHR>(surface), &present_modes_count, present_modes.begin());

	for (auto& present_mode : present_modes) { for (auto p_m : presentModes) { if (present_mode == p_m) { return &present_mode - present_modes.begin(); } } }

	return 0xFFFFFFFF;
}

bool GAL::VulkanSurface::IsSupported(VulkanRenderDevice* renderDevice)
{
	VkBool32 supported = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(renderDevice->GetVkPhysicalDevice(), 0, reinterpret_cast<VkSurfaceKHR>(surface), &supported);

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderDevice->GetVkPhysicalDevice(), reinterpret_cast<VkSurfaceKHR>(surface), &surface_capabilities);

	return supported;
}
