#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanAccelerationStructures.h"
#include "GAL/Vulkan/VulkanSynchronization.h"
#include "GAL/Vulkan/VulkanCommandBuffer.h"
#include "GAL/Vulkan/VulkanBindings.h"
#include "GTSL/StaticString.hpp"

using namespace GTSL;

void* vkAllocate(void* data, uint64 size, uint64 alignment, VkSystemAllocationScope)
{
	auto* allocation_info = static_cast<GAL::VulkanRenderDevice::AllocationInfo*>(data);
	return allocation_info->Allocate(allocation_info->UserData, size, alignment);
}

void* vkReallocate(void* data, void* originalAlloc, uint64 size, uint64 alignment, VkSystemAllocationScope)
{
	auto* allocation_info = static_cast<GAL::VulkanRenderDevice::AllocationInfo*>(data);

	if(originalAlloc && size)
	{
		return allocation_info->Reallocate(allocation_info->UserData, originalAlloc, size, alignment);
	}

	if(!originalAlloc && size)
	{
		return allocation_info->Allocate(allocation_info->UserData, size, alignment);
	}

	allocation_info->Deallocate(allocation_info->UserData, originalAlloc);
	return nullptr;
}

void vkFree(void* data, void* alloc)
{
	if (alloc)
	{
		auto* allocation_info = static_cast<GAL::VulkanRenderDevice::AllocationInfo*>(data);
		allocation_info->Deallocate(allocation_info->UserData, alloc);
	}
}

uint32 GAL::VulkanRenderDevice::FindMemoryType(const uint32 typeFilter, const uint32 memoryType) const
{
	for (uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryType) == memoryType) { return i; }
	}

	return 0xffffffff;
}

GTSL::Array<GAL::VulkanRenderDevice::MemoryHeap, 16> GAL::VulkanRenderDevice::GetMemoryHeaps() const
{
	GTSL::Array<MemoryHeap, 16> memoryHeaps;

	for(uint8 i = 0; i < memoryProperties.memoryHeapCount; ++i)
	{
		MemoryHeap memoryHeap;
		memoryHeap.Size = GTSL::Byte(memoryProperties.memoryHeaps[i].size);
		memoryHeap.MemoryTypes = 0;
		
		TranslateMask<VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, (uint32)MemoryType::GPU>(memoryProperties.memoryHeaps[i].flags, memoryHeap.MemoryTypes);

		memoryHeaps.EmplaceBack(memoryHeap);
	}

	return memoryHeaps;
}

GTSL::Array<GAL::MemoryTypes, 16> GAL::VulkanRenderDevice::GetMemoryTypes() const
{
	GTSL::Array<GAL::MemoryTypes, 16> memoryTypes;

	for(uint8 i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		GAL::MemoryTypes memoryType = 0;

		TranslateMask<VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,	(uint32)MemoryType::GPU>			(memoryProperties.memoryTypes[i].propertyFlags, memoryType);
		TranslateMask<VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,	(uint32)MemoryType::HOST_VISIBLE>	(memoryProperties.memoryTypes[i].propertyFlags, memoryType);
		TranslateMask<VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,	(uint32)MemoryType::HOST_COHERENT>	(memoryProperties.memoryTypes[i].propertyFlags, memoryType);
		TranslateMask<VK_MEMORY_PROPERTY_HOST_CACHED_BIT,	(uint32)MemoryType::HOST_CACHED>	(memoryProperties.memoryTypes[i].propertyFlags,	memoryType);

		memoryTypes.EmplaceBack(memoryType);
	}

	return memoryTypes;
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
		//device->GetDebugPrintFunction()(pCallbackData->pMessage, GAL::RenderDevice::MessageSeverity::MESSAGE);
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
	GPUInfo result; VkPhysicalDeviceProperties physicalDeviceProperties;

	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	
	result.GPUName = physicalDeviceProperties.deviceName;
	result.DriverVersion = physicalDeviceProperties.vendorID;
	result.APIVersion = physicalDeviceProperties.apiVersion;
	for(auto e : physicalDeviceProperties.pipelineCacheUUID)
	{
		result.PipelineCacheUUID[&e - physicalDeviceProperties.pipelineCacheUUID] = e;
	}
	
	return result;
}

