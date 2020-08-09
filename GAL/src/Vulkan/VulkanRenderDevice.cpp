	#include "GAL/Vulkan/VulkanRenderDevice.h"

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GAL/ext/vulkan/vulkan_win32.h>
#endif

#include "GAL/Vulkan/VulkanAccelerationStructures.h"
#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanBuffer.h"
#include "GAL/Vulkan/VulkanCommandBuffer.h"
#include "GAL/Vulkan/VulkanImage.h"
#include "GAL/Vulkan/VulkanSynchronization.h"
#include "GTSL/Console.h"
#include "GTSL/StaticString.hpp"

void GAL::VulkanRenderDevice::GetBufferMemoryRequirements(const VulkanBuffer* buffer, MemoryRequirements& bufferMemoryRequirements) const
{
	VkMemoryRequirements vk_memory_requirements;
	vkGetBufferMemoryRequirements(device, static_cast<const VulkanBuffer*>(buffer)->GetVkBuffer(), &vk_memory_requirements);
	bufferMemoryRequirements.Alignment = vk_memory_requirements.alignment;
	bufferMemoryRequirements.MemoryTypes = vk_memory_requirements.memoryTypeBits;
	bufferMemoryRequirements.Size = vk_memory_requirements.size;
}

void GAL::VulkanRenderDevice::GetImageMemoryRequirements(const VulkanImage* image,	MemoryRequirements& imageMemoryRequirements) const
{
	VkMemoryRequirements vk_memory_requirements;
	vkGetImageMemoryRequirements(device, image->GetVkImage(), &vk_memory_requirements);
	imageMemoryRequirements.Alignment = vk_memory_requirements.alignment;
	imageMemoryRequirements.MemoryTypes = vk_memory_requirements.memoryTypeBits;
	imageMemoryRequirements.Size = vk_memory_requirements.size;
}

void GAL::VulkanRenderDevice::GetAccelerationStructureMemoryRequirements(const GetAccelerationStructureMemoryRequirementsInfo& accelerationStructureMemoryRequirementsInfo) const
{
	VkAccelerationStructureMemoryRequirementsInfoKHR acceleration_structure_memory_requirements_info{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR };
	acceleration_structure_memory_requirements_info.accelerationStructure = accelerationStructureMemoryRequirementsInfo.AccelerationStructure->GetVkAccelerationStructure();
	acceleration_structure_memory_requirements_info.buildType = static_cast<VkAccelerationStructureBuildTypeKHR>(accelerationStructureMemoryRequirementsInfo.AccelerationStructureBuildType);
	acceleration_structure_memory_requirements_info.type = static_cast<VkAccelerationStructureMemoryRequirementsTypeKHR>(accelerationStructureMemoryRequirementsInfo.AccelerationStructureMemoryRequirementsType);
	VkMemoryRequirements2 memory_requirements2;
	vkGetAccelerationStructureMemoryRequirementsKHR(device, &acceleration_structure_memory_requirements_info, &memory_requirements2);
	accelerationStructureMemoryRequirementsInfo.MemoryRequirements->Size = memory_requirements2.memoryRequirements.size;
	accelerationStructureMemoryRequirementsInfo.MemoryRequirements->Alignment = memory_requirements2.memoryRequirements.alignment;
	accelerationStructureMemoryRequirementsInfo.MemoryRequirements->MemoryTypes = memory_requirements2.memoryRequirements.memoryTypeBits;
}

GTSL::uint32 GAL::VulkanRenderDevice::FindMemoryType(const GTSL::uint32 typeFilter, const GTSL::uint32 memoryType) const
{
	for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryType) == memoryType) { return i; }
	}

	return 0xffffffff;
}

#undef ERROR

