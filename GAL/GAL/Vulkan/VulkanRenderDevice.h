#pragma once

#include "GAL/RenderDevice.h"

#include "Vulkan.h"

#include <GTSL/Pair.h>

#include "GTSL/Array.hpp"
#include "GTSL/Buffer.hpp"
#include "GTSL/Allocator.h"
#include "GTSL/DataSizes.h"
#include "GTSL/DLL.h"

namespace GAL
{
#undef ERROR
	
	class VulkanRenderDevice final : public RenderDevice
	{
	public:
		struct RayTracingCapabilities
		{
			GTSL::uint32 RecursionDepth = 0, ShaderGroupHandleAlignment = 0, ShaderGroupBaseAlignment = 0, ShaderGroupHandleSize = 0, ScratchBuildOffsetAlignment = 0;
			bool CanBuildOnHost = false;
		};

		VulkanRenderDevice() = default;

		struct QueueKey {
			GTSL::uint32 Family, Queue;
		};
		
		struct CreateInfo
		{
			GTSL::Range<const GTSL::UTF8*> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Range<const QueueType*> Queues;
			GTSL::Range<QueueKey*> QueueKeys;
			GTSL::Delegate<void(const char*, MessageSeverity)> DebugPrintFunction;
			bool Debug = false;
			bool PerformanceValidation = false;
			bool SynchronizationValidation = false;
			GTSL::Range<const GTSL::Pair<Extension, void*>*> Extensions;
			AllocationInfo AllocationInfo;
		};

		[[nodiscard]] bool Initialize(const CreateInfo& createInfo) {
			debugPrintFunction = createInfo.DebugPrintFunction;
			
			if (!vulkanDLL.LoadLibrary("vulkan-1")) { return false; }
			
			auto vkAllocate = [](void* data, GTSL::uint64 size, GTSL::uint64 alignment, VkSystemAllocationScope) {
				auto* allocation_info = static_cast<AllocationInfo*>(data);
				return allocation_info->Allocate(allocation_info->UserData, size, alignment);
			};

			auto vkReallocate = [](void* data, void* originalAlloc, const GTSL::uint64 size, const GTSL::uint64 alignment, VkSystemAllocationScope) -> void* {
				auto* allocation_info = static_cast<AllocationInfo*>(data);

				if (originalAlloc && size) {
					return allocation_info->Reallocate(allocation_info->UserData, originalAlloc, size, alignment);
				}

				if (!originalAlloc && size) {
					return allocation_info->Allocate(allocation_info->UserData, size, alignment);
				}

				allocation_info->Deallocate(allocation_info->UserData, originalAlloc);
				return nullptr;
			};

			auto vkFree = [](void* data, void* alloc) -> void {
				if (alloc) {
					auto* allocation_info = static_cast<AllocationInfo*>(data);
					allocation_info->Deallocate(allocation_info->UserData, alloc);
				}
			};
			
			allocationCallbacks.pUserData = &allocationInfo;
			allocationCallbacks.pfnAllocation = vkAllocate; allocationCallbacks.pfnReallocation = vkReallocate; allocationCallbacks.pfnFree = vkFree;
			allocationCallbacks.pfnInternalAllocation = nullptr; allocationCallbacks.pfnInternalFree = nullptr;

			allocationInfo = createInfo.AllocationInfo; debug = createInfo.Debug;

			VkDeviceCreateInfo vkDeviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

			{
				VkApplicationInfo vkApplicationInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
				//vkEnumerateInstanceVersion(&vkApplicationInfo.apiVersion);
				vkApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);
				vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(createInfo.ApplicationVersion[0], createInfo.ApplicationVersion[1], createInfo.ApplicationVersion[2]);
				vkApplicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
				vkApplicationInfo.pApplicationName = createInfo.ApplicationName.begin();
				vkApplicationInfo.pEngineName = "Game-Tek | GAL";

				VkInstanceCreateInfo vkInstanceCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

				auto setInstancepNext = [&](void* newPointer)
				{
					if (vkInstanceCreateInfo.pNext) {
						//pointer to last structure now extending vkInstanceCreateInfo
						auto* str = static_cast<GTSL::byte*>(const_cast<void*>(vkInstanceCreateInfo.pNext)); //constness is only there to guarantee VK will not touch it, WE can do it with no problem
						void** strpNext = reinterpret_cast<void**>(str + sizeof(VkStructureType));
						*strpNext = newPointer;
						return;
					}

					vkInstanceCreateInfo.pNext = newPointer;
				};

				GTSL::Array<const char*, 8> instanceLayers;

				if constexpr (_DEBUG) {
					if (debug) { instanceLayers.EmplaceBack("VK_LAYER_KHRONOS_validation"); }
				}

				GTSL::Array<const char*, 16> instanceExtensions{
			#if(_DEBUG)
					VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			#endif
				};

				for (auto e : createInfo.Extensions)
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
					default:;
					}
				}

#if (_DEBUG)
				GTSL::Array<VkValidationFeatureEnableEXT, 8> enables;
				if (createInfo.SynchronizationValidation) { enables.EmplaceBack(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT); };
				if (createInfo.PerformanceValidation) { enables.EmplaceBack(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT); }
				VkValidationFeaturesEXT features = {};
				features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
				features.enabledValidationFeatureCount = enables.GetLength();
				features.pEnabledValidationFeatures = enables.begin();

				setInstancepNext(&features);