GAL::VulkanTextureFormat GAL::VulkanRenderDevice::FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const
{
	VkFormatProperties format_properties;

	VkFormatFeatureFlags features{};

	TranslateMask<(uint32)VulkanTextureUse::TRANSFER_SOURCE,			VK_FORMAT_FEATURE_TRANSFER_SRC_BIT>					(findSupportedImageFormat.TextureUses, features);
	TranslateMask<(uint32)VulkanTextureUse::TRANSFER_DESTINATION,		VK_FORMAT_FEATURE_TRANSFER_DST_BIT>					(findSupportedImageFormat.TextureUses, features);
	TranslateMask<(uint32)VulkanTextureUse::SAMPLE,						VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT>				(findSupportedImageFormat.TextureUses, features);
	TranslateMask<(uint32)VulkanTextureUse::STORAGE,					VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT>				(findSupportedImageFormat.TextureUses, features);
	TranslateMask<(uint32)VulkanTextureUse::COLOR_ATTACHMENT,			VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT>				(findSupportedImageFormat.TextureUses, features);
	TranslateMask<(uint32)VulkanTextureUse::DEPTH_STENCIL_ATTACHMENT,	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT>		(findSupportedImageFormat.TextureUses, features);
	
	for (auto e : findSupportedImageFormat.Candidates)
	{
		vkGetPhysicalDeviceFormatProperties(physicalDevice, static_cast<VkFormat>(e), &format_properties);

		switch (static_cast<VkImageTiling>(findSupportedImageFormat.TextureTiling))
		{
		case VK_IMAGE_TILING_LINEAR:
		{
			if (format_properties.linearTilingFeatures & features) { return e; }

			break;
		}
		case VK_IMAGE_TILING_OPTIMAL:
		{
			if (format_properties.optimalTilingFeatures & features) { return e; }

			break;
		}
			
		default: __debugbreak();
		}
	}

	return VulkanTextureFormat::UNDEFINED;
}

void GAL::VulkanQueue::Wait(const class VulkanRenderDevice* renderDevice) const { vkQueueWaitIdle(queue); }

void GAL::VulkanQueue::Submit(const SubmitInfo& submitInfo)
{
	//VkTimelineSemaphoreSubmitInfo vk_timeline_semaphore_submit_info{ VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	//vk_timeline_semaphore_submit_info.waitSemaphoreValueCount = vk_wait_semaphores.GetLength();
	//vk_timeline_semaphore_submit_info.pWaitSemaphoreValues = submitInfo.WaitValues.begin();
	//vk_timeline_semaphore_submit_info.signalSemaphoreValueCount = vk_signal_semaphores.GetLength();
	//vk_timeline_semaphore_submit_info.pSignalSemaphoreValues = submitInfo.SignalValues.begin();
	
	VkSubmitInfo vkSubmitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	//vk_submit_info.pNext = &vk_timeline_semaphore_submit_info;
	
	vkSubmitInfo.commandBufferCount = submitInfo.CommandBuffers.ElementCount();
	vkSubmitInfo.pCommandBuffers = reinterpret_cast<const VkCommandBuffer*>(submitInfo.CommandBuffers.begin());
	
	vkSubmitInfo.pWaitDstStageMask = submitInfo.WaitPipelineStages.begin();
	
	vkSubmitInfo.signalSemaphoreCount = submitInfo.SignalSemaphores.ElementCount();
	vkSubmitInfo.pSignalSemaphores = reinterpret_cast<const VkSemaphore*>(submitInfo.SignalSemaphores.begin());

	vkSubmitInfo.waitSemaphoreCount = submitInfo.WaitSemaphores.ElementCount();
	vkSubmitInfo.pWaitSemaphores = reinterpret_cast<const VkSemaphore*>(submitInfo.WaitSemaphores.begin());
	
	VK_CHECK(vkQueueSubmit(queue, 1, &vkSubmitInfo, submitInfo.Fence.GetVkFence()))
}