#if (_DEBUG)
inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	auto* device = static_cast<GAL::VulkanRenderDevice*>(pUserData);
	
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	{
		device->GetDebugPrintFunction()(pCallbackData->pMessage, GAL::RenderDevice::MessageSeverity::MESSAGE);
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	{
		device->GetDebugPrintFunction()(pCallbackData->pMessage, GAL::RenderDevice::MessageSeverity::MESSAGE);
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
	{
		device->GetDebugPrintFunction()(pCallbackData->pMessage, GAL::RenderDevice::MessageSeverity::WARNING);
		break;
	}
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
	{
		device->GetDebugPrintFunction()(pCallbackData->pMessage, GAL::RenderDevice::MessageSeverity::ERROR);
		break;
	}
	default: break;
	}

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

GTSL::uint32 GAL::VulkanRenderDevice::FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const
{
	VkFormatProperties format_properties;

	VkFormatFeatureFlags features;
	
	switch (findSupportedImageFormat.ImageUse)
	{
	case VulkanImageUse::TRANSFER_SOURCE: features = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT; break;
	case VulkanImageUse::TRANSFER_DESTINATION: features = VK_FORMAT_FEATURE_TRANSFER_DST_BIT; break;
	case VulkanImageUse::SAMPLE: features = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT; break;
	case VulkanImageUse::STORAGE: features = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT; break;
	case VulkanImageUse::COLOR_ATTACHMENT: features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT; break;
	case VulkanImageUse::DEPTH_STENCIL_ATTACHMENT: features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT; break;
	case VulkanImageUse::TRANSIENT_ATTACHMENT: [[fallthrough]];
	case VulkanImageUse::INPUT_ATTACHMENT: [[fallthrough]];
	default: features = 0; break;
	}
	
	for (auto e : findSupportedImageFormat.Candidates)
	{
		vkGetPhysicalDeviceFormatProperties(physicalDevice, static_cast<VkFormat>(e), &format_properties);

		switch (findSupportedImageFormat.ImageTiling)
		{
		case VK_IMAGE_TILING_LINEAR: if (format_properties.linearTilingFeatures & features) return e;
		case VK_IMAGE_TILING_OPTIMAL: if (format_properties.optimalTilingFeatures & features) return e;
		default: __debugbreak();
		}
	}

	return VK_FORMAT_UNDEFINED;
}

void GAL::VulkanQueue::Wait() const { vkQueueWaitIdle(queue); }

void GAL::VulkanQueue::Submit(const SubmitInfo& submitInfo)
{
	auto vulkan_command_buffers = GTSL::Ranger<const VulkanCommandBuffer>(submitInfo.CommandBuffers);
	
	GTSL::Array<VkCommandBuffer, 16> vk_command_buffers(static_cast<GTSL::uint32>(vulkan_command_buffers.ElementCount()));
	for(const auto& e : vulkan_command_buffers)
	{
		vk_command_buffers[RangeForIndex(e, vulkan_command_buffers)] = e.GetVkCommandBuffer();
	}
	
	GTSL::Ranger<const VulkanSemaphore> vulkan_signal_semaphores(submitInfo.SignalSemaphores);
	GTSL::Ranger<const VulkanSemaphore> vulkan_wait_semaphores(submitInfo.WaitSemaphores);

	//VkTimelineSemaphoreSubmitInfo vk_timeline_semaphore_submit_info{ VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	//vk_timeline_semaphore_submit_info.waitSemaphoreValueCount = vk_wait_semaphores.GetLength();
	//vk_timeline_semaphore_submit_info.pWaitSemaphoreValues = submitInfo.WaitValues.begin();
	//vk_timeline_semaphore_submit_info.signalSemaphoreValueCount = vk_signal_semaphores.GetLength();
	//vk_timeline_semaphore_submit_info.pSignalSemaphoreValues = submitInfo.SignalValues.begin();
	
	VkSubmitInfo vk_submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	//vk_submit_info.pNext = &vk_timeline_semaphore_submit_info;
	
	vk_submit_info.commandBufferCount = vk_command_buffers.GetLength();
	vk_submit_info.pCommandBuffers = vk_command_buffers.begin();

	GTSL::Array<GTSL::uint32, 16> vk_pipeline_stages(submitInfo.WaitPipelineStages.ElementCount());
	{
		for(const auto& e : submitInfo.WaitPipelineStages)
		{
			vk_pipeline_stages[RangeForIndex(e, submitInfo.WaitPipelineStages)] = PipelineStageToVkPipelineStageFlags(e);
		}
	}
	vk_submit_info.pWaitDstStageMask = vk_pipeline_stages.begin();
	
	vk_submit_info.signalSemaphoreCount = vulkan_signal_semaphores.ElementCount();
	vk_submit_info.pSignalSemaphores = reinterpret_cast<const VkSemaphore*>(vulkan_signal_semaphores.begin());

	vk_submit_info.waitSemaphoreCount = vulkan_wait_semaphores.ElementCount();
	vk_submit_info.pWaitSemaphores = reinterpret_cast<const VkSemaphore*>(vulkan_wait_semaphores.begin());
	
	VK_CHECK(vkQueueSubmit(queue, 1, &vk_submit_info, submitInfo.Fence ? static_cast<const VulkanFence*>(submitInfo.Fence)->GetVkFence() : nullptr));
}

#define GET_DEVICE_PROC(device, var) var = reinterpret_cast<PFN_##var>(vkGetDeviceProcAddr(device, #var))

GAL::VulkanRenderDevice::VulkanRenderDevice(const CreateInfo& createInfo) : RenderDevice(createInfo.DebugPrintFunction)
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
	vk_debug_utils_messenger_create_info_EXT.pUserData = this;
#endif

	VkInstanceCreateInfo vk_instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	vk_instance_create_info.pNext = &vk_debug_utils_messenger_create_info_EXT;
	vk_instance_create_info.pApplicationInfo = &vk_application_info;
	vk_instance_create_info.enabledLayerCount = instance_layers.GetLength();
	vk_instance_create_info.ppEnabledLayerNames = instance_layers.begin();
	vk_instance_create_info.enabledExtensionCount = instance_extensions.GetLength();
	vk_instance_create_info.ppEnabledExtensionNames = instance_extensions.begin();

	VK_CHECK(vkCreateInstance(&vk_instance_create_info, GetVkAllocationCallbacks(), &instance))

#if (_DEBUG)
	createDebugUtilsFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	destroyDebugUtilsFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

	createDebugUtilsFunction(instance, &vk_debug_utils_messenger_create_info_EXT, GetVkAllocationCallbacks(), &debugMessenger);
#endif

	uint32_t phsyical_device_count{ 0 };
	vkEnumeratePhysicalDevices(instance, &phsyical_device_count, nullptr);
	GTSL::Array<VkPhysicalDevice, 8> vk_physical_devices(phsyical_device_count);
	vkEnumeratePhysicalDevices(instance, &phsyical_device_count, vk_physical_devices.begin());

	physicalDevice = vk_physical_devices[0];

	VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES };
	timeline_semaphore_features.timelineSemaphore = true;

	GTSL::Array<VkDeviceQueueCreateInfo, 16> vk_device_queue_create_infos;

	GTSL::uint32 queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, nullptr);
	//Get the amount of queue families there are in the physical device.
	GTSL::Array<VkQueueFamilyProperties, 32> vk_queue_families_properties(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, vk_queue_families_properties.begin());

	GTSL::Array<bool, 32> used_families;

	GTSL::Array<VkQueueFlags, 32> vk_queues_flag_bits(createInfo.QueueCreateInfos.ElementCount());
	{
		for (auto& e : vk_queues_flag_bits)
		{
			e = createInfo.QueueCreateInfos[&e - vk_queues_flag_bits.begin()].Capabilities;
		}
	}

	GTSL::Array<GTSL::Array<GTSL::float32, 64>, 64> families_priorities;
	GTSL::Array<GTSL::Array<GTSL::uint32, 64>, 64> families_indeces;

	for (GTSL::uint8 QUEUE = 0; QUEUE < createInfo.QueueCreateInfos.ElementCount(); ++QUEUE)
	{
		for (GTSL::uint8 FAMILY = 0; FAMILY < queue_families_count; ++FAMILY)
		{
			if (vk_queue_families_properties[FAMILY].queueCount > 0 && vk_queue_families_properties[FAMILY].queueFlags & vk_queues_flag_bits[QUEUE]) //if family has vk_queue_flags_bits[FAMILY] create queue from this family
			{
				if (used_families[FAMILY]) //if a queue is already being used from this family add another
				{
					++vk_device_queue_create_infos[FAMILY].queueCount;
					families_priorities[FAMILY].EmplaceBack(createInfo.QueueCreateInfos[QUEUE].QueuePriority);
					families_indeces[FAMILY].EmplaceBack(QUEUE);
					vk_device_queue_create_infos[FAMILY].pQueuePriorities = &families_priorities[FAMILY][vk_device_queue_create_infos[FAMILY].queueCount - 1];
					break;
				}

				vk_device_queue_create_infos.EmplaceBack();
				families_priorities.EmplaceBack();
				families_indeces.EmplaceBack();
				used_families.EmplaceBack(true);
				
				families_priorities[FAMILY].EmplaceBack(createInfo.QueueCreateInfos[QUEUE].QueuePriority);
				families_indeces[FAMILY].EmplaceBack(QUEUE);
				
				vk_device_queue_create_infos[FAMILY].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				vk_device_queue_create_infos[FAMILY].pNext = nullptr;
				vk_device_queue_create_infos[FAMILY].flags = 0;
				vk_device_queue_create_infos[FAMILY].queueFamilyIndex = FAMILY;
				vk_device_queue_create_infos[FAMILY].queueCount = 1;
				vk_device_queue_create_infos[FAMILY].pQueuePriorities = &families_priorities[FAMILY][vk_device_queue_create_infos[FAMILY].queueCount - 1];
				break;
			}
		}
	}

	VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	VkPhysicalDeviceFeatures2 features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

	features2.features.samplerAnisotropy = true;

	void** next_property = &properties2.pNext;
	void** next_feature = &features2.pNext;

	GTSL::Array<GTSL::byte, 32 * 1024> properties_structures;
	GTSL::Array<GTSL::byte, 32 * 1024> features_structures;
	GTSL::Array<const char*, 32> device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	auto place_properties_structure = [&](const GTSL::uint64 size, void* structure, void** structureNext)
	{
		auto& structure_array = properties_structures;

		structure_array.Resize(structure_array.GetLength() + static_cast<GTSL::uint32>(size));
		GTSL::MemCopy(size, structure, structure_array.end() - size);
		*next_feature = structure_array.end() - size;
		next_feature = structureNext;
	};

	auto place_features_structure = [&](const GTSL::uint64 size, void* structure, void** structureNext)
	{
		auto& structure_array = features_structures;
		
		structure_array.Resize(structure_array.GetLength() + static_cast<GTSL::uint32>(size));
		GTSL::MemCopy(size, structure, structure_array.end() - size);
		*next_feature = structure_array.end() - size;
		next_feature = structureNext;
	};

	place_features_structure(sizeof(VkPhysicalDeviceTimelineSemaphoreFeatures), &timeline_semaphore_features, &timeline_semaphore_features.pNext);
	device_extensions.EmplaceBack(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);

	for (auto e : createInfo.Extensions)
	{
		switch (e)
		{
		case Extension::RAY_TRACING:
		{
			device_extensions.EmplaceBack(VK_KHR_RAY_TRACING_EXTENSION_NAME);
			device_extensions.EmplaceBack(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
			device_extensions.EmplaceBack(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
			device_extensions.EmplaceBack(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
			device_extensions.EmplaceBack(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);

			VkPhysicalDeviceRayTracingPropertiesKHR ray_tracing_properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR };
			VkPhysicalDeviceRayTracingFeaturesKHR ray_tracing_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_FEATURES_KHR };

			place_properties_structure(sizeof(VkPhysicalDeviceRayTracingPropertiesKHR), &ray_tracing_properties, &ray_tracing_properties.pNext);
			place_features_structure(sizeof(VkPhysicalDeviceRayTracingFeaturesKHR), &ray_tracing_features, &ray_tracing_features.pNext);
			
			break;
		}
		default:;
		}
	}

	vkGetPhysicalDeviceProperties2(physicalDevice, &properties2);
	vkGetPhysicalDeviceFeatures2(physicalDevice, &features2);

	deviceProperties = properties2.properties;

	VkDeviceCreateInfo vk_device_create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	vk_device_create_info.pNext = &features2; //extended features
	vk_device_create_info.queueCreateInfoCount = vk_device_queue_create_infos.GetLength();
	vk_device_create_info.pQueueCreateInfos = vk_device_queue_create_infos.begin();
	vk_device_create_info.pEnabledFeatures = nullptr;
	vk_device_create_info.enabledExtensionCount = device_extensions.GetLength();
	vk_device_create_info.ppEnabledExtensionNames = device_extensions.begin();

	VK_CHECK(vkCreateDevice(physicalDevice, &vk_device_create_info, GetVkAllocationCallbacks(), &device));

	for (GTSL::uint8 FAMILY = 0; FAMILY < families_indeces.GetLength(); ++FAMILY)
	{
		for (GTSL::uint8 QUEUE = 0; QUEUE < families_indeces[FAMILY].GetLength(); ++QUEUE)
		{
			createInfo.Queues[families_indeces[FAMILY][QUEUE]].familyIndex = FAMILY;
			createInfo.Queues[families_indeces[FAMILY][QUEUE]].queueIndex = QUEUE;
			vkGetDeviceQueue(device, FAMILY, QUEUE, &createInfo.Queues[families_indeces[FAMILY][QUEUE]].queue);
		}
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (auto e : createInfo.Extensions)
	{
		switch (e)
		{
		case Extension::RAY_TRACING:
		{		
			GET_DEVICE_PROC(device, vkCreateAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkDestroyAccelerationStructureKHR);
				
			GET_DEVICE_PROC(device, vkCreateRayTracingPipelinesKHR);
			GET_DEVICE_PROC(device, vkBindAccelerationStructureMemoryKHR);
				
			GET_DEVICE_PROC(device, vkGetAccelerationStructureMemoryRequirementsKHR);
			GET_DEVICE_PROC(device, vkGetAccelerationStructureDeviceAddressKHR);
				
			GET_DEVICE_PROC(device, vkCreateDeferredOperationKHR);
			GET_DEVICE_PROC(device, vkDestroyDeferredOperationKHR);
				
			GET_DEVICE_PROC(device, vkCmdCopyAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkCmdBuildAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkCmdWriteAccelerationStructuresPropertiesKHR);
			GET_DEVICE_PROC(device, vkCmdCopyMemoryToAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkCmdCopyAccelerationStructureToMemoryKHR);
			GET_DEVICE_PROC(device, vkCmdTraceRaysKHR);
				
			break;
		}
		default:;
		}
	}

#if (_DEBUG)
	GET_DEVICE_PROC(device, vkSetDebugUtilsObjectNameEXT);
#endif
}

GAL::VulkanRenderDevice::~VulkanRenderDevice()
{
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, GetVkAllocationCallbacks());
#if (_DEBUG)
	destroyDebugUtilsFunction(instance, debugMessenger, GetVkAllocationCallbacks());
#endif
	vkDestroyInstance(instance, GetVkAllocationCallbacks());
}