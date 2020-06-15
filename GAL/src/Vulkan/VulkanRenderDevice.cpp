#include "GAL/Vulkan/VulkanRenderDevice.h"

#if (_WIN32)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#include "GAL/Vulkan/VulkanCommandBuffer.h"
#include "GAL/Vulkan/VulkanTexture.h"
#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanRenderContext.h"
#include "GTSL/StaticString.hpp"

GTSL::uint32 GAL::VulkanRenderDevice::FindMemoryType(GTSL::uint32 memoryType, GTSL::uint32 memoryFlags) const
{
	for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if (memoryType & (1 << i)) { return i; }
	}

	//BE_ASSERT(true, "Failed to find a suitable memory type!")
}

void GAL::VulkanRenderDevice::AllocateMemory(VkMemoryRequirements* memoryRequirements, GTSL::uint32 memoryPropertyFlag, VkDeviceMemory* deviceMemory) const
{
	VkMemoryAllocateInfo vk_memory_allocate_info{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	vk_memory_allocate_info.allocationSize = memoryRequirements->size;
	vk_memory_allocate_info.memoryTypeIndex = FindMemoryType(memoryRequirements->memoryTypeBits, memoryPropertyFlag);

	VK_CHECK(vkAllocateMemory(device, &vk_memory_allocate_info, GetVkAllocationCallbacks(), deviceMemory));
}

#if (_DEBUG)
inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (messageSeverity)
	{
	//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: BE_BASIC_LOG_MESSAGE("Vulkan: %s", pCallbackData->pMessage) break;
	//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: BE_BASIC_LOG_MESSAGE("Vulkan: %s", pCallbackData->pMessage) break;
	//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: BE_BASIC_LOG_WARNING("Vulkan: %s", pCallbackData->pMessage) break;
	//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: BE_BASIC_LOG_ERROR("Vulkan: %s, %s", pCallbackData->pObjects->pObjectName, pCallbackData->pMessage) break;
	default: break;
	}

	return VK_FALSE;
}
#endif // BE_DEBUG

GAL::VulkanRenderDevice::~VulkanRenderDevice()
{
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, GetVkAllocationCallbacks());
#if (_WIN32)
	destroyDebugUtilsFunction(instance, debugMessenger, GetVkAllocationCallbacks());
#endif
	vkDestroyInstance(instance, GetVkAllocationCallbacks());
}

bool GAL::VulkanRenderDevice::IsVulkanSupported()
{
#if (_WIN32)
	return true;
#endif
}

GAL::GPUInfo GAL::VulkanRenderDevice::GetGPUInfo()
{
	GAL::GPUInfo result;

	result.GPUName = deviceProperties.deviceName;
	result.DriverVersion = deviceProperties.vendorID;
	result.APIVersion = deviceProperties.apiVersion;

	return result;
}

GAL::ImageFormat GAL::VulkanRenderDevice::FindNearestSupportedImageFormat(GTSL::Ranger<ImageFormat> candidates, const ImageUse imageUse, const ImageTiling imageTiling) const
{
	GTSL::Array<VkFormat, 32> formats;

	for (auto& e : candidates) { formats.EmplaceBack(ImageFormatToVkFormat(e)); }

	return VkFormatToImageFormat(FindSupportedVkFormat(formats, ImageUseToVkFormatFeatureFlagBits(imageUse), ImageTilingToVkImageTiling(imageTiling)));
}

GAL::VulkanRenderDevice::VulkanQueue::VulkanQueue(const CreateInfo& queueCreateInfo)
{
}

void GAL::VulkanRenderDevice::VulkanQueue::Dispatch(const DispatchInfo& dispatchInfo)
{
	VkSubmitInfo vk_submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	vk_submit_info.commandBufferCount = 1;
	auto vk_command_buffer = static_cast<GAL::VulkanCommandBuffer*>(dispatchInfo.CommandBuffer)->GetVkCommandBuffer();
	vk_submit_info.pCommandBuffers = &vk_command_buffer;
	GTSL::uint32 vk_pipeline_stage = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	vk_submit_info.pWaitDstStageMask = &vk_pipeline_stage;
	vkQueueSubmit(queue, 1, &vk_submit_info, nullptr);
}