#define GET_DEVICE_PROC(device, var) var = reinterpret_cast<PFN_##var>(vkGetDeviceProcAddr(device, #var))

bool GAL::VulkanRenderDevice::Initialize(const CreateInfo& createInfo)
{
	debugPrintFunction = createInfo.DebugPrintFunction;
	
	allocationCallbacks.pUserData = &allocationInfo;
	allocationCallbacks.pfnAllocation = &vkAllocate; allocationCallbacks.pfnReallocation = &vkReallocate; allocationCallbacks.pfnFree = &vkFree;
	allocationCallbacks.pfnInternalAllocation = nullptr; allocationCallbacks.pfnInternalFree = nullptr;

	allocationInfo = createInfo.AllocationInfo;

	debug = createInfo.Debug;

	{
		VkApplicationInfo vkApplicationInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
		//vkEnumerateInstanceVersion(&vkApplicationInfo.apiVersion);
		vkApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);
		vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(createInfo.ApplicationVersion[0], createInfo.ApplicationVersion[1], createInfo.ApplicationVersion[2]);
		vkApplicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		vkApplicationInfo.pApplicationName = createInfo.ApplicationName.begin();
		vkApplicationInfo.pEngineName = "Game-Tek | GAL";

		Array<const char*, 8> instanceLayers;

		if constexpr (_DEBUG) {
			if (debug) { instanceLayers.EmplaceBack("VK_LAYER_KHRONOS_validation"); }
		}

		Array<const char*, 16> instanceExtensions{
	#if(_DEBUG)
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	#endif
		};

		for(auto e : createInfo.Extensions)
		{
			switch (e.First)
			{
			case Extension::RAY_TRACING: break;
			case Extension::PIPELINE_CACHE_EXTERNAL_SYNC: break;
			case Extension::SCALAR_LAYOUT: break;
			case Extension::SWAPCHAIN_RENDERING:
			{
				instanceExtensions.EmplaceBack(VK_KHR_SURFACE_EXTENSION_NAME);
#if (_WIN32)
				instanceExtensions.EmplaceBack("VK_KHR_win32_surface");
#endif

				break;
			}
			default: ;
			}
		}
		
#if (_DEBUG)
		Array<VkValidationFeatureEnableEXT, 8> enables;
		if (createInfo.SynchronizationValidation) { enables.EmplaceBack(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT); };
		if (createInfo.PerformanceValidation) { enables.EmplaceBack(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT); }
		VkValidationFeaturesEXT features = {};
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount = enables.GetLength();
		features.pEnabledValidationFeatures = enables.begin();

		VkDebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfoExt{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		vkDebugUtilsMessengerCreateInfoExt.pNext = debug ? &features : nullptr;
		vkDebugUtilsMessengerCreateInfoExt.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		vkDebugUtilsMessengerCreateInfoExt.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		vkDebugUtilsMessengerCreateInfoExt.pfnUserCallback = debugCallback;
		vkDebugUtilsMessengerCreateInfoExt.pUserData = this;
#endif

		VkInstanceCreateInfo vkInstanceCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		vkInstanceCreateInfo.pNext = debug ? &vkDebugUtilsMessengerCreateInfoExt : nullptr;
		vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
		vkInstanceCreateInfo.enabledLayerCount = instanceLayers.GetLength();
		vkInstanceCreateInfo.ppEnabledLayerNames = instanceLayers.begin();
		vkInstanceCreateInfo.enabledExtensionCount = instanceExtensions.GetLength();
		vkInstanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.begin();

		if (vkCreateInstance(&vkInstanceCreateInfo, GetVkAllocationCallbacks(), &instance) != VK_SUCCESS) { return false; }

#if (_DEBUG)
		if (debug) {
			reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))(instance, &vkDebugUtilsMessengerCreateInfoExt, GetVkAllocationCallbacks(), &debugMessenger);
		}	
