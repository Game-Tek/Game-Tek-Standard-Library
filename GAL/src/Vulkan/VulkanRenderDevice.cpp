#include "GAL/Vulkan/VulkanRenderDevice.h"

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GAL/ext/vulkan/vulkan_win32.h>
#endif

#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanCommandBuffer.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanSynchronization.h"
#include "GTSL/Console.h"
#include "GTSL/StaticString.hpp"

GTSL::uint32 GAL::VulkanRenderDevice::FindMemoryType(GTSL::uint32 memoryType, GTSL::uint32 memoryFlags) const
{
	for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if (memoryType & (1 << i)) { return i; }
	}

	//BE_ASSERT(true, "Failed to find a suitable memory type!")
}

#if (_DEBUG)
inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: GTSL::Console::SetTextColor(GTSL::Console::ConsoleTextColor::WHITE); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: GTSL::Console::SetTextColor(GTSL::Console::ConsoleTextColor::WHITE); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:  GTSL::Console::SetTextColor(GTSL::Console::ConsoleTextColor::YELLOW); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:  GTSL::Console::SetTextColor(GTSL::Console::ConsoleTextColor::RED); break;
	default: break;
	}

	GTSL::Console::Print(GTSL::Ranger<const GTSL::UTF8>(GTSL::StringLength(pCallbackData->pMessage), pCallbackData->pMessage));
	GTSL::Console::SetTextColor(GTSL::Console::ConsoleTextColor::WHITE);
	
	return VK_FALSE;
}
#endif // BE_DEBUG

GAL::GPUInfo GAL::VulkanRenderDevice::GetGPUInfo()
{
	GPUInfo result;

	result.GPUName = deviceProperties.deviceName;
	result.DriverVersion = deviceProperties.vendorID;
	result.APIVersion = deviceProperties.apiVersion;
	for(auto e : deviceProperties.pipelineCacheUUID)
	{
		result.PipelineCacheUUID[&e - deviceProperties.pipelineCacheUUID] = e;
	}
	
	return result;
}

GAL::ImageFormat GAL::VulkanRenderDevice::FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const
{
	GTSL::Array<VkFormat, 32> formats;

	for (auto& e : findSupportedImageFormat.Candidates) { formats.EmplaceBack(ImageFormatToVkFormat(e)); }

	return VkFormatToImageFormat(FindSupportedVkFormat(formats, ImageUseToVkFormatFeatureFlagBits(findSupportedImageFormat.ImageUse), ImageTilingToVkImageTiling(findSupportedImageFormat.ImageTiling)));
}