GAL::VulkanRenderDevice::VulkanRenderDevice(const RenderDeviceCreateInfo& renderDeviceCreateInfo)
{
	VkApplicationInfo vk_application_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	vk_application_info.pNext = nullptr;
	vkEnumerateInstanceVersion(&vk_application_info.apiVersion);
	vk_application_info.applicationVersion = VK_MAKE_VERSION(renderDeviceCreateInfo.ApplicationVersion[0], renderDeviceCreateInfo.ApplicationVersion[1], renderDeviceCreateInfo.ApplicationVersion[2]);
	vk_application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	GTSL::StaticString<512> name(renderDeviceCreateInfo.ApplicationName);
	name += '\0';
	vk_application_info.pApplicationName = name.begin();
	vk_application_info.pEngineName = "Game-Tek | GAL";

	GTSL::Array<const char*, 32, GTSL::uint8> instance_layers = {
#if(_DEBUG)
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_LUNARG_parameter_validation",
	};
#else
};
#endif

	GTSL::Array<const char*, 32, GTSL::uint8> extensions = {
#if(_DEBUG)
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif

		VK_KHR_SURFACE_EXTENSION_NAME,

#if (_WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

	VkInstanceCreateInfo vk_instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	vk_instance_create_info.pApplicationInfo = &vk_application_info;
	vk_instance_create_info.enabledLayerCount = instance_layers.GetLength();
	vk_instance_create_info.ppEnabledLayerNames = instance_layers.GetData();
	vk_instance_create_info.enabledExtensionCount = extensions.GetLength();
	vk_instance_create_info.ppEnabledExtensionNames = extensions.GetData();

	VK_CHECK(vkCreateInstance(&vk_instance_create_info, GetVkAllocationCallbacks(), &instance))

#if (_DEBUG)
		VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_EXT {};
	vk_debug_utils_messenger_create_info_EXT.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	vk_debug_utils_messenger_create_info_EXT.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	vk_debug_utils_messenger_create_info_EXT.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	vk_debug_utils_messenger_create_info_EXT.pfnUserCallback = debugCallback;
	vk_debug_utils_messenger_create_info_EXT.pUserData = nullptr; // Optional
#endif

#if (_WIN32)
	createDebugUtilsFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	destroyDebugUtilsFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

	createDebugUtilsFunction(instance, &vk_debug_utils_messenger_create_info_EXT, GetVkAllocationCallbacks(), &debugMessenger);
#endif

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	VkPhysicalDeviceFeatures vk_physical_device_features{};
	vk_physical_device_features.samplerAnisotropy = true;
	vk_physical_device_features.shaderSampledImageArrayDynamicIndexing = true;

	GTSL::Array<const char*, 32, GTSL::uint8> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	auto& queue_create_infos = renderDeviceCreateInfo.QueueCreateInfos;

	GTSL::Array<VkDeviceQueueCreateInfo, 16> vk_device_queue_create_infos(queue_create_infos.ElementCount());

	GTSL::uint32 queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, nullptr);
	//Get the amount of queue families there are in the physical device.
	GTSL::Array<VkQueueFamilyProperties, 32> vk_queue_families_properties(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, vk_queue_families_properties.GetData());

	GTSL::Array<bool, 32> used_families(queue_families_count);
	GTSL::Array<VkQueueFlagBits, 32> vk_queues_flag_bits(queue_families_count);
	{
		GTSL::uint8 i = 0;
		for (auto& e : vk_queues_flag_bits)
		{
			//e = VkQueueFlagBits(vk_queue_families_properties[i].Capabilities);
			++i;
		}
	}

	for (GTSL::uint8 q = 0; q < queue_create_infos.ElementCount(); ++q)
	{
		for (GTSL::uint8 f = 0; f < queue_families_count; ++f)
		{
			if (vk_queue_families_properties[f].queueCount > 0 && vk_queue_families_properties[f].queueFlags & vk_queues_flag_bits[f])
			{
				if (used_families[f])
				{
					++vk_device_queue_create_infos[f].queueCount;
					vk_device_queue_create_infos[f].pQueuePriorities = &queue_create_infos[q].QueuePriority;
					break;
				}

				used_families[f] = true;
				vk_device_queue_create_infos[f].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				vk_device_queue_create_infos[f].pNext = nullptr;
				vk_device_queue_create_infos[f].flags = 0;
				vk_device_queue_create_infos[f].queueFamilyIndex = f;
				vk_device_queue_create_infos[f].queueCount = 1;
				break;
			}
		}
	}

	VkDeviceCreateInfo vk_device_create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	vk_device_create_info.queueCreateInfoCount = vk_device_queue_create_infos.GetLength();
	vk_device_create_info.pQueueCreateInfos = vk_device_queue_create_infos.GetData();
	vk_device_create_info.pEnabledFeatures = &vk_physical_device_features;
	vk_device_create_info.enabledExtensionCount = device_extensions.GetLength();
	vk_device_create_info.ppEnabledExtensionNames = device_extensions.GetData();

	VK_CHECK(vkCreateDevice(physicalDevice, &vk_device_create_info, GetVkAllocationCallbacks(), &device));

	for (GTSL::uint8 i = 0; i < renderDeviceCreateInfo.QueueCreateInfos.ElementCount(); ++i)
	{
		for (GTSL::uint8 j = 0; j < vk_device_queue_create_infos[i].queueCount; ++j)
		{
			static_cast<VulkanQueue*>(renderDeviceCreateInfo.Queues + i)->familyIndex = vk_device_queue_create_infos[i].queueFamilyIndex;
			static_cast<VulkanQueue*>(renderDeviceCreateInfo.Queues + i)->queueIndex = j;
			vkGetDeviceQueue(device, vk_device_queue_create_infos[i].queueFamilyIndex, j, &static_cast<VulkanQueue*>(renderDeviceCreateInfo.Queues + i)->queue);
		}
	}
}

VkFormat GAL::VulkanRenderDevice::FindSupportedVkFormat(GTSL::Ranger<VkFormat> formats, const VkFormatFeatureFlags formatFeatureFlags, const VkImageTiling imageTiling) const
{
	VkFormatProperties format_properties;

	for (auto& e : formats)
	{
		vkGetPhysicalDeviceFormatProperties(physicalDevice, e, &format_properties);

		switch (imageTiling)
		{
		case VK_IMAGE_TILING_LINEAR:
			if (format_properties.linearTilingFeatures & formatFeatureFlags) return e;
			
		case VK_IMAGE_TILING_OPTIMAL:
			if (format_properties.optimalTilingFeatures & formatFeatureFlags) return e;
		}
	}

	return VK_FORMAT_UNDEFINED;
}
