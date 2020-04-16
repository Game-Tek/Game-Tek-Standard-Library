#include "GAL/Vulkan/VulkanRenderDevice.h"

#if (_WIN32)
#include "windows.h"
#include "vulkan/vulkan_win32.h"
#endif

#include "GAL/Vulkan/VulkanCommandBuffer.h"

#include <GTSL/FixedVector.hpp>
#include "GAL/Vulkan/VulkanRenderMesh.h"
#include "GAL/Vulkan/VulkanUniformBuffer.h"
#include "GAL/Vulkan/VulkanTexture.h"
#include "GAL/Vulkan/VulkanRenderPass.h"
#include "GAL/Vulkan/VulkanBindings.h"
#include "GAL/Vulkan/VulkanRenderTarget.h"
#include "GAL/Vulkan/VulkanPipelines.h"
#include "GAL/Vulkan/VulkanRenderContext.h"
#include <vector>

VkFormat VulkanRenderDevice::FindSupportedFormat(const GTSL::FixedVector<VkFormat>& formats, const VkFormatFeatureFlags formatFeatureFlags, const VkImageTiling imageTiling) const
{
	VkFormatProperties format_properties;

	bool isSupported = false;

	for (auto& e : formats)
	{
		vkGetPhysicalDeviceFormatProperties(physicalDevice, e, &format_properties);

		switch (imageTiling)
		{
		case VK_IMAGE_TILING_LINEAR:
			isSupported = format_properties.linearTilingFeatures & formatFeatureFlags;
			break;
		case VK_IMAGE_TILING_OPTIMAL:
			isSupported = format_properties.optimalTilingFeatures & formatFeatureFlags;
			break;
		}

		if (isSupported)
		{
			return e;
		}
	}

	return VK_FORMAT_UNDEFINED;
}

GTSL::uint32 VulkanRenderDevice::FindMemoryType(GTSL::uint32 memoryType, GTSL::uint32 memoryFlags) const
{
	for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if (memoryType & (1 << i)) { return i; }
	}

	//BE_ASSERT(true, "Failed to find a suitable memory type!")
}

void VulkanRenderDevice::AllocateMemory(VkMemoryRequirements* memoryRequirements, GTSL::uint32 memoryPropertyFlag, VkDeviceMemory* deviceMemory) const
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


