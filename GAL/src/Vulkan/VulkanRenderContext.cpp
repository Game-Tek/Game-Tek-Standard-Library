#include "GAL/Vulkan/VulkanRenderContext.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#if (_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GAL/ext/vulkan/vulkan_win32.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include "GAL/Vulkan/VulkanCommandBuffer.h"
#include "GAL/Vulkan/VulkanSynchronization.h"

VkSurfaceFormatKHR GAL::VulkanRenderContext::findFormat(VulkanRenderDevice* vulkanRenderDevice, VkSurfaceKHR surface)
{
	VkPhysicalDevice pd = vulkanRenderDevice->GetVkPhysicalDevice();
	
	GTSL::uint32 formats_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formats_count, nullptr);
	GTSL::Array<VkSurfaceFormatKHR, 50> supported_surface_formats(formats_count);
	formats_count = supported_surface_formats.GetCapacity();
	vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formats_count, supported_surface_formats.begin());
	
	//NASTY, REMOVE
	VkBool32 supports = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(pd, 0, surface, &supports);
	//NASTY, REMOVE

	VkSurfaceCapabilitiesKHR SurfaceCapabilities{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, surface, &SurfaceCapabilities);

	VkBool32 supported = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(pd, 0, surface, &supported);

	return supported_surface_formats[0];
}

VkPresentModeKHR GAL::VulkanRenderContext::findPresentMode(const VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	GTSL::uint32 present_modes_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_modes_count, nullptr);
	GTSL::Array<VkPresentModeKHR, 10> supported_present_modes(present_modes_count);
	present_modes_count = supported_present_modes.GetCapacity();
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_modes_count, supported_present_modes.begin());

	GTSL::uint8 best_score = 0;
	VkPresentModeKHR best_present_mode{};
	for (auto& e : supported_present_modes)
	{
		GTSL::uint8 score = 0;
		switch (e)
		{
		case VK_PRESENT_MODE_MAILBOX_KHR: score = 255; break;
		case VK_PRESENT_MODE_FIFO_KHR: score = 254; break;
		case VK_PRESENT_MODE_IMMEDIATE_KHR: score = 253; break;
		default: score = 0;
		}
		
		if (score > best_score)
		{
			best_score = score;
			best_present_mode = e;
		}
	}

	return best_present_mode;
}

GAL::VulkanRenderContext::VulkanRenderContext(const CreateInfo& createInfo)
{
	//BE_ASSERT(renderContextCreateInfo.DesiredFramesInFlight > vulkanSwapchainImages.GetCapacity(), "Requested swapchain image count is more than what the engine can handle, please request less.")
	
	extent = createInfo.SurfaceArea;
	
	VkWin32SurfaceCreateInfoKHR vk_win32_surface_create_info_khr{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	vk_win32_surface_create_info_khr.hwnd = static_cast<HWND>(static_cast<WindowsWindowData*>(createInfo.SystemData)->WindowHandle);
	vk_win32_surface_create_info_khr.hinstance = static_cast<HINSTANCE>(static_cast<WindowsWindowData*>(createInfo.SystemData)->InstanceHandle);
	VK_CHECK(vkCreateWin32SurfaceKHR(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkInstance(), &vk_win32_surface_create_info_khr, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &surface));

	surfaceFormat = findFormat(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice), surface);
	
	presentMode = PresentModeToVkPresentModeKHR(createInfo.PresentMode);
	
	VkSwapchainCreateInfoKHR vk_swapchain_create_info_khr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vk_swapchain_create_info_khr.surface = surface;
	vk_swapchain_create_info_khr.minImageCount = createInfo.DesiredFramesInFlight;
	vk_swapchain_create_info_khr.imageFormat = surfaceFormat.format;
	vk_swapchain_create_info_khr.imageColorSpace = surfaceFormat.colorSpace;
	vk_swapchain_create_info_khr.imageExtent = Extent2DToVkExtent2D(extent);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vk_swapchain_create_info_khr.imageArrayLayers = 1;
	vk_swapchain_create_info_khr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	vk_swapchain_create_info_khr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_swapchain_create_info_khr.queueFamilyIndexCount = 0;
	vk_swapchain_create_info_khr.pQueueFamilyIndices = nullptr;
	vk_swapchain_create_info_khr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vk_swapchain_create_info_khr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vk_swapchain_create_info_khr.presentMode = presentMode;
	vk_swapchain_create_info_khr.clipped = VK_TRUE;
	vk_swapchain_create_info_khr.oldSwapchain = nullptr;

	VK_CHECK(vkCreateSwapchainKHR(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_swapchain_create_info_khr, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &swapchain));
}