				auto VKAPI_CALL debugCallback = [](const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> VkBool32
				{
					auto* deviceCallback = static_cast<GAL::VulkanRenderDevice*>(pUserData);

					switch (messageSeverity) {
					case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
						//device->GetDebugPrintFunction()(pCallbackData->pMessage, GAL::RenderDevice::MessageSeverity::MESSAGE);
						break;
					}
					case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
						deviceCallback->GetDebugPrintFunction()(pCallbackData->pMessage, MessageSeverity::MESSAGE);
						break;
					}
					case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
						deviceCallback->GetDebugPrintFunction()(pCallbackData->pMessage, MessageSeverity::WARNING);
						break;
					}
					case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
						deviceCallback->GetDebugPrintFunction()(pCallbackData->pMessage, MessageSeverity::ERROR);
						break;
					}
					case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
					default:;
					}

					return VK_FALSE;
				};
				
				VkDebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfoExt{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
				vkDebugUtilsMessengerCreateInfoExt.pNext = nullptr;
				vkDebugUtilsMessengerCreateInfoExt.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				vkDebugUtilsMessengerCreateInfoExt.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				vkDebugUtilsMessengerCreateInfoExt.pfnUserCallback = debugCallback;
				vkDebugUtilsMessengerCreateInfoExt.pUserData = this;
#endif

				vkInstanceCreateInfo.pNext = debug ? &vkDebugUtilsMessengerCreateInfoExt : nullptr;
				vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
				vkInstanceCreateInfo.enabledLayerCount = instanceLayers.GetLength();
				vkInstanceCreateInfo.ppEnabledLayerNames = instanceLayers.begin();
				vkInstanceCreateInfo.enabledExtensionCount = instanceExtensions.GetLength();
				vkInstanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.begin();

				if (vulkanDLL.LoadDynamicFunction("vkCreateInstance").Call<VkResult>(&vkInstanceCreateInfo, GetVkAllocationCallbacks(), &instance) != VK_SUCCESS) { return false; }

#if (_DEBUG)
				if (debug) {
					vulkanDLL.LoadDynamicFunction("vkCreateDebugUtilsMessengerEXT").Call<VkResult>(instance, &vkDebugUtilsMessengerCreateInfoExt, GetVkAllocationCallbacks(), &debugMessenger);
				}
#endif
			}

			{
				uint32_t physicalDeviceCount{ 16 }; VkPhysicalDevice vkPhysicalDevices[16];
				vulkanDLL.LoadDynamicFunction("vkEnumeratePhysicalDevices").Call<VkResult>(instance, &physicalDeviceCount, vkPhysicalDevices);
				physicalDevice = vkPhysicalDevices[0];
			}

			{
				GTSL::Array<VkDeviceQueueCreateInfo, 8> vkDeviceQueueCreateInfos; GTSL::Array<GTSL::Array<GTSL::uint16, 8>, 8> familiesIndices;
				
				{
					GTSL::uint32 queueFamiliesCount = 32; VkQueueFamilyProperties vkQueueFamiliesProperties[32];
					//Get the amount of queue families there are in the physical device.
					vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceQueueFamilyProperties").Call<VkResult>(physicalDevice, &queueFamiliesCount, vkQueueFamiliesProperties);

					VkQueueFlags vkQueuesFlagBits[16];
					for (GTSL::uint8 i = 0; i < createInfo.Queues.ElementCount(); ++i) {
						vkQueuesFlagBits[i] = ToVulkan(createInfo.Queues[i]);
					}

					GTSL::Array<GTSL::Array<GTSL::float32, 8>, 8> familiesPriorities;

					for (GTSL::uint8 QUEUE = 0; QUEUE < createInfo.Queues.ElementCount(); ++QUEUE) {
						for (GTSL::uint8 FAMILY = 0; FAMILY < queueFamiliesCount; ++FAMILY) {
							if (vkQueueFamiliesProperties[FAMILY].queueCount > 0 && vkQueueFamiliesProperties[FAMILY].queueFlags & vkQueuesFlagBits[QUEUE]) //if family has vk_queue_flags_bits[FAMILY] create queue from this family
							{
								if (familiesIndices[FAMILY].GetLength()) { //if a queue is already being used from this family add another
									++vkDeviceQueueCreateInfos[FAMILY].queueCount;
								} else {
									vkDeviceQueueCreateInfos.EmplaceBack(); familiesPriorities.EmplaceBack(); familiesIndices.EmplaceBack();
									
									vkDeviceQueueCreateInfos[FAMILY].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
									vkDeviceQueueCreateInfos[FAMILY].pNext = nullptr;
									vkDeviceQueueCreateInfos[FAMILY].flags = 0;
									vkDeviceQueueCreateInfos[FAMILY].queueFamilyIndex = FAMILY;
									vkDeviceQueueCreateInfos[FAMILY].queueCount = 1;
								}
								
								familiesPriorities[FAMILY].EmplaceBack(1.0f);
								familiesIndices[FAMILY].EmplaceBack(QUEUE);
								vkDeviceQueueCreateInfos[FAMILY].pQueuePriorities = &familiesPriorities[FAMILY][vkDeviceQueueCreateInfos[FAMILY].queueCount - 1];
							}
						}
					}
				}

				VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 }; VkPhysicalDeviceFeatures2 features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

				features2.features.samplerAnisotropy = true;
				features2.features.shaderSampledImageArrayDynamicIndexing = true;
				features2.features.shaderStorageImageArrayDynamicIndexing = true;
				features2.features.shaderUniformBufferArrayDynamicIndexing = true;
				features2.features.shaderStorageBufferArrayDynamicIndexing = true;
				features2.features.shaderInt16 = true; features2.features.shaderInt64 = true;
				features2.features.robustBufferAccess = createInfo.Debug;
				features2.features.shaderStorageImageReadWithoutFormat = true; features2.features.shaderStorageImageWriteWithoutFormat = true;

				void** lastProperty = &properties2.pNext; void** lastFeature = &features2.pNext;

				{
					GTSL::Buffer<GTSL::StackAllocator<8192>> buffer; GTSL::Array<GTSL::StaticString<32>, 32> deviceExtensions;

					auto placePropertiesStructure = [&]<typename T>(T** structure, VkStructureType structureType) {
						auto* newStructure = buffer.AllocateStructure<T>(); *lastProperty = static_cast<void*>(newStructure);
						*structure = newStructure; newStructure->sType = structureType;
						lastProperty = &newStructure->pNext;
					};

					auto placeFeaturesStructure = [&]<typename T>(T** structure, VkStructureType structureType) {
						auto* newStructure = buffer.AllocateStructure<T>(); *lastFeature = static_cast<void*>(newStructure);
						*structure = newStructure; newStructure->sType = structureType;
						lastFeature = &newStructure->pNext;
					};

					auto getProperties = [&](void* prop) {
						VkPhysicalDeviceProperties2 props{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
						props.pNext = prop;
						vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceProperties2").Call<VkResult>(physicalDevice, &props);
					};

					auto getFeatures = [&](void* feature) {
						VkPhysicalDeviceFeatures2 feats{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
						feats.pNext = feature;
						vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceFeatures2").Call<VkResult>(physicalDevice, &feats);
					};

					[[no_discard]] auto tryAddExtension = [&](const char* extensionName) {
						GTSL::StaticString<32> name(extensionName);
						auto searchResult = deviceExtensions.Find(name);
						if (!searchResult.State()) { deviceExtensions.EmplaceBack(name); return true; }
						return false;
					};

					{
						VkPhysicalDeviceVulkan11Features* structure;
						placeFeaturesStructure(&structure, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
						structure->storageBuffer16BitAccess = true; structure->storagePushConstant16 = true;
					}

					{
						VkPhysicalDeviceVulkan12Features* structure;
						placeFeaturesStructure(&structure, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
						structure->separateDepthStencilLayouts = true; structure->timelineSemaphore = true;
						structure->bufferDeviceAddress = true; structure->descriptorIndexing = true;
						structure->scalarBlockLayout = true; structure->shaderInt8 = true;
						structure->storageBuffer8BitAccess = true; structure->runtimeDescriptorArray = true;
						structure->descriptorBindingPartiallyBound = true; structure->shaderSampledImageArrayNonUniformIndexing = true;
						structure->shaderStorageBufferArrayNonUniformIndexing = true; structure->shaderStorageImageArrayNonUniformIndexing = true;
						structure->shaderUniformBufferArrayNonUniformIndexing = true;
					}

					tryAddExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

					for (GTSL::uint32 extension = 0; extension < createInfo.Extensions.ElementCount(); ++extension)
					{
						switch (createInfo.Extensions[extension].First)
						{
						case Extension::RAY_TRACING: {
							if (tryAddExtension("VK_KHR_acceleration_structure")) {
								{
									VkPhysicalDeviceAccelerationStructureFeaturesKHR* features;
									VkPhysicalDeviceAccelerationStructurePropertiesKHR* properties;

									placePropertiesStructure(&properties, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR);
									placeFeaturesStructure(&features, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR);

									features->accelerationStructure = true;
									features->accelerationStructureHostCommands;
								}

								VkPhysicalDeviceAccelerationStructureFeaturesKHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
								VkPhysicalDeviceAccelerationStructurePropertiesKHR properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR };

								getProperties(&properties);
								getFeatures(&features);

								auto* capabilities = static_cast<RayTracingCapabilities*>(createInfo.Extensions[extension].Second);
								capabilities->CanBuildOnHost = features.accelerationStructureHostCommands;
								capabilities->ScratchBuildOffsetAlignment = properties.minAccelerationStructureScratchOffsetAlignment;
							}

							tryAddExtension("VK_KHR_ray_query");

							if (tryAddExtension("VK_KHR_ray_tracing_pipeline")) {
								{
									VkPhysicalDeviceRayTracingPipelineFeaturesKHR* features;
									VkPhysicalDeviceRayTracingPipelinePropertiesKHR* properties;

									placePropertiesStructure(&properties, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR);
									placeFeaturesStructure(&features, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR);

									features->rayTracingPipeline = true;
								}

								VkPhysicalDeviceRayTracingPipelineFeaturesKHR features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
								VkPhysicalDeviceRayTracingPipelinePropertiesKHR properties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };

								getProperties(&properties); getFeatures(&features);
								
								auto* capabilities = static_cast<RayTracingCapabilities*>(createInfo.Extensions[extension].Second);
								capabilities->RecursionDepth = properties.maxRayRecursionDepth;
								capabilities->ShaderGroupHandleAlignment = properties.shaderGroupHandleAlignment;
								capabilities->ShaderGroupBaseAlignment = properties.shaderGroupBaseAlignment;
								capabilities->ShaderGroupHandleSize = properties.shaderGroupHandleSize;
							}

							if (tryAddExtension(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME)) {}

							if (tryAddExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME)) {}

							break;
						}
						case Extension::PIPELINE_CACHE_EXTERNAL_SYNC:
						{
							VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT* pipelineCacheSyncControl;
							placeFeaturesStructure(&pipelineCacheSyncControl, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT);
							pipelineCacheSyncControl->pipelineCreationCacheControl = true;

							break;
						}
						case Extension::SWAPCHAIN_RENDERING: break;
						}
					}

					vkDeviceCreateInfo.pNext = &features2; //extended features
					vkDeviceCreateInfo.queueCreateInfoCount = vkDeviceQueueCreateInfos.GetLength();
					vkDeviceCreateInfo.pQueueCreateInfos = vkDeviceQueueCreateInfos.begin();
					vkDeviceCreateInfo.pEnabledFeatures = nullptr;
					vkDeviceCreateInfo.enabledExtensionCount = deviceExtensions.GetLength();

					GTSL::Array<const char*, 32> strings; {
						for (GTSL::uint32 i = 0; i < deviceExtensions.GetLength(); ++i) { strings.EmplaceBack(deviceExtensions[i].begin()); }
					}

					vkDeviceCreateInfo.ppEnabledExtensionNames = strings.begin();

					if (vulkanDLL.LoadDynamicFunction("vkCreateDevice").Call<VkResult>(physicalDevice, &vkDeviceCreateInfo, GetVkAllocationCallbacks(), &device) != VK_SUCCESS) { return false; }

					vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceProperties2").Call<VkResult>(physicalDevice, &properties2);
					vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceFeatures2").Call<VkResult>(physicalDevice, &features2);

					uniformBufferMinOffset = properties2.properties.limits.minUniformBufferOffsetAlignment;
					storageBufferMinOffset = properties2.properties.limits.minStorageBufferOffsetAlignment;
					linearNonLinearAlignment = properties2.properties.limits.bufferImageGranularity;
				}

				for (GTSL::uint8 FAMILY = 0; FAMILY < familiesIndices.GetLength(); ++FAMILY) {
					for (GTSL::uint8 QUEUE = 0; QUEUE < familiesIndices[FAMILY].GetLength(); ++QUEUE) {
						createInfo.QueueKeys[familiesIndices[FAMILY][QUEUE]].Family = FAMILY;
						createInfo.QueueKeys[familiesIndices[FAMILY][QUEUE]].Queue = QUEUE;
					}
				}
			}

			vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceMemoryProperties").Call<VkResult>(physicalDevice, &memoryProperties);

			vulkanDLL.LoadDynamicFunction("vkQueueSubmit", &VkQueueSubmit);
			vulkanDLL.LoadDynamicFunction("vkQueuePresetKHR", &VkQueuePresent);

			vulkanDLL.LoadDynamicFunction("vkCreateSwapchainKHR", &VkCreateSwapchain);
			vulkanDLL.LoadDynamicFunction("vkGetSwapchainImagesKHR", &VkGetSwapchainImages);
			vulkanDLL.LoadDynamicFunction("vkAcquireNextImageKHR", &VkAcquireNextImage);
			vulkanDLL.LoadDynamicFunction("vkDestroySwapchainKHR", &VkDestroySwapchain);

			vulkanDLL.LoadDynamicFunction("vkCreateWin32SurfaceKHR", &VkCreateWin32Surface);
			vulkanDLL.LoadDynamicFunction("vkDestroySurfaceKHR", &VkDestroySurface);

			vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", &VkGetPhysicalDeviceSurfaceCapabilities);
			vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceSurfaceFormatsKHR", &VkGetPhysicalDeviceSurfaceFormats);
			vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceSurfacePresentModesKHR", &VkGetPhysicalDeviceSurfacePresentModes);
			vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceSurfaceSupportKHR", &VkGetPhysicalDeviceSurfaceSupport);

			vulkanDLL.LoadDynamicFunction("vkCreateBuffer", &VkCreateBuffer);
			vulkanDLL.LoadDynamicFunction("vkGetBufferDeviceAddress", &VkGetBufferDeviceAddress);
			vulkanDLL.LoadDynamicFunction("vkDestroyBuffer", &VkDestroyBuffer);
			vulkanDLL.LoadDynamicFunction("vkGetBufferMemoryRequirements", &VkGetBufferMemoryRequirements);
			vulkanDLL.LoadDynamicFunction("vkBindBufferMemory", &VkBindBufferMemory);
			
			vulkanDLL.LoadDynamicFunction("vkCreateImage", &VkCreateImage);			
			vulkanDLL.LoadDynamicFunction("vkDestroyImage", &VkDestroyImage);
			vulkanDLL.LoadDynamicFunction("vkGetImageMemoryRequirements", &VkGetImageMemoryRequirements);
			vulkanDLL.LoadDynamicFunction("vkBindImageMemory", &VkBindImageMemory);
			
			vulkanDLL.LoadDynamicFunction("vkCreateCommandPool", &VkCreateCommandPool);
			vulkanDLL.LoadDynamicFunction("vkDestroyCommandPool", &VkDestroyCommandPool);
			vulkanDLL.LoadDynamicFunction("vkResetCommandPool", &VkResetCommandPool);
			vulkanDLL.LoadDynamicFunction("vkAllocateCommandBuffers", &VkAllocateCommandBuffers);
			vulkanDLL.LoadDynamicFunction("vkBeginCommandBuffer", &VkBeginCommandBuffer);
			vulkanDLL.LoadDynamicFunction("vkEndCommandBuffer", &VkEndCommandBuffer);
			
			vulkanDLL.LoadDynamicFunction("vkCreateRenderPass", &VkCreateRenderPass);
			vulkanDLL.LoadDynamicFunction("vkDestroyRenderPass", &VkDestroyRenderPass);
			vulkanDLL.LoadDynamicFunction("vkCreateFramebuffer", &VkCreateFramebuffer);
			vulkanDLL.LoadDynamicFunction("vkDestroyFramebuffer", &VkDestroyFramebuffer);
			
			vulkanDLL.LoadDynamicFunction("vkCreateShaderModule", &VkCreateShaderModule);
			vulkanDLL.LoadDynamicFunction("vkDestroyShaderModule", &VkDestroyShaderModule);
			
			vulkanDLL.LoadDynamicFunction("vkCreatePipelineLayout", &VkCreatePipelineLayout);
			vulkanDLL.LoadDynamicFunction("vkDestroyPipelineLayout", &VkDestroyPipelineLayout);
			
			vulkanDLL.LoadDynamicFunction("vkCreatePipelineCache", &VkCreatePipelineCache);
			vulkanDLL.LoadDynamicFunction("vkMergePipelineCaches", &VkMergePipelineCaches);
			vulkanDLL.LoadDynamicFunction("vkGetPipelineCacheData", &VkGetPipelineCacheData);
			vulkanDLL.LoadDynamicFunction("vkDestroyPipelineCache", &VkDestroyPipelineCache);
			
			vulkanDLL.LoadDynamicFunction("vkCreateDescriptorSetLayout", &VkCreateDescriptorSetLayout);
			vulkanDLL.LoadDynamicFunction("vkDestroyDescriptorSetLayout", &VkDestroyDescriptorSetLayout);
			
			vulkanDLL.LoadDynamicFunction("vkCreateDescriptorPool", &VkCreateDescriptorPool);
			vulkanDLL.LoadDynamicFunction("vkAllocateDescriptorSets", &VkAllocateDescriptorSets);
			vulkanDLL.LoadDynamicFunction("vkUpdateDescriptorSets", &VkUpdateDescriptorSets);
			vulkanDLL.LoadDynamicFunction("vkDestroyDescriptorPool", &VkDestroyDescriptorPool);
			
			vulkanDLL.LoadDynamicFunction("vkCreateFence", &VkCreateFence);
			vulkanDLL.LoadDynamicFunction("vkWaitForFences", &VkWaitForFences);
			vulkanDLL.LoadDynamicFunction("vkResetFences", &VkResetFences);
			vulkanDLL.LoadDynamicFunction("vkDestroyFence", &VkDestroyFence);
			vulkanDLL.LoadDynamicFunction("vkCreateSemaphore", &VkCreateSemaphore);
			vulkanDLL.LoadDynamicFunction("vkDestroySemaphore", &VkDestroySemaphore);
			vulkanDLL.LoadDynamicFunction("vkCreateEvent", &VkCreateEvent);
			vulkanDLL.LoadDynamicFunction("vkSetEvent", &VkSetEvent);
			vulkanDLL.LoadDynamicFunction("vkResetEvent", &VkResetEvent);
			vulkanDLL.LoadDynamicFunction("vkDestroyEvent", &VkDestroyEvent);
			
			vulkanDLL.LoadDynamicFunction("vkCreateGraphicsPipeline", &VkCreateGraphicsPipelines);
			vulkanDLL.LoadDynamicFunction("vkCreateComputePipelines", &VkCreateComputePipelines);
			vulkanDLL.LoadDynamicFunction("vkDestroyPipeline", &VkDestroyPipeline);
			
			vulkanDLL.LoadDynamicFunction("vkAllocateMemory", &VkAllocateMemory);
			vulkanDLL.LoadDynamicFunction("vkFreeMemory", &VkFreeMemory);
			vulkanDLL.LoadDynamicFunction("vkMapMemory", &VkMapMemory);
			vulkanDLL.LoadDynamicFunction("vkUnmapMemory", &VkUnmapMemory);
			
			vulkanDLL.LoadDynamicFunction("vkCreateImageView", &VkCreateImageView);
			vulkanDLL.LoadDynamicFunction("vkDestroyImageView", &VkDestroyImageView);
			
			vulkanDLL.LoadDynamicFunction("vkCreateSampler", &VkCreateSampler);
			vulkanDLL.LoadDynamicFunction("vkDestroySampler", &VkCreateSampler);
			
			vulkanDLL.LoadDynamicFunction("vkCreateQueryPool", &VkCreateQueryPool);
			vulkanDLL.LoadDynamicFunction("vkGetQueryPoolResults", &VkGetQueryPoolResults);
			vulkanDLL.LoadDynamicFunction("vkDestroyQueryPool", &VkDestroyQueryPool);
			
			vulkanDLL.LoadDynamicFunction("vkBeginCommandBuffer", &VkBeginCommandBuffer);
			vulkanDLL.LoadDynamicFunction("vkEndCommandBuffer", &VkEndCommandBuffer);
			
			vulkanDLL.LoadDynamicFunction("vkCmdBeginRenderPass", &VkCmdBeginRenderPass);
			vulkanDLL.LoadDynamicFunction("vkCmdNextSubpass", &VkCmdNextSubpass);
			vulkanDLL.LoadDynamicFunction("vkCmdEndRenderPass", &VkCmdEndRenderPass);
			
			vulkanDLL.LoadDynamicFunction("vkCmdSetScissor", &VkCmdSetScissor);
			vulkanDLL.LoadDynamicFunction("vkCmdSetViewport", &VkCmdSetViewport);
			
			vulkanDLL.LoadDynamicFunction("vkCmdBindPipeline", &VkCmdBindPipeline);
			vulkanDLL.LoadDynamicFunction("vkCmdBindDescriptorSets", &VkCmdBindDescriptorSets);
			vulkanDLL.LoadDynamicFunction("vkCmdPushConstants", &VkCmdPushConstants);
			
			vulkanDLL.LoadDynamicFunction("vkCmdBindVertexBuffers", &VkCmdBindVertexBuffers);
			vulkanDLL.LoadDynamicFunction("vkCmdBindIndexBuffer", &VkCmdBindIndexBuffer);
			
			vulkanDLL.LoadDynamicFunction("vkCmdDrawIndexed", &VkCmdDrawIndexed);
			vulkanDLL.LoadDynamicFunction("vkCmdDispatch", &VkCmdDispatch);
			
			vulkanDLL.LoadDynamicFunction("vkCmdCopyBuffer", &VkCmdCopyBuffer);
			vulkanDLL.LoadDynamicFunction("vkCmdCopyBufferToImage", &VkCmdCopyBufferToImage);
			vulkanDLL.LoadDynamicFunction("vkCmdCopyImage", &VkCmdCopyImage);
			
			vulkanDLL.LoadDynamicFunction("vkCmdPipelineBarrier", &VkCmdPipelineBarrier);
			
			vulkanDLL.LoadDynamicFunction("vkCmdSetEvent", &VkCmdSetEvent);
			vulkanDLL.LoadDynamicFunction("vkCmdResetEvent", &VkCmdResetEvent);
			
			
			for (auto e : createInfo.Extensions) {
				switch (e.First) {
				case Extension::RAY_TRACING: {
					vulkanDLL.LoadDynamicFunction("vkCreateAccelerationStructureKHR", &vkCreateAccelerationStructureKHR);
					vulkanDLL.LoadDynamicFunction("vkDestroyAccelerationStructureKHR", &vkDestroyAccelerationStructureKHR);
					vulkanDLL.LoadDynamicFunction("vkCreateRayTracingPipelinesKHR", &vkCreateRayTracingPipelinesKHR);
					vulkanDLL.LoadDynamicFunction("vkGetAccelerationStructureBuildSizesKHR", &vkGetAccelerationStructureBuildSizesKHR);
					vulkanDLL.LoadDynamicFunction("vkGetRayTracingShaderGroupHandlesKHR", &vkGetRayTracingShaderGroupHandlesKHR);
					vulkanDLL.LoadDynamicFunction("vkBuildAccelerationStructuresKHR", &vkBuildAccelerationStructuresKHR);
					vulkanDLL.LoadDynamicFunction("vkCmdBuildAccelerationStructuresKHR", &vkCmdBuildAccelerationStructuresKHR);
					vulkanDLL.LoadDynamicFunction("vkGetAccelerationStructureDeviceAddressKHR", &vkGetAccelerationStructureDeviceAddressKHR);
					vulkanDLL.LoadDynamicFunction("vkCreateDeferredOperationKHR", &vkCreateDeferredOperationKHR);
					vulkanDLL.LoadDynamicFunction("vkDeferredOperationJoinKHR", &vkDeferredOperationJoinKHR);
					vulkanDLL.LoadDynamicFunction("vkGetDeferredOperationResultKHR", &vkGetDeferredOperationResultKHR);
					vulkanDLL.LoadDynamicFunction("vkGetDeferredOperationMaxConcurrencyKHR", &vkGetDeferredOperationMaxConcurrencyKHR);
					vulkanDLL.LoadDynamicFunction("vkDestroyDeferredOperationKHR", &vkDestroyDeferredOperationKHR);
					vulkanDLL.LoadDynamicFunction("vkCmdCopyAccelerationStructureKHR", &vkCmdCopyAccelerationStructureKHR);
					vulkanDLL.LoadDynamicFunction("vkCmdCopyAccelerationStructureToMemoryKHR", &vkCmdCopyAccelerationStructureToMemoryKHR);
					vulkanDLL.LoadDynamicFunction("vkCmdCopyMemoryToAccelerationStructureKHR", &vkCmdCopyMemoryToAccelerationStructureKHR);
					vulkanDLL.LoadDynamicFunction("vkCmdWriteAccelerationStructuresPropertiesKHR", &vkCmdWriteAccelerationStructuresPropertiesKHR);
					vulkanDLL.LoadDynamicFunction("vkCmdTraceRaysKHR", &vkCmdTraceRaysKHR);
					break;
				}
				default:;
				}
			}

			for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
				memoryTypes[i] = ToGAL(memoryProperties.memoryTypes[i].propertyFlags);
			}

			if constexpr (_DEBUG) {
				vulkanDLL.LoadDynamicFunction("vkSetDebugUtilsObjectNameEXT", &vkSetDebugUtilsObjectNameEXT);
				vulkanDLL.LoadDynamicFunction("vkCmdInsertDebugUtilsLabelEXT", &vkCmdInsertDebugUtilsLabelEXT);
				vulkanDLL.LoadDynamicFunction("vkCmdBeginDebugUtilsLabelEXT", &vkCmdBeginDebugUtilsLabelEXT);
				vulkanDLL.LoadDynamicFunction("vkCmdEndDebugUtilsLabelEXT", &vkCmdEndDebugUtilsLabelEXT);

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

		void Destroy() {
			vulkanDLL.LoadDynamicFunction("vkDeviceWaitIdle").Call<VkResult>(device);
			vulkanDLL.LoadDynamicFunction("vkDestroyDevice").Call<VkResult>(device, GetVkAllocationCallbacks());

#if (_DEBUG)
			if (debug) {
				vulkanDLL.LoadDynamicFunction("vkDestroyDebugUtilsMessengerEXT").Call<VkResult>(instance, debugMessenger, GetVkAllocationCallbacks());
			}
			debugClear(debugMessenger);
#endif

			vulkanDLL.LoadDynamicFunction("vkDestroyInstance").Call<VkResult>(instance, GetVkAllocationCallbacks());

			debugClear(device); debugClear(instance);
		}
		
		~VulkanRenderDevice() = default;

		GPUInfo GetGPUInfo() const {
			GPUInfo result; VkPhysicalDeviceProperties physicalDeviceProperties;

			vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceProperties").Call<VkResult>(physicalDevice, &physicalDeviceProperties);

			result.GPUName = physicalDeviceProperties.deviceName;
			result.DriverVersion = physicalDeviceProperties.driverVersion;
			result.APIVersion = physicalDeviceProperties.apiVersion;
			for (auto e : physicalDeviceProperties.pipelineCacheUUID) {
				result.PipelineCacheUUID[&e - physicalDeviceProperties.pipelineCacheUUID] = e;
			}

			return result;
		}

		[[nodiscard]] uint32_t GetMemoryTypeIndex(MemoryType memoryType) const {			
			for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
				if (memoryType == memoryTypes[i]) {
					return i;
				}
			}

			return 0xFFFFFFFF;
		}

		struct FindSupportedImageFormat
		{
			GTSL::Range<FormatDescriptor*> Candidates;
			TextureUse TextureUses;
			FormatDescriptor FormatDescriptor;
			Tiling TextureTiling;
		};
		[[nodiscard]] FormatDescriptor FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const {
			VkFormatProperties format_properties;

			VkFormatFeatureFlags features{};

			TranslateMask(TextureUses::TRANSFER_SOURCE, VK_FORMAT_FEATURE_TRANSFER_SRC_BIT, findSupportedImageFormat.TextureUses, features);
			TranslateMask(TextureUses::TRANSFER_DESTINATION, VK_FORMAT_FEATURE_TRANSFER_DST_BIT, findSupportedImageFormat.TextureUses, features);
			TranslateMask(TextureUses::SAMPLE, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT, findSupportedImageFormat.TextureUses, features);
			TranslateMask(TextureUses::STORAGE, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT, findSupportedImageFormat.TextureUses, features);
			if(findSupportedImageFormat.TextureUses & TextureUses::ATTACHMENT) {
				switch (findSupportedImageFormat.FormatDescriptor.Type) {
				case TextureType::COLOR: features |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT; break;
				case TextureType::DEPTH: features |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT; break;
				}
			}

			for (auto e : findSupportedImageFormat.Candidates) {
				vulkanDLL.LoadDynamicFunction("vkGetPhysicalDeviceFormatProperties").Call<VkResult>(physicalDevice, ToVulkan(MakeFormatFromFormatDescriptor(e)), &format_properties);

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

			return {};
		}
		
		[[nodiscard]] VkInstance GetVkInstance() const { return instance; }
		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice; }
		[[nodiscard]] VkDevice GetVkDevice() const { return device; }
		
		[[nodiscard]] MemoryType FindNearestMemoryType(MemoryType memoryType) const {
			for (GTSL::uint32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
				if ((ToGAL(memoryProperties.memoryTypes[i].propertyFlags) & memoryType) == memoryType) {
					return ToGAL(memoryProperties.memoryTypes[i].propertyFlags);
				}
			}

			return MemoryType();
		}

		[[nodiscard]] GTSL::uint32 GetUniformBufferBindingOffsetAlignment() const { return static_cast<GTSL::uint32>(uniformBufferMinOffset); }
		[[nodiscard]] GTSL::uint32 GetStorageBufferBindingOffsetAlignment() const { return static_cast<GTSL::uint32>(storageBufferMinOffset); }

		struct MemoryHeap
		{
			GTSL::Byte Size;
			MemoryType HeapType;

			GTSL::Array<MemoryType, 16> MemoryTypes;
		};
		
		GTSL::Array<MemoryHeap, 16> GetMemoryHeaps() const {
			GTSL::Array<MemoryHeap, 16> memoryHeaps;

			for (GTSL::uint8 heapIndex = 0; heapIndex < memoryProperties.memoryHeapCount; ++heapIndex) {
				MemoryHeap memoryHeap;
				memoryHeap.Size = GTSL::Byte(memoryProperties.memoryHeaps[heapIndex].size);
				memoryHeap.HeapType = 0;

				TranslateMask(VK_MEMORY_HEAP_DEVICE_LOCAL_BIT, MemoryTypes::GPU, memoryProperties.memoryHeaps[heapIndex].flags, memoryHeap.HeapType);

				for (GTSL::uint8 memType = 0; memType < memoryProperties.memoryTypeCount; ++memType) {
					if (memoryProperties.memoryTypes[memType].heapIndex == heapIndex) {
						MemoryType memoryType;

						TranslateMask(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, MemoryTypes::GPU, memoryProperties.memoryTypes[memType].propertyFlags, memoryType);
						TranslateMask(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, MemoryTypes::HOST_VISIBLE, memoryProperties.memoryTypes[memType].propertyFlags, memoryType);
						TranslateMask(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, MemoryTypes::HOST_COHERENT, memoryProperties.memoryTypes[memType].propertyFlags, memoryType);
						TranslateMask(VK_MEMORY_PROPERTY_HOST_CACHED_BIT, MemoryTypes::HOST_CACHED, memoryProperties.memoryTypes[memType].propertyFlags, memoryType);

						memoryHeap.MemoryTypes.EmplaceBack(memoryType);
					}
				}

				memoryHeaps.EmplaceBack(memoryHeap);
			}

			return memoryHeaps;
		}
	

		[[nodiscard]] const VkAllocationCallbacks* GetVkAllocationCallbacks() const { return nullptr; }

		GTSL::uint32 GetLinearNonLinearGranularity() const { return linearNonLinearAlignment; }

		GTSL::DLL vulkanDLL;
		
		PFN_vkCmdBeginRenderPass VkCmdBeginRenderPass = nullptr;
		PFN_vkCmdEndRenderPass VkCmdEndRenderPass = nullptr;
		PFN_vkCmdNextSubpass VkCmdNextSubpass;
		PFN_vkCmdDrawIndexed VkCmdDrawIndexed;
		PFN_vkAcquireNextImageKHR VkAcquireNextImage;
		PFN_vkResetCommandPool VkResetCommandPool;
		PFN_vkCreateBuffer VkCreateBuffer; PFN_vkDestroyBuffer VkDestroyBuffer;
		PFN_vkGetBufferMemoryRequirements VkGetBufferMemoryRequirements;
		PFN_vkGetImageMemoryRequirements VkGetImageMemoryRequirements;
		PFN_vkGetBufferDeviceAddress VkGetBufferDeviceAddress;
		PFN_vkCreateImage VkCreateImage; PFN_vkDestroyImage VkDestroyImage;
		PFN_vkCreateImageView VkCreateImageView; PFN_vkDestroyImageView VkDestroyImageView;
		PFN_vkCreateFramebuffer VkCreateFramebuffer; PFN_vkDestroyFramebuffer VkDestroyFramebuffer;
		PFN_vkAllocateMemory VkAllocateMemory; PFN_vkFreeMemory VkFreeMemory;
		PFN_vkMapMemory VkMapMemory; PFN_vkUnmapMemory VkUnmapMemory;
		PFN_vkCreatePipelineCache VkCreatePipelineCache; PFN_vkDestroyPipelineCache VkDestroyPipelineCache;
		PFN_vkMergePipelineCaches VkMergePipelineCaches;
		PFN_vkGetPipelineCacheData VkGetPipelineCacheData;
		PFN_vkCreateShaderModule VkCreateShaderModule; PFN_vkDestroyShaderModule VkDestroyShaderModule;
		PFN_vkCreatePipelineLayout VkCreatePipelineLayout; PFN_vkDestroyPipelineLayout VkDestroyPipelineLayout;
		PFN_vkCreateGraphicsPipelines VkCreateGraphicsPipelines; PFN_vkCreateComputePipelines VkCreateComputePipelines;
		PFN_vkDestroyPipeline VkDestroyPipeline;
		PFN_vkCreateDescriptorPool VkCreateDescriptorPool; PFN_vkDestroyDescriptorPool VkDestroyDescriptorPool;
		PFN_vkCreateDescriptorSetLayout VkCreateDescriptorSetLayout; PFN_vkDestroyDescriptorSetLayout VkDestroyDescriptorSetLayout;
		PFN_vkAllocateDescriptorSets VkAllocateDescriptorSets;
		PFN_vkUpdateDescriptorSets VkUpdateDescriptorSets;
		PFN_vkAllocateCommandBuffers VkAllocateCommandBuffers;
		PFN_vkCreateCommandPool VkCreateCommandPool; PFN_vkDestroyCommandPool VkDestroyCommandPool;
		PFN_vkBeginCommandBuffer VkBeginCommandBuffer; PFN_vkEndCommandBuffer VkEndCommandBuffer;
		PFN_vkCreateSampler VkCreateSampler; PFN_vkDestroySampler VkDestroySampler;
		PFN_vkCreateSwapchainKHR VkCreateSwapchain; PFN_vkDestroySwapchainKHR VkDestroySwapchain;
		PFN_vkGetSwapchainImagesKHR VkGetSwapchainImages;
		PFN_vkCreateRenderPass VkCreateRenderPass; PFN_vkDestroyRenderPass VkDestroyRenderPass;
		PFN_vkBindBufferMemory VkBindBufferMemory;
		PFN_vkBindImageMemory VkBindImageMemory;
		PFN_vkCmdBindPipeline VkCmdBindPipeline;
		PFN_vkCmdDispatch VkCmdDispatch;
		PFN_vkCmdCopyBuffer VkCmdCopyBuffer;
		PFN_vkCmdCopyBufferToImage VkCmdCopyBufferToImage;
		PFN_vkCmdCopyImage VkCmdCopyImage;
		PFN_vkCmdPipelineBarrier VkCmdPipelineBarrier;
		PFN_vkCmdBindDescriptorSets VkCmdBindDescriptorSets;
		PFN_vkCmdPushConstants VkCmdPushConstants;
		PFN_vkCmdBindVertexBuffers VkCmdBindVertexBuffers;
		PFN_vkCmdBindIndexBuffer VkCmdBindIndexBuffer;
		PFN_vkCmdSetScissor VkCmdSetScissor;
		PFN_vkCmdSetViewport VkCmdSetViewport;
		PFN_vkCmdSetEvent VkCmdSetEvent;
		PFN_vkCmdResetEvent VkCmdResetEvent;
		PFN_vkCreateQueryPool VkCreateQueryPool; PFN_vkDestroyQueryPool VkDestroyQueryPool;
		PFN_vkGetQueryPoolResults VkGetQueryPoolResults;
		PFN_vkQueueSubmit VkQueueSubmit;
		PFN_vkQueuePresentKHR VkQueuePresent;