void GAL::VulkanQueue::Submit(const SubmitInfo& submitInfo)
{
	GTSL::Array<VkCommandBuffer, 16> vk_command_buffers(submitInfo.CommandBuffers.ElementCount());
	{
		for(auto* e : submitInfo.CommandBuffers)
		{
			vk_command_buffers[&e - submitInfo.CommandBuffers.begin()] = static_cast<VulkanCommandBuffer*>(e)->GetVkCommandBuffer();
		}
	}
	
	GTSL::Array<VkSemaphore, 16> vk_signal_semaphores(submitInfo.SignalSemaphores.ElementCount());
	{
		for (auto* e : submitInfo.SignalSemaphores)
		{
			vk_signal_semaphores[&e - submitInfo.SignalSemaphores.begin()] = static_cast<VulkanSemaphore*>(e)->GetVkSemaphore();
		}
	}

	GTSL::Array<VkSemaphore, 16> vk_wait_semaphores(submitInfo.WaitSemaphores.ElementCount());
	{
		for (auto* e : submitInfo.WaitSemaphores)
		{
			vk_wait_semaphores[&e - submitInfo.WaitSemaphores.begin()] = static_cast<VulkanSemaphore*>(e)->GetVkSemaphore();
		}
	}

	VkTimelineSemaphoreSubmitInfo vk_timeline_semaphore_submit_info{ VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	vk_timeline_semaphore_submit_info.waitSemaphoreValueCount = vk_wait_semaphores.GetLength();
	vk_timeline_semaphore_submit_info.pWaitSemaphoreValues = submitInfo.WaitValues.begin();
	vk_timeline_semaphore_submit_info.signalSemaphoreValueCount = vk_signal_semaphores.GetLength();
	vk_timeline_semaphore_submit_info.pSignalSemaphoreValues = submitInfo.SignalValues.begin();
	
	VkSubmitInfo vk_submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	vk_submit_info.pNext = &vk_timeline_semaphore_submit_info;
	
	vk_submit_info.commandBufferCount = vk_command_buffers.GetLength();
	vk_submit_info.pCommandBuffers = vk_command_buffers.begin();

	GTSL::Array<GTSL::uint32, 16> vk_pipeline_stages(submitInfo.WaitPipelineStages.ElementCount());
	{
		for(auto e : submitInfo.WaitPipelineStages) { vk_pipeline_stages[&e - submitInfo.WaitPipelineStages.begin()] = PipelineStageToVkPipelineStageFlags(e); }
	}
	vk_submit_info.pWaitDstStageMask = vk_pipeline_stages.begin();
	
	vk_submit_info.signalSemaphoreCount = vk_signal_semaphores.GetLength();
	vk_submit_info.pSignalSemaphores = vk_signal_semaphores.begin();

	vk_submit_info.waitSemaphoreCount = vk_wait_semaphores.GetLength();
	vk_submit_info.pWaitSemaphores = vk_wait_semaphores.begin();
	
	VK_CHECK(vkQueueSubmit(queue, 1, &vk_submit_info, static_cast<VulkanFence*>(submitInfo.Fence)->GetVkFence()));
}

GAL::VulkanRenderDevice::VulkanRenderDevice(const CreateInfo& createInfo)
{
	VkApplicationInfo vk_application_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	vk_application_info.pNext = nullptr;
	vkEnumerateInstanceVersion(&vk_application_info.apiVersion);
	vk_application_info.applicationVersion = VK_MAKE_VERSION(createInfo.ApplicationVersion[0], createInfo.ApplicationVersion[1], createInfo.ApplicationVersion[2]);
	vk_application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	GTSL::StaticString<512> name(createInfo.ApplicationName);
	name += '\0';
	vk_application_info.pApplicationName = name.begin();
	vk_application_info.pEngineName = "Game-Tek | GAL";

	GTSL::Array<const char*, 32> instance_layers{
#if(_DEBUG)
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_LUNARG_standard_validation",
	};
#else
};
#endif

	GTSL::Array<const char*, 32> instance_extensions{
#if(_DEBUG)
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif

		VK_KHR_SURFACE_EXTENSION_NAME,

#if (_WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

#if (_DEBUG)
	VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_EXT{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	vk_debug_utils_messenger_create_info_EXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	vk_debug_utils_messenger_create_info_EXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	vk_debug_utils_messenger_create_info_EXT.pfnUserCallback = debugCallback;
	vk_debug_utils_messenger_create_info_EXT.pUserData = nullptr; // Optional
#endif

	VkInstanceCreateInfo vk_instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	vk_instance_create_info.pNext = &vk_debug_utils_messenger_create_info_EXT;
	vk_instance_create_info.pApplicationInfo = &vk_application_info;
	vk_instance_create_info.enabledLayerCount = instance_layers.GetLength();
	vk_instance_create_info.ppEnabledLayerNames = instance_layers.begin();
	vk_instance_create_info.enabledExtensionCount = instance_extensions.GetLength();
	vk_instance_create_info.ppEnabledExtensionNames = instance_extensions.begin();

	VK_CHECK(vkCreateInstance(&vk_instance_create_info, GetVkAllocationCallbacks(), &instance))

#if (_WIN32)
	createDebugUtilsFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	destroyDebugUtilsFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

	createDebugUtilsFunction(instance, &vk_debug_utils_messenger_create_info_EXT, GetVkAllocationCallbacks(), &debugMessenger);
#endif

	uint32_t phsyical_device_count{ 0 };
	vkEnumeratePhysicalDevices(instance, &phsyical_device_count, nullptr);
	GTSL::Array<VkPhysicalDevice, 8> vk_physical_devices(phsyical_device_count);
	vkEnumeratePhysicalDevices(instance, &phsyical_device_count, vk_physical_devices.begin());

	physicalDevice = vk_physical_devices[0];

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	VkPhysicalDeviceFeatures vk_physical_device_features{};
	vk_physical_device_features.samplerAnisotropy = true;
	vk_physical_device_features.shaderSampledImageArrayDynamicIndexing = true;

	GTSL::Array<const char*, 32> device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	GTSL::Array<VkDeviceQueueCreateInfo, 16> vk_device_queue_create_infos(createInfo.QueueCreateInfos.ElementCount());

	GTSL::uint32 queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, nullptr);
	//Get the amount of queue families there are in the physical device.
	GTSL::Array<VkQueueFamilyProperties, 32> vk_queue_families_properties(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, vk_queue_families_properties.begin());

	GTSL::Array<bool, 32> used_families(queue_families_count);
	for (auto& e : used_families) { e = false; }

	GTSL::Array<VkQueueFlags, 32> vk_queues_flag_bits(queue_families_count);
	{
		for (auto& e : vk_queues_flag_bits)
		{
			e = QueueCapabilitiesToVkQueueFlags(createInfo.QueueCreateInfos[&e - vk_queues_flag_bits.begin()].Capabilities);
		}
	}

	for (GTSL::uint8 QUEUE = 0; QUEUE < vk_device_queue_create_infos.GetLength(); ++QUEUE)
	{
		for (GTSL::uint8 FAMILY = 0; FAMILY < queue_families_count; ++FAMILY)
		{
			if (vk_queue_families_properties[FAMILY].queueCount > 0 && vk_queue_families_properties[FAMILY].queueFlags & vk_queues_flag_bits[FAMILY]) //if family has vk_queue_flags_bits[FAMILY] create queue from this family
			{
				if (used_families[FAMILY]) //if a queue is already being used from this family add another
				{
					vk_device_queue_create_infos[FAMILY].queueCount++;
					vk_device_queue_create_infos[FAMILY].pQueuePriorities = &createInfo.QueueCreateInfos[QUEUE].QueuePriority;
					break;
				}

				vk_device_queue_create_infos[FAMILY].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				vk_device_queue_create_infos[FAMILY].pNext = nullptr;
				vk_device_queue_create_infos[FAMILY].flags = 0;
				vk_device_queue_create_infos[FAMILY].queueFamilyIndex = FAMILY;
				vk_device_queue_create_infos[FAMILY].queueCount = 1;
				vk_device_queue_create_infos[FAMILY].pQueuePriorities = &createInfo.QueueCreateInfos[QUEUE].QueuePriority;
				used_families[FAMILY] = true;
				break;
			}
		}
	}

	VkDeviceCreateInfo vk_device_create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	vk_device_create_info.queueCreateInfoCount = vk_device_queue_create_infos.GetLength();
	vk_device_create_info.pQueueCreateInfos = vk_device_queue_create_infos.begin();
	vk_device_create_info.pEnabledFeatures = &vk_physical_device_features;
	vk_device_create_info.enabledExtensionCount = device_extensions.GetLength();
	vk_device_create_info.ppEnabledExtensionNames = device_extensions.begin();

	VK_CHECK(vkCreateDevice(physicalDevice, &vk_device_create_info, GetVkAllocationCallbacks(), &device));

	for (GTSL::uint8 i = 0; i < createInfo.QueueCreateInfos.ElementCount(); ++i)
	{
		for (GTSL::uint8 j = 0; j < vk_device_queue_create_infos[i].queueCount; ++j)
		{
			static_cast<VulkanQueue*>(createInfo.Queues[i])->familyIndex = vk_device_queue_create_infos[i].queueFamilyIndex;
			static_cast<VulkanQueue*>(createInfo.Queues[i])->queueIndex = j;
			vkGetDeviceQueue(device, vk_device_queue_create_infos[i].queueFamilyIndex, j, &static_cast<VulkanQueue*>(createInfo.Queues[i])->queue);
		}
	}
}

GAL::VulkanRenderDevice::~VulkanRenderDevice()
{
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, GetVkAllocationCallbacks());
#if (_WIN32)
	destroyDebugUtilsFunction(instance, debugMessenger, GetVkAllocationCallbacks());
#endif
	vkDestroyInstance(instance, GetVkAllocationCallbacks());
}

VkFormat GAL::VulkanRenderDevice::FindSupportedVkFormat(GTSL::Ranger<VkFormat> formats, const VkFormatFeatureFlags formatFeatureFlags, const VkImageTiling imageTiling) const
{
	VkFormatProperties format_properties;

	for (auto& e : formats)
	{
		vkGetPhysicalDeviceFormatProperties(physicalDevice, e, &format_properties);

		switch (imageTiling)
		{
			case VK_IMAGE_TILING_LINEAR: if (format_properties.linearTilingFeatures & formatFeatureFlags) return e;
			case VK_IMAGE_TILING_OPTIMAL: if (format_properties.optimalTilingFeatures & formatFeatureFlags) return e;
		}
	}

	return VK_FORMAT_UNDEFINED;
}
