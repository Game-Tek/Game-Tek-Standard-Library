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

GAL::VulkanRenderContext::VulkanRenderContext(const CreateInfo& createInfo)
{
	VkWin32SurfaceCreateInfoKHR vk_win32_surface_create_info_khr{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	vk_win32_surface_create_info_khr.hwnd = static_cast<HWND>(static_cast<WindowsWindowData*>(createInfo.SystemData)->WindowHandle);
	vk_win32_surface_create_info_khr.hinstance = static_cast<HINSTANCE>(static_cast<WindowsWindowData*>(createInfo.SystemData)->InstanceHandle);
	VK_CHECK(vkCreateWin32SurfaceKHR(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkInstance(), &vk_win32_surface_create_info_khr,
		static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), reinterpret_cast<VkSurfaceKHR*>(&surface)));

	VkBool32 supported = 0;
	vkGetPhysicalDeviceSurfaceSupportKHR(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkPhysicalDevice(), 0, reinterpret_cast<VkSurfaceKHR>(surface), &supported);
	
	VkSwapchainCreateInfoKHR vk_swapchain_create_info_khr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vk_swapchain_create_info_khr.surface = reinterpret_cast<VkSurfaceKHR>(surface);
	vk_swapchain_create_info_khr.minImageCount = createInfo.DesiredFramesInFlight;
	vk_swapchain_create_info_khr.imageFormat = static_cast<VkFormat>(createInfo.Format);
	vk_swapchain_create_info_khr.imageColorSpace = static_cast<VkColorSpaceKHR>(createInfo.ColorSpace);
	vk_swapchain_create_info_khr.imageExtent = Extent2DToVkExtent2D(createInfo.SurfaceArea);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vk_swapchain_create_info_khr.imageArrayLayers = 1;
	vk_swapchain_create_info_khr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	vk_swapchain_create_info_khr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_swapchain_create_info_khr.queueFamilyIndexCount = 0;
	vk_swapchain_create_info_khr.pQueueFamilyIndices = nullptr;
	vk_swapchain_create_info_khr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vk_swapchain_create_info_khr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vk_swapchain_create_info_khr.presentMode = static_cast<VkPresentModeKHR>(createInfo.PresentMode);
	vk_swapchain_create_info_khr.clipped = VK_TRUE;
	vk_swapchain_create_info_khr.oldSwapchain = nullptr;

	VK_CHECK(vkCreateSwapchainKHR(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_swapchain_create_info_khr,
		static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), reinterpret_cast<VkSwapchainKHR*>(&swapchain)));
}