#if (_WIN64)
		PFN_vkCreateWin32SurfaceKHR VkCreateWin32Surface;
#endif
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR VkGetPhysicalDeviceSurfaceCapabilities;
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR VkGetPhysicalDeviceSurfaceFormats;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR VkGetPhysicalDeviceSurfacePresentModes;
		PFN_vkGetPhysicalDeviceSurfaceSupportKHR VkGetPhysicalDeviceSurfaceSupport;
		PFN_vkDestroySurfaceKHR VkDestroySurface;
		PFN_vkCreateFence VkCreateFence; PFN_vkDestroyFence VkDestroyFence;
		PFN_vkWaitForFences VkWaitForFences; PFN_vkResetFences VkResetFences;
		PFN_vkCreateSemaphore VkCreateSemaphore; PFN_vkDestroySemaphore VkDestroySemaphore;
		PFN_vkCreateEvent VkCreateEvent; PFN_vkDestroyEvent VkDestroyEvent;
		PFN_vkSetEvent VkSetEvent; PFN_vkResetEvent VkResetEvent;
		
		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR = nullptr;
		PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = nullptr;
		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = nullptr;
		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR = nullptr;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = nullptr;
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = nullptr;
		PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR = nullptr;
		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR = nullptr;
		PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR = nullptr;
		PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR = nullptr;
		PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR = nullptr;
		PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR = nullptr;
		PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR = nullptr;
		PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR = nullptr;
		PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR = nullptr;
		PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR = nullptr;
		PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR = nullptr;
		PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR = nullptr;
		