VulkanRenderDevice::VulkanRenderDevice(const RenderDeviceCreateInfo& renderDeviceCreateInfo) : vulkanQueues(renderDeviceCreateInfo.QueueCreateInfos->GetLength(), GetPersistentAllocationsAllocatorReference())
{
	VkApplicationInfo vk_application_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	vk_application_info.pNext = nullptr;
	vkEnumerateInstanceVersion(&vk_application_info.apiVersion);
	vk_application_info.applicationVersion = VK_MAKE_VERSION(renderDeviceCreateInfo.ApplicationVersion[0], renderDeviceCreateInfo.ApplicationVersion[1], renderDeviceCreateInfo.ApplicationVersion[2]);
	vk_application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	vk_application_info.pApplicationName = renderDeviceCreateInfo.ApplicationName.c_str();
	vk_application_info.pEngineName = "Game-Tek | GAL";
	
	GTSL::Array<const char*, 32, GTSL::uint8> instance_layers = {
#if (_DEBUG)
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
	VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_EXT{};
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
	vk_physical_device_features.samplerAnisotropy = VK_TRUE;
	vk_physical_device_features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;

	GTSL::Array<const char*, 32, GTSL::uint8> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	auto queue_create_infos = renderDeviceCreateInfo.QueueCreateInfos;

	GTSL::Vector<VkDeviceQueueCreateInfo> vk_device_queue_create_infos(queue_create_infos->GetLength(), queue_create_infos->GetLength(), GetTransientAllocationsAllocatorReference());

	GTSL::uint32 queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, nullptr);
	//Get the amount of queue families there are in the physical device.
	GTSL::Vector<VkQueueFamilyProperties> vk_queue_families_properties(queue_families_count, GetTransientAllocationsAllocatorReference());
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_families_count, vk_queue_families_properties.GetData());

	GTSL::Vector<bool> used_families(queue_families_count, GetTransientAllocationsAllocatorReference());
	GTSL::Vector<VkQueueFlagBits> vk_queues_flag_bits(queue_families_count, queue_families_count, GetTransientAllocationsAllocatorReference());
	{
		GTSL::uint8 i = 0;
		for (auto& e : vk_queues_flag_bits)
		{
			e = VkQueueFlagBits(queue_create_infos->At(i).Capabilities);
			++i;
		}
	}

	for (GTSL::uint8 q = 0; q < queue_create_infos->GetLength(); ++q)
	{
		for (GTSL::uint8 f = 0; f < queue_families_count; ++f)
		{
			if (vk_queue_families_properties[f].queueCount > 0 && vk_queue_families_properties[f].queueFlags & vk_queues_flag_bits[f])
			{
				if (used_families[f])
				{
					++vk_device_queue_create_infos[f].queueCount;
					vk_device_queue_create_infos[f].pQueuePriorities = &queue_create_infos->At(q).QueuePriority;
					break;
				}

				vk_device_queue_create_infos[f].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; used_families[f] = true;
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

	for (GTSL::uint8 i = 0; i < renderDeviceCreateInfo.QueueCreateInfos->GetLength(); ++i)
	{
		for (GTSL::uint8 j = 0; j < vk_device_queue_create_infos[i].queueCount; ++j)
		{
			VulkanQueue::VulkanQueueCreateInfo vulkan_queue_create_info;
			vulkan_queue_create_info.FamilyIndex = vk_device_queue_create_infos[i].queueFamilyIndex;
			vulkan_queue_create_info.QueueIndex = j;
			vkGetDeviceQueue(device, vk_device_queue_create_infos[i].queueFamilyIndex, j, &vulkan_queue_create_info.Queue);
			vulkanQueues.EmplaceBack(renderDeviceCreateInfo.QueueCreateInfos->At(i + j), vulkan_queue_create_info);
			*renderDeviceCreateInfo.QueueCreateInfos->At(i).QueueToSet = &vulkanQueues[i + j];
		}
	}
}

VulkanRenderDevice::~VulkanRenderDevice()
{
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, GetVkAllocationCallbacks());
#if (_WIN32)
	destroyDebugUtilsFunction(instance, debugMessenger, GetVkAllocationCallbacks());
#endif
	vkDestroyInstance(instance, GetVkAllocationCallbacks());
}

bool VulkanRenderDevice::IsVulkanSupported()
{
#if (_WIN32)
	return true;
#endif
}

GAL::GPUInfo VulkanRenderDevice::GetGPUInfo()
{
	GAL::GPUInfo result;

	result.GPUName = deviceProperties.deviceName;
	result.DriverVersion = deviceProperties.vendorID;
	result.APIVersion = deviceProperties.apiVersion;

	return result;
}

GAL::RenderMesh* VulkanRenderDevice::CreateRenderMesh(const GAL::RenderMesh::RenderMeshCreateInfo& renderMeshCreateInfo)
{
	void* m{ nullptr };
	GTSL::uint64 a;
	persistentAllocatorReference->Allocate(sizeof(VulkanRenderMesh), alignof(VulkanRenderMesh), &m, &a);
	::new(m) VulkanRenderMesh(this, renderMeshCreateInfo);
	return static_cast<GAL::RenderMesh*>(m);
}

GAL::UniformBuffer* VulkanRenderDevice::CreateUniformBuffer(const GAL::UniformBufferCreateInfo& uniformBufferCreateInfo)
{
	void* m{ nullptr };
	GTSL::uint64 a;
	persistentAllocatorReference->Allocate(sizeof(VulkanUniformBuffer), alignof(VulkanUniformBuffer), &m, &a);
	::new(m) VulkanUniformBuffer(this, uniformBufferCreateInfo);
	return static_cast<GAL::UniformBuffer*>(m);
}

GAL::RenderTarget* VulkanRenderDevice::CreateRenderTarget(const GAL::RenderTarget::RenderTargetCreateInfo& _ICI) { return new VulkanRenderTarget(this, _ICI); }

GAL::Texture* VulkanRenderDevice::CreateTexture(const GAL::TextureCreateInfo& textureCreateInfo) { return new VulkanTexture(this, textureCreateInfo); }

GAL::BindingsPool* VulkanRenderDevice::CreateBindingsPool(const GAL::BindingsPoolCreateInfo& bindingsPoolCreateInfo) { return new VulkanBindingsPool(this, bindingsPoolCreateInfo); }

GAL::BindingsSet* VulkanRenderDevice::CreateBindingsSet(const GAL::BindingsSetCreateInfo& bindingsSetCreateInfo) { return new VulkanBindingsSet(this, bindingsSetCreateInfo); }

GAL::GraphicsPipeline* VulkanRenderDevice::CreateGraphicsPipeline(const GAL::GraphicsPipelineCreateInfo& _GPCI) { return new VulkanGraphicsPipeline(this, _GPCI); }

GAL::RenderPass* VulkanRenderDevice::CreateRenderPass(const GAL::RenderPassCreateInfo& renderPasCreateInfo) {	return new VulkanRenderPass(this, renderPasCreateInfo); }

GAL::ComputePipeline* VulkanRenderDevice::CreateComputePipeline(const GAL::ComputePipelineCreateInfo& computePipelineCreateInfo) { return new VulkanComputePipeline(this, computePipelineCreateInfo); }

GAL::Framebuffer* VulkanRenderDevice::CreateFramebuffer(const GAL::FramebufferCreateInfo& frameBufferCreateInfo) {	return new VulkanFramebuffer(this, frameBufferCreateInfo); }

GAL::RenderContext* VulkanRenderDevice::CreateRenderContext(const GAL::RenderContextCreateInfo& renderContextCreateInfo) { return new VulkanRenderContext(this, renderContextCreateInfo); }

GAL::CommandBuffer* VulkanRenderDevice::CreateCommandBuffer(const GAL::CommandBuffer::CommandBufferCreateInfo& commandBufferCreateInfo) { return new VulkanCommandBuffer(this, commandBufferCreateInfo); }

VulkanRenderDevice::VulkanQueue::VulkanQueue(const QueueCreateInfo& queueCreateInfo, const VulkanQueueCreateInfo& vulkanQueueCreateInfo) : queue(vulkanQueueCreateInfo.Queue), queueIndex(vulkanQueueCreateInfo.QueueIndex), familyIndex(vulkanQueueCreateInfo.FamilyIndex)
{
}

void VulkanRenderDevice::VulkanQueue::Dispatch(const DispatchInfo& dispatchInfo)
{
	VkSubmitInfo vk_submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	vk_submit_info.commandBufferCount = 1;
	auto vk_command_buffer = static_cast<VulkanCommandBuffer*>(dispatchInfo.CommandBuffer)->GetVkCommandBuffer();
	vk_submit_info.pCommandBuffers = &vk_command_buffer;
	GTSL::uint32 vk_pipeline_stage = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	vk_submit_info.pWaitDstStageMask = &vk_pipeline_stage;
	vkQueueSubmit(queue, 1, &vk_submit_info, nullptr);
}