#endif
	}

	{
		uint32_t physicalDeviceCount{ 0 }; vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		Array<VkPhysicalDevice, 8> vkPhysicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, vkPhysicalDevices.begin());

		physicalDevice = vkPhysicalDevices[0];
	}

	{
		Array<VkDeviceQueueCreateInfo, 8> vkDeviceQueueCreateInfos; Array<Array<uint16, 8>, 8> familiesIndices;

		{
			uint32 queueFamiliesCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
			//Get the amount of queue families there are in the physical device.
			Array<VkQueueFamilyProperties, 32> vkQueueFamilyPropertieses(queueFamiliesCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, vkQueueFamilyPropertieses.begin());

			Array<bool, 8> usedFamilies(queueFamiliesCount); for (auto& e : usedFamilies) { e = false; }

			Array<VkQueueFlags, 16> vkQueuesFlagBits(createInfo.QueueCreateInfos.ElementCount());
			for (auto& e : vkQueuesFlagBits) {
				e = createInfo.QueueCreateInfos[&e - vkQueuesFlagBits.begin()].Capabilities;
			}

			Array<Array<float32, 8>, 8> familiesPriorities;

			for (uint8 QUEUE = 0; QUEUE < createInfo.QueueCreateInfos.ElementCount(); ++QUEUE) {
				for (uint8 FAMILY = 0; FAMILY < queueFamiliesCount; ++FAMILY) {
					if (vkQueueFamilyPropertieses[FAMILY].queueCount > 0 && vkQueueFamilyPropertieses[FAMILY].queueFlags & vkQueuesFlagBits[QUEUE]) //if family has vk_queue_flags_bits[FAMILY] create queue from this family
					{
						if (usedFamilies[FAMILY]) //if a queue is already being used from this family add another
						{
							++vkDeviceQueueCreateInfos[FAMILY].queueCount;
							familiesPriorities[FAMILY].EmplaceBack(createInfo.QueueCreateInfos[QUEUE].QueuePriority);
							familiesIndices[FAMILY].EmplaceBack(QUEUE);
							vkDeviceQueueCreateInfos[FAMILY].pQueuePriorities = &familiesPriorities[FAMILY][vkDeviceQueueCreateInfos[FAMILY].queueCount - 1];
							break;
						}

						vkDeviceQueueCreateInfos.EmplaceBack();
						familiesPriorities.EmplaceBack();
						familiesIndices.EmplaceBack();

						usedFamilies[FAMILY] = true;

						familiesPriorities[FAMILY].EmplaceBack(createInfo.QueueCreateInfos[QUEUE].QueuePriority);
						familiesIndices[FAMILY].EmplaceBack(QUEUE);

						vkDeviceQueueCreateInfos[FAMILY].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
						vkDeviceQueueCreateInfos[FAMILY].pNext = nullptr;
						vkDeviceQueueCreateInfos[FAMILY].flags = 0;
						vkDeviceQueueCreateInfos[FAMILY].queueFamilyIndex = FAMILY;
						vkDeviceQueueCreateInfos[FAMILY].queueCount = 1;
						vkDeviceQueueCreateInfos[FAMILY].pQueuePriorities = &familiesPriorities[FAMILY][vkDeviceQueueCreateInfos[FAMILY].queueCount - 1];
						break;
					}
				}
			}
		}

		VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
		VkPhysicalDeviceFeatures2 features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

		features2.features.samplerAnisotropy = true;
		features2.features.shaderSampledImageArrayDynamicIndexing = true;
		features2.features.shaderStorageImageArrayDynamicIndexing = true;
		features2.features.shaderUniformBufferArrayDynamicIndexing = true;
		features2.features.shaderStorageBufferArrayDynamicIndexing = true;
		features2.features.shaderInt16 = true;
		features2.features.shaderInt64 = true;
		features2.features.shaderStorageImageReadWithoutFormat = true;
		features2.features.shaderStorageImageWriteWithoutFormat = true;

		void** lastProperty = &properties2.pNext; void** lastFeature = &features2.pNext;

		{
			Array<byte, 24 * 256> structsBuffer; Array<StaticString<32>, 32> deviceExtensions;

			auto placePropertiesStructure = [&]<typename T>(T structure) {
				structsBuffer.Resize(structsBuffer.GetLength() + static_cast<uint32>(sizeof(structure)));
				MemCopy(sizeof(structure), &structure, structsBuffer.end() - sizeof(structure));
				*lastProperty = structsBuffer.end() - sizeof(structure);
				lastProperty = &static_cast<T*>(*lastProperty)->pNext;
			};

			auto placeFeaturesStructure = [&]<typename T>(T structure) {
				structsBuffer.Resize(structsBuffer.GetLength() + static_cast<uint32>(sizeof(structure)));
				MemCopy(sizeof(structure), &structure, structsBuffer.end() - sizeof(structure));
				*lastFeature = structsBuffer.end() - sizeof(structure);
				lastFeature = &static_cast<T*>(*lastFeature)->pNext;
			};

			auto getProperties = [&](void* prop) {
				VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
				props.pNext = prop;
				vkGetPhysicalDeviceProperties2(physicalDevice, &props);
			};

			auto getFeatures = [&](void* feature) {
				VkPhysicalDeviceFeatures2 feats{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
				feats.pNext = feature;
				vkGetPhysicalDeviceFeatures2(physicalDevice, &feats);
			};

			[[no_discard]] auto tryAddExtension = [&](const char* extensionName) {
				StaticString<32> name(extensionName);
				auto searchResult = deviceExtensions.Find(name);
				if (!searchResult.State()) { deviceExtensions.EmplaceBack(name); return true; }
				return false;
			};

			{
				VkPhysicalDeviceTimelineSemaphoreFeatures vkPhysicalDeviceTimelineSemaphoreFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES };
				vkPhysicalDeviceTimelineSemaphoreFeatures.timelineSemaphore = true;
				placeFeaturesStructure(vkPhysicalDeviceTimelineSemaphoreFeatures);
			}

			tryAddExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			{
				VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures vkPhysicalDeviceSeparateDepthStencilLayoutsFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES };
				vkPhysicalDeviceSeparateDepthStencilLayoutsFeatures.separateDepthStencilLayouts = true;
				placeFeaturesStructure(vkPhysicalDeviceSeparateDepthStencilLayoutsFeatures);
			}

			{
				VkPhysicalDeviceDescriptorIndexingFeatures vkPhysicalDeviceDescriptorIndexingFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES };
				vkPhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray = true;
				vkPhysicalDeviceDescriptorIndexingFeatures.descriptorBindingPartiallyBound = true;
				vkPhysicalDeviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;
				vkPhysicalDeviceDescriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = true;
				vkPhysicalDeviceDescriptorIndexingFeatures.shaderStorageImageArrayNonUniformIndexing = true;
				vkPhysicalDeviceDescriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = true;

				placeFeaturesStructure(vkPhysicalDeviceDescriptorIndexingFeatures);
			}

			{
				VkPhysicalDeviceBufferDeviceAddressFeatures vkBufferAddressFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES };
				vkBufferAddressFeatures.bufferDeviceAddress = true;
				placeFeaturesStructure(vkBufferAddressFeatures);
			}

			for (uint32 extension = 0; extension < createInfo.Extensions.ElementCount(); ++extension)
			{
				switch (createInfo.Extensions[extension].First)
				{
				case Extension::RAY_TRACING:
				{
					if (tryAddExtension("VK_KHR_acceleration_structure")) {
						VkPhysicalDeviceAccelerationStructureFeaturesKHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
						VkPhysicalDeviceAccelerationStructurePropertiesKHR properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR };

						features.accelerationStructure = true;
						features.accelerationStructureHostCommands;

						placePropertiesStructure(properties);
						placeFeaturesStructure(features);

						getProperties(&properties);
						getFeatures(&features);

						auto* capabilities = static_cast<RayTracingCapabilities*>(createInfo.Extensions[extension].Second);
						capabilities->CanBuildOnHost = features.accelerationStructureHostCommands;
						capabilities->ScratchBuildOffsetAlignment = properties.minAccelerationStructureScratchOffsetAlignment;
					}

					tryAddExtension("VK_KHR_ray_query");

					if (tryAddExtension("VK_KHR_ray_tracing_pipeline")) {
						VkPhysicalDeviceRayTracingPipelineFeaturesKHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
						VkPhysicalDeviceRayTracingPipelinePropertiesKHR properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };

						features.rayTracingPipeline = true;

						placePropertiesStructure(properties);
						placeFeaturesStructure(features);

						getProperties(&properties);
						getFeatures(&features);

						auto* capabilities = static_cast<RayTracingCapabilities*>(createInfo.Extensions[extension].Second);
						capabilities->RecursionDepth = properties.maxRayRecursionDepth;
						capabilities->ShaderGroupAlignment = properties.shaderGroupHandleAlignment;
						capabilities->ShaderGroupBaseAlignment = properties.shaderGroupBaseAlignment;
						capabilities->ShaderGroupHandleSize = properties.shaderGroupHandleSize;
					}

					if (tryAddExtension(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME)) {}

					if (tryAddExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME)) {}

					break;
				}
				case Extension::PIPELINE_CACHE_EXTERNAL_SYNC:
				{
					VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT pipelineCacheSyncControl{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT };
					pipelineCacheSyncControl.pipelineCreationCacheControl = true;
					placeFeaturesStructure(pipelineCacheSyncControl);

					break;
				}
				case Extension::SCALAR_LAYOUT:
				{
					VkPhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayoutFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES };
					scalarBlockLayoutFeatures.scalarBlockLayout = true;
					placeFeaturesStructure(scalarBlockLayoutFeatures);
					break;
				}
				case Extension::SWAPCHAIN_RENDERING: break;
				default: __debugbreak();
				}
			}

			VkDeviceCreateInfo vkDeviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
			vkDeviceCreateInfo.pNext = &features2; //extended features
			vkDeviceCreateInfo.queueCreateInfoCount = vkDeviceQueueCreateInfos.GetLength();
			vkDeviceCreateInfo.pQueueCreateInfos = vkDeviceQueueCreateInfos.begin();
			vkDeviceCreateInfo.pEnabledFeatures = nullptr;
			vkDeviceCreateInfo.enabledExtensionCount = deviceExtensions.GetLength();

			Array<const char*, 32> strings(deviceExtensions.GetLength()); {
				for (uint32 i = 0; i < deviceExtensions.GetLength(); ++i) { strings[i] = deviceExtensions[i].begin(); }
			}

			vkDeviceCreateInfo.ppEnabledExtensionNames = strings.begin();

			if (vkCreateDevice(physicalDevice, &vkDeviceCreateInfo, GetVkAllocationCallbacks(), &device) != VK_SUCCESS) { return false; }

			vkGetPhysicalDeviceProperties2(physicalDevice, &properties2); vkGetPhysicalDeviceFeatures2(physicalDevice, &features2);

			uniformBufferMinOffset = properties2.properties.limits.minUniformBufferOffsetAlignment;
			storageBufferMinOffset = properties2.properties.limits.minStorageBufferOffsetAlignment;
			linearNonLinearAlignment = properties2.properties.limits.bufferImageGranularity;
		}

		for (uint8 FAMILY = 0; FAMILY < familiesIndices.GetLength(); ++FAMILY) {
			for (uint8 QUEUE = 0; QUEUE < familiesIndices[FAMILY].GetLength(); ++QUEUE) {
				createInfo.Queues[familiesIndices[FAMILY][QUEUE]]->familyIndex = FAMILY;
				createInfo.Queues[familiesIndices[FAMILY][QUEUE]]->queueIndex = QUEUE;
				vkGetDeviceQueue(device, FAMILY, QUEUE, &createInfo.Queues[familiesIndices[FAMILY][QUEUE]]->queue);
			}
		}
	}

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (auto e : createInfo.Extensions)
	{
		switch (e.First)
		{
		case Extension::RAY_TRACING:
		{
			GET_DEVICE_PROC(device, vkCreateAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkDestroyAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkCreateRayTracingPipelinesKHR);
			GET_DEVICE_PROC(device, vkGetAccelerationStructureBuildSizesKHR);
			GET_DEVICE_PROC(device, vkGetRayTracingShaderGroupHandlesKHR);
			GET_DEVICE_PROC(device, vkBuildAccelerationStructuresKHR);
			GET_DEVICE_PROC(device, vkCmdBuildAccelerationStructuresKHR);
			GET_DEVICE_PROC(device, vkGetAccelerationStructureDeviceAddressKHR);
			GET_DEVICE_PROC(device, vkCreateDeferredOperationKHR);
			GET_DEVICE_PROC(device, vkDeferredOperationJoinKHR);
			GET_DEVICE_PROC(device, vkGetDeferredOperationResultKHR);
			GET_DEVICE_PROC(device, vkGetDeferredOperationMaxConcurrencyKHR);
			GET_DEVICE_PROC(device, vkDestroyDeferredOperationKHR);
			GET_DEVICE_PROC(device, vkCmdCopyAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkCmdCopyAccelerationStructureToMemoryKHR);
			GET_DEVICE_PROC(device, vkCmdCopyMemoryToAccelerationStructureKHR);
			GET_DEVICE_PROC(device, vkCmdWriteAccelerationStructuresPropertiesKHR);
			GET_DEVICE_PROC(device, vkCmdTraceRaysKHR);

			break;
		}
		default:;
		}
	}

	if constexpr (_DEBUG) {
		vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
		vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
		vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
		vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));

		//NVIDIA's driver have a bug when setting the name for this 3 object types, TODO. fix in the future
		//{
		//	StaticString<128> name(createInfo.ApplicationName);
		//	name += " instance"; name += '\0';
		//
		//	VkDebugUtilsObjectNameInfoEXT debug_utils_object_name_info_ext{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		//	debug_utils_object_name_info_ext.objectHandle = reinterpret_cast<uint64>(instance);
		//	debug_utils_object_name_info_ext.objectType = VK_OBJECT_TYPE_INSTANCE;
		//	debug_utils_object_name_info_ext.pObjectName = name.begin();
		//	//debug_utils_object_name_info_ext.pObjectName = "Instance";
		//	vkSetDebugUtilsObjectNameEXT(device, &debug_utils_object_name_info_ext);
		//}
		//
		//{
		//	StaticString<128> name(createInfo.ApplicationName);
		//	name += " physical device"; name += '\0';
		//
		//	VkDebugUtilsObjectNameInfoEXT debug_utils_object_name_info_ext{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		//	debug_utils_object_name_info_ext.objectHandle = reinterpret_cast<uint64>(physicalDevice);
		//	debug_utils_object_name_info_ext.objectType = VK_OBJECT_TYPE_PHYSICAL_DEVICE;
		//	debug_utils_object_name_info_ext.pObjectName = name.begin();
		//	//debug_utils_object_name_info_ext.pObjectName = "PhysicalDevice";
		//	vkSetDebugUtilsObjectNameEXT(device, &debug_utils_object_name_info_ext);
		//}
		//
		//{
		//	StaticString<128> name(createInfo.ApplicationName);
		//	name += " device"; name += '\0';
		//	
		//	VkDebugUtilsObjectNameInfoEXT debug_utils_object_name_info_ext{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		//	debug_utils_object_name_info_ext.objectHandle = reinterpret_cast<uint64>(device);
		//	debug_utils_object_name_info_ext.objectType = VK_OBJECT_TYPE_DEVICE;
		//	debug_utils_object_name_info_ext.pObjectName = name.begin();
		//	//debug_utils_object_name_info_ext.pObjectName = "Device";
		//	vkSetDebugUtilsObjectNameEXT(device, &debug_utils_object_name_info_ext);
		//}
	}

	return true;
}

GAL::VulkanRenderDevice::~VulkanRenderDevice()
{
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, GetVkAllocationCallbacks());
	
#if (_DEBUG)
	if (debug) {
		reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))(instance, debugMessenger, GetVkAllocationCallbacks());
	}
	debugClear(debugMessenger);
#endif
	
	vkDestroyInstance(instance, GetVkAllocationCallbacks());

	debugClear(device); debugClear(instance);
}