#if (_DEBUG)
		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
		PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = nullptr;
		PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
		PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = nullptr;
#endif

	private:
#if (_DEBUG)
		VkDebugUtilsMessengerEXT debugMessenger = nullptr;
		bool debug = false;
#endif

		GTSL::uint16 uniformBufferMinOffset, storageBufferMinOffset, linearNonLinearAlignment;
		
		VkInstance instance = nullptr;
		VkPhysicalDevice physicalDevice = nullptr;
		VkDevice device = nullptr;
		AllocationInfo allocationInfo;
		VkAllocationCallbacks allocationCallbacks;
		VkPhysicalDeviceMemoryProperties memoryProperties;

		MemoryType memoryTypes[16];
		
		friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};

	template<typename T>
	void setName(const VulkanRenderDevice* renderDevice, T handle, const VkObjectType objectType, const GTSL::Range<const GTSL::UTF8*> text) {
		if constexpr (_DEBUG) {
			VkDebugUtilsObjectNameInfoEXT vkDebugUtilsObjectNameInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
			vkDebugUtilsObjectNameInfo.objectHandle = reinterpret_cast<GTSL::uint64>(handle);
			vkDebugUtilsObjectNameInfo.objectType = objectType;
			vkDebugUtilsObjectNameInfo.pObjectName = text.begin();
			renderDevice->vkSetDebugUtilsObjectNameEXT(renderDevice->GetVkDevice(), &vkDebugUtilsObjectNameInfo);
		}
	}
}
