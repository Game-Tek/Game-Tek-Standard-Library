#pragma once

#include "GAL/RenderDevice.h"

#include "Vulkan.h"
#include <GAL/ext/vulkan/vulkan_beta.h>

#include <GTSL/Pair.h>

namespace GAL
{
	class VulkanFence;
	class VulkanSemaphore;
	class VulkanCommandBuffer;

	class VulkanQueue final : public Queue
	{
	public:
		VulkanQueue() = default;
		~VulkanQueue() = default;

		void Wait(const class VulkanRenderDevice* renderDevice) const;

		struct SubmitInfo final : VulkanRenderInfo
		{
			GTSL::Range<const VulkanCommandBuffer*> CommandBuffers;
			GTSL::Range<const VulkanSemaphore*> SignalSemaphores;
			GTSL::Range<const VulkanSemaphore*> WaitSemaphores;
			GTSL::Range<const GTSL::uint64*> SignalValues;
			GTSL::Range<const GTSL::uint64*> WaitValues;
			/**
			 * \brief Pipeline stages at which each corresponding semaphore wait will occur.
			 */
			GTSL::Range<const GTSL::uint32*> WaitPipelineStages;
			const VulkanFence* Fence{ nullptr };
		};
		void Submit(const SubmitInfo& submitInfo);

		[[nodiscard]] VkQueue GetVkQueue() const { return queue; }
		[[nodiscard]] GTSL::uint32 GetQueueIndex() const { return queueIndex; }
		[[nodiscard]] GTSL::uint32 GetFamilyIndex() const { return familyIndex; }

	private:
		VkQueue queue = nullptr;
		GTSL::uint32 queueIndex = 0;
		GTSL::uint32 familyIndex = 0;

		friend class VulkanRenderDevice;
	};
	
	class VulkanRenderDevice final : public RenderDevice
	{
	public:
		VulkanRenderDevice() = default;

		struct RayTracingCapabilities
		{
			GTSL::uint32 RecursionDepth = 0, ShaderGroupAlignment = 0, ShaderGroupBaseAlignment = 0, ShaderGroupHandleSize = 0, ScratchBuildOffsetAlignment = 0;
			bool CanBuildOnHost = false;
		};

		struct CreateInfo
		{
			GTSL::Range<const GTSL::UTF8*> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Range<const Queue::CreateInfo*> QueueCreateInfos;
			GTSL::Range<VulkanQueue**> Queues;
			GTSL::Delegate<void(const char*, MessageSeverity)> DebugPrintFunction;
			bool Debug = false;
			bool PerformanceValidation = false;
			GTSL::Range<const GTSL::Pair<Extension, void*>*> Extensions;
			AllocationInfo AllocationInfo;
		};
		explicit VulkanRenderDevice(const CreateInfo& createInfo);
		
		~VulkanRenderDevice();

		GPUInfo GetGPUInfo();

		struct FindSupportedImageFormat
		{
			GTSL::Range<VulkanTextureFormat*> Candidates;
			VulkanTextureUses::value_type TextureUses;
			VulkanTextureTiling TextureTiling;
		};
		[[nodiscard]] VulkanTextureFormat FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const;
		
		[[nodiscard]] VkInstance GetVkInstance() const { return instance; }
		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice; }
		[[nodiscard]] VkDevice GetVkDevice() const { return device; }
		
		[[nodiscard]] GTSL::uint32 FindMemoryType(GTSL::uint32 typeFilter, GTSL::uint32 memoryType) const;

		[[nodiscard]] GTSL::uint32 GetuniformBufferBindingOffsetAlignment() const { return static_cast<GTSL::uint32>(deviceProperties.limits.minUniformBufferOffsetAlignment); }
		[[nodiscard]] GTSL::uint32 GetStorageBufferBindingOffsetAlignment() const { return static_cast<GTSL::uint32>(deviceProperties.limits.minStorageBufferOffsetAlignment); }
		
		[[nodiscard]] const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return deviceProperties; }

		[[nodiscard]] const VkAllocationCallbacks* GetVkAllocationCallbacks() const { return nullptr; }

		GTSL::uint32 GetLinearNonLinearGranularity() const { return deviceProperties.limits.bufferImageGranularity; }
		
		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
		PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
		
		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
		
		PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;

		PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
		PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
		PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
		PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
		PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;

		PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
		PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
		PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
		PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
		PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
		
#if (_DEBUG)
		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
		PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
		PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
		PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
#endif

	private:
#if (_DEBUG)
		PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsFunction;
		VkDebugUtilsMessengerEXT debugMessenger;
		PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsFunction;

#endif

		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;

		AllocationInfo allocationInfo;
		
		VkAllocationCallbacks allocationCallbacks;

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceMemoryProperties memoryProperties;
		
		friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};
}