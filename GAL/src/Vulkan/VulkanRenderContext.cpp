#include "GAL/Vulkan/VulkanRenderContext.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#if (_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#include <GAL/ext/vulkan/vulkan_win32.h>
#endif

#include "GAL/Vulkan/VulkanCommandBuffer.h"

VkSurfaceFormatKHR GAL::VulkanRenderContext::findFormat(VulkanRenderDevice* vulkanRenderDevice, VkSurfaceKHR surface)
{
	VkPhysicalDevice pd = vulkanRenderDevice->GetVkPhysicalDevice();
	
	GTSL::uint32 formats_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formats_count, nullptr);
	GTSL::Array<VkSurfaceFormatKHR, 50, GTSL::uint8> supported_surface_formats(formats_count);
	formats_count = supported_surface_formats.GetCapacity();
	vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formats_count, supported_surface_formats.GetData());
	
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

VkPresentModeKHR GAL::VulkanRenderContext::findPresentMode(const VkPhysicalDevice _PD, VkSurfaceKHR _Surface)
{
	GTSL::uint32 present_modes_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_PD, _Surface, &present_modes_count, nullptr);
	GTSL::Array<VkPresentModeKHR, 10, GTSL::uint8> supported_present_modes(present_modes_count);
	present_modes_count = supported_present_modes.GetCapacity();
	vkGetPhysicalDeviceSurfacePresentModesKHR(_PD, _Surface, &present_modes_count, supported_present_modes.GetData());

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

GAL::VulkanRenderContext::VulkanRenderContext(const CreateInfo& createInfo) : RenderContext(createInfo)
{
	//BE_ASSERT(renderContextCreateInfo.DesiredFramesInFlight > vulkanSwapchainImages.GetCapacity(), "Requested swapchain image count is more than what the engine can handle, please request less.")
	
	extent = createInfo.SurfaceArea;
	
	VkWin32SurfaceCreateInfoKHR vk_win32_surface_create_info_khr{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	vk_win32_surface_create_info_khr.hwnd = static_cast<HWND>(static_cast<WindowsWindowData*>(createInfo.SystemData)->WindowHandle);
	vk_win32_surface_create_info_khr.hinstance = static_cast<HINSTANCE>(static_cast<WindowsWindowData*>(createInfo.SystemData)->InstanceHandle);
	VK_CHECK(vkCreateWin32SurfaceKHR(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkInstance(), &vk_win32_surface_create_info_khr, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &surface));

	surfaceFormat = findFormat(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice), surface);
	
	presentMode = findPresentMode(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkPhysicalDevice(), surface);
	
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
	
	VkSemaphoreCreateInfo vk_semaphore_create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	VkFenceCreateInfo vk_fence_create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vk_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (GTSL::uint8 i = 0; i < maxFramesInFlight; ++i)
	{		
		VK_CHECK(vkCreateSemaphore(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_semaphore_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &imagesAvailable[i]));
		VK_CHECK(vkCreateSemaphore(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_semaphore_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &rendersFinished[i]));
		VK_CHECK(vkCreateFence(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_fence_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &inFlightFences[i]));
	}
}

void GAL::VulkanRenderContext::Destroy(RenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroySwapchainKHR(vk_render_device->GetVkDevice(), swapchain, vk_render_device->GetVkAllocationCallbacks());
	vkDestroySurfaceKHR(vk_render_device->GetVkInstance(), surface, vk_render_device->GetVkAllocationCallbacks());

	for(GTSL::uint8 i = 0; i < maxFramesInFlight; ++i)
	{
		vkDestroySemaphore(vk_render_device->GetVkDevice(), imagesAvailable[i], vk_render_device->GetVkAllocationCallbacks());
		vkDestroySemaphore(vk_render_device->GetVkDevice(), rendersFinished[i], vk_render_device->GetVkAllocationCallbacks());
		vkDestroyFence(vk_render_device->GetVkDevice(), inFlightFences[i], vk_render_device->GetVkAllocationCallbacks());
	}
}

void GAL::VulkanRenderContext::OnResize(const ResizeInfo& _RI)
{
	extent = _RI.NewWindowSize;

	VkSwapchainCreateInfoKHR vk_swapchain_create_info_khr{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	vk_swapchain_create_info_khr.surface = surface;
	vk_swapchain_create_info_khr.minImageCount = maxFramesInFlight;
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
	vk_swapchain_create_info_khr.oldSwapchain = swapchain;

	vkCreateSwapchainKHR(static_cast<VulkanRenderDevice*>(_RI.RenderDevice)->GetVkDevice(), &vk_swapchain_create_info_khr, static_cast<VulkanRenderDevice*>(_RI.RenderDevice)->GetVkAllocationCallbacks(), &swapchain);
}

void GAL::VulkanRenderContext::AcquireNextImage(const AcquireNextImageInfo& acquireNextImageInfo)
{
	GTSL::uint32 image_index = 0;

	vkAcquireNextImageKHR(static_cast<VulkanRenderDevice*>(acquireNextImageInfo.RenderDevice)->GetVkDevice(), swapchain, ~0ULL, imagesAvailable[currentImage], nullptr, &image_index);

	//This signals the semaphore when the image becomes available
	imageIndex = image_index;
}

void GAL::VulkanRenderContext::Flush(const FlushInfo& flushInfo)
{
	vkWaitForFences(static_cast<VulkanRenderDevice*>(flushInfo.RenderDevice)->GetVkDevice(), 1, &inFlightFences[currentImage], true, ~0ULL);//Get current's frame fences and wait for it.
	vkResetFences(static_cast<VulkanRenderDevice*>(flushInfo.RenderDevice)->GetVkDevice(), 1, &inFlightFences[currentImage]); //Then reset it.

	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	//Set current's frame ImageAvaiable semaphore as the semaphore to wait for to start rendering to.
	//Set current's frame RenderFinished semaphore as the semaphore to signal once rendering has finished.

	auto command_buffer = static_cast<VulkanCommandBuffer*>(flushInfo.CommandBuffer)->GetVkCommandBuffer();

	/* Submit signal semaphore to graphics queue */
	VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	{
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &imagesAvailable[currentImage];
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &rendersFinished[currentImage];

		submit_info.pWaitDstStageMask = wait_stages;
	}

	vkQueueSubmit(reinterpret_cast<VulkanQueue*>(flushInfo.Queue)->GetVkQueue(), 1, &submit_info, inFlightFences[currentImage]);

	//Signal fence when execution of this queue has finished.
	vkWaitForFences(static_cast<VulkanRenderDevice*>(flushInfo.RenderDevice)->GetVkDevice(), 1, &inFlightFences[currentImage], true, ~0ULL);

	vkResetCommandBuffer(command_buffer, 0);
}

void GAL::VulkanRenderContext::Present(const PresentInfo& presentInfo)
{
	VkSemaphore wait_semaphores[] = { rendersFinished[currentImage] };

	GTSL::uint32 image_index = imageIndex;

	VkPresentInfoKHR present_info = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	{
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = wait_semaphores;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		present_info.pResults = nullptr;
	}

	vkQueuePresentKHR(static_cast<VulkanQueue*>(presentInfo.Queue)->GetVkQueue(), &present_info);

	currentImage = (currentImage + 1) % maxFramesInFlight;
}

GTSL::Array<GAL::VulkanRenderTarget, 5> GAL::VulkanRenderContext::GetRenderTargets(const RenderTargetsInfo& renderTargetsInfo)
{
	GTSL::Array<VulkanRenderTarget, 5> render_targets;
	
	GTSL::uint32 swapchain_image_count = 0;
	vkGetSwapchainImagesKHR(static_cast<VulkanRenderDevice*>(renderTargetsInfo.RenderDevice)->GetVkDevice(), swapchain, &swapchain_image_count, nullptr);
	render_targets.Resize(swapchain_image_count);

	GTSL::Array<VkImage, 5> vk_images(swapchain_image_count);
	VK_CHECK(vkGetSwapchainImagesKHR(static_cast<VulkanRenderDevice*>(renderTargetsInfo.RenderDevice)->GetVkDevice(), swapchain, &swapchain_image_count, vk_images.GetData()));

	for(auto& e : render_targets)
	{
		VkImageViewCreateInfo vk_image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		vk_image_view_create_info.format = surfaceFormat.format;
		vk_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vk_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vk_image_view_create_info.subresourceRange.baseArrayLayer = 0;
		vk_image_view_create_info.subresourceRange.baseMipLevel = 0;
		vk_image_view_create_info.subresourceRange.layerCount = 1;
		vk_image_view_create_info.subresourceRange.levelCount = 1;
		
		VK_CHECK(vkCreateImageView(static_cast<VulkanRenderDevice*>(renderTargetsInfo.RenderDevice)->GetVkDevice(), &vk_image_view_create_info, static_cast<VulkanRenderDevice*>(renderTargetsInfo.RenderDevice)->GetVkAllocationCallbacks(), &e.imageView));

		e.image = vk_images[&e - render_targets.begin()];
	}
	
	return render_targets;
}