void GAL::VulkanRenderContext::Destroy(RenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroySwapchainKHR(vk_render_device->GetVkDevice(), reinterpret_cast<VkSwapchainKHR>(swapchain), vk_render_device->GetVkAllocationCallbacks());
	vkDestroySurfaceKHR(vk_render_device->GetVkInstance(), reinterpret_cast<VkSurfaceKHR>(surface), vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanRenderContext::CheckSupported()
{
}

void GAL::VulkanRenderContext::Recreate(const RecreateInfo& resizeInfo)
{
	VkSwapchainCreateInfoKHR vk_swapchain_create_info_khr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vk_swapchain_create_info_khr.surface = reinterpret_cast<VkSurfaceKHR>(surface);
	vk_swapchain_create_info_khr.minImageCount = resizeInfo.DesiredFramesInFlight;
	vk_swapchain_create_info_khr.imageFormat = static_cast<VkFormat>(resizeInfo.Format);
	vk_swapchain_create_info_khr.imageColorSpace = static_cast<VkColorSpaceKHR>(resizeInfo.ColorSpace);
	vk_swapchain_create_info_khr.imageExtent = Extent2DToVkExtent2D(resizeInfo.SurfaceArea);
	//The imageArrayLayers specifies the amount of layers each image consists of. This is always 1 unless you are developing a stereoscopic 3D application.
	vk_swapchain_create_info_khr.imageArrayLayers = 1;
	vk_swapchain_create_info_khr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	vk_swapchain_create_info_khr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vk_swapchain_create_info_khr.queueFamilyIndexCount = 0;
	vk_swapchain_create_info_khr.pQueueFamilyIndices = nullptr;
	vk_swapchain_create_info_khr.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	vk_swapchain_create_info_khr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vk_swapchain_create_info_khr.presentMode = static_cast<VkPresentModeKHR>(resizeInfo.PresentMode);
	vk_swapchain_create_info_khr.clipped = VK_TRUE;
	vk_swapchain_create_info_khr.oldSwapchain = reinterpret_cast<VkSwapchainKHR>(swapchain);

	vkCreateSwapchainKHR(static_cast<VulkanRenderDevice*>(resizeInfo.RenderDevice)->GetVkDevice(), &vk_swapchain_create_info_khr,
		static_cast<VulkanRenderDevice*>(resizeInfo.RenderDevice)->GetVkAllocationCallbacks(), reinterpret_cast<VkSwapchainKHR*>(&swapchain));
}

bool GAL::VulkanRenderContext::AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo)
{
	GTSL::uint32 image_index = 0;

	auto result = vkAcquireNextImageKHR(static_cast<VulkanRenderDevice*>(acquireNextImageInfo.RenderDevice)->GetVkDevice(), reinterpret_cast<VkSwapchainKHR>(swapchain),
		~0ULL, static_cast<VulkanSemaphore*>(acquireNextImageInfo.Semaphore)->GetVkSemaphore(),
	    acquireNextImageInfo.Fence ? static_cast<VulkanFence*>(acquireNextImageInfo.Fence)->GetVkFence() : nullptr, &image_index);

	imageIndex = static_cast<GTSL::uint8>(image_index);

	return result != VK_SUCCESS;
}

void GAL::VulkanRenderContext::Present(const PresentInfo& presentInfo)
{
	GTSL::uint32 image_index = imageIndex;

	GTSL::Array<VkSemaphore, 16> vk_wait_semaphores(static_cast<GTSL::uint32>(presentInfo.WaitSemaphores.ElementCount()));
	for (const Semaphore& e : presentInfo.WaitSemaphores)
	{
		vk_wait_semaphores[static_cast<GTSL::uint32>(&e - presentInfo.WaitSemaphores.begin())] = static_cast<const VulkanSemaphore&>(e).GetVkSemaphore();
	}
	
	VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	{
		present_info.waitSemaphoreCount = vk_wait_semaphores.GetLength();
		present_info.pWaitSemaphores = vk_wait_semaphores.begin();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = reinterpret_cast<VkSwapchainKHR*>(&swapchain);
		present_info.pImageIndices = &image_index;
		present_info.pResults = nullptr;
	}

	vkQueuePresentKHR(static_cast<VulkanQueue*>(presentInfo.Queue)->GetVkQueue(), &present_info);

	imageIndex = (imageIndex + 1) % maxFramesInFlight;
}

GTSL::Array<GAL::VulkanImageView, 5> GAL::VulkanRenderContext::GetImages(const GetImagesInfo& getImagesInfo)
{
	GTSL::Array<VulkanImageView, 5> vulkan_images;
	
	GTSL::uint32 swapchain_image_count = 0;
	vkGetSwapchainImagesKHR(static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), reinterpret_cast<VkSwapchainKHR>(swapchain), &swapchain_image_count, nullptr);
	vulkan_images.Resize(swapchain_image_count);
	
	maxFramesInFlight = static_cast<GTSL::uint8>(swapchain_image_count);

	GTSL::Array<VkImage, 5> vk_images(swapchain_image_count);
	VK_CHECK(vkGetSwapchainImagesKHR(static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), reinterpret_cast<VkSwapchainKHR>(swapchain),
		&swapchain_image_count, vk_images.begin()));

	for(auto& e : vulkan_images)
	{
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
		
		VK_CHECK(vkCreateImageView(static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkDevice(), &vk_image_view_create_info, static_cast<VulkanRenderDevice*>(getImagesInfo.RenderDevice)->GetVkAllocationCallbacks(), &e.imageView));
	}
	
	return vulkan_images;
}