void GAL::VulkanRenderContext::Destroy(RenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroySwapchainKHR(vk_render_device->GetVkDevice(), swapchain, vk_render_device->GetVkAllocationCallbacks());
	vkDestroySurfaceKHR(vk_render_device->GetVkInstance(), surface, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanRenderContext::Recreate(const RecreateInfo& resizeInfo)
{
	extent = resizeInfo.NewWindowSize;

	VkSwapchainCreateInfoKHR vk_swapchain_create_info_khr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vk_swapchain_create_info_khr.surface = surface;
	vk_swapchain_create_info_khr.minImageCount = resizeInfo.DesiredFramesInFlight;
	vk_swapchain_create_info_khr.imageFormat = surfaceFormat.format;
	vk_swapchain_create_info_khr.imageColorSpace = surfaceFormat.colorSpace;
	vk_swapchain_create_info_khr.imageExtent = Extent2DToVkExtent2D(extent);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vk_swapchain_create_info_khr.imageArrayLayers = 1;
	vk_swapchain_create_info_khr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	vk_swapchain_create_info_khr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_swapchain_create_info_khr.queueFamilyIndexCount = 0;
	vk_swapchain_create_info_khr.pQueueFamilyIndices = nullptr;
	vk_swapchain_create_info_khr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vk_swapchain_create_info_khr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vk_swapchain_create_info_khr.presentMode = PresentModeToVkPresentModeKHR(resizeInfo.NewPresentMode);
	vk_swapchain_create_info_khr.clipped = VK_TRUE;
	vk_swapchain_create_info_khr.oldSwapchain = swapchain;

	vkCreateSwapchainKHR(static_cast<VulkanRenderDevice*>(resizeInfo.RenderDevice)->GetVkDevice(), &vk_swapchain_create_info_khr, static_cast<VulkanRenderDevice*>(resizeInfo.RenderDevice)->GetVkAllocationCallbacks(), &swapchain);
}

bool GAL::VulkanRenderContext::AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo)
{
	GTSL::uint32 image_index = 0;

	auto result = vkAcquireNextImageKHR(static_cast<VulkanRenderDevice*>(acquireNextImageInfo.RenderDevice)->GetVkDevice(), swapchain, ~0ULL, static_cast<VulkanSemaphore*>(acquireNextImageInfo.Semaphore)->GetVkSemaphore(),
	                                    static_cast<VulkanFence*>(acquireNextImageInfo.Fence)->GetVkFence(), &image_index);
	imageIndex = image_index;

	return result != VK_SUCCESS;
}

void GAL::VulkanRenderContext::Present(const PresentInfo& presentInfo)
{
	GTSL::uint32 image_index = imageIndex;

	GTSL::Array<VkSemaphore, 16> vk_wait_semaphores(presentInfo.WaitSemaphores.ElementCount());
	{
		for (auto& e : presentInfo.WaitSemaphores)
		{
			vk_wait_semaphores[&e - presentInfo.WaitSemaphores.begin()] = static_cast<VulkanSemaphore&>(e).GetVkSemaphore();
		}
	}
	
	VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	{
		present_info.waitSemaphoreCount = vk_wait_semaphores.GetLength();
		present_info.pWaitSemaphores = vk_wait_semaphores.begin();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		present_info.pResults = nullptr;
	}

	vkQueuePresentKHR(static_cast<VulkanQueue*>(presentInfo.Queue)->GetVkQueue(), &present_info);

	imageIndex = (imageIndex + 1) % maxFramesInFlight;
}

GTSL::Array<GAL::VulkanImage, 5> GAL::VulkanRenderContext::GetImages(const GetImagesInfo& getImagesInfo)
{
	GTSL::Array<VulkanImage, 5> vulkan_images;
	
	GTSL::uint32 swapchain_image_count = 0;
	maxFramesInFlight = static_cast<GTSL::uint8>(swapchain_image_count);
	vkGetSwapchainImagesKHR(static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), swapchain, &swapchain_image_count, nullptr);
	vulkan_images.Resize(swapchain_image_count);

	GTSL::Array<VkImage, 5> vk_images(swapchain_image_count);
	VK_CHECK(vkGetSwapchainImagesKHR(static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), swapchain, &swapchain_image_count, vk_images.begin()));

	for(auto& e : vulkan_images)
	{
		VkImageViewCreateInfo vk_image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		vk_image_view_create_info.format = surfaceFormat.format;
		vk_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vk_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
		vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
		vk_image_view_create_info.subresourceRange.layerCount = 1;
		vk_image_view_create_info.subresourceRange.levelCount = 1;
		
		VK_CHECK(vkCreateImageView(static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), &vk_image_view_create_info, static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkAllocationCallbacks(), &e.imageView));

		e.image = vk_images[&e - vulkan_images.begin()];
	}
	
	return vulkan_images;
}
