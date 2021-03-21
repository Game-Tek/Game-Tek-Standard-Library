#pragma once

#include "GAL/RenderDevice.h"

#include "Vulkan.h"
#include <GAL/ext/vulkan/vulkan_beta.h>

#include <GTSL/Pair.h>


#include "VulkanSynchronization.h"
#include "GTSL/Array.hpp"
#include "GTSL/DataSizes.h"

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
			VulkanFence Fence;
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
			bool SynchronizationValidation = false;
			GTSL::Range<const GTSL::Pair<Extension, void*>*> Extensions;
			AllocationInfo AllocationInfo;
		};
		VulkanRenderDevice() = default;

		[[no_discard]] bool Initialize(const CreateInfo& createInfo);
		
		~VulkanRenderDevice();

		GPUInfo GetGPUInfo();

		struct FindSupportedImageFormat
		{
			GTSL::Range<VulkanTextureFormat*> Candidates;
			VulkanTextureUses TextureUses;
			VulkanTextureTiling TextureTiling;
		};
		[[nodiscard]] VulkanTextureFormat FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const;
		
		[[nodiscard]] VkInstance GetVkInstance() const { return instance; }
		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice; }
		[[nodiscard]] VkDevice GetVkDevice() const { return device; }
		
		[[nodiscard]] GTSL::uint32 FindMemoryType(GTSL::uint32 typeFilter, GTSL::uint32 memoryType) const;

		[[nodiscard]] GTSL::uint32 GetUniformBufferBindingOffsetAlignment() const { return static_cast<GTSL::uint32>(uniformBufferMinOffset); }
		[[nodiscard]] GTSL::uint32 GetStorageBufferBindingOffsetAlignment() const { return static_cast<GTSL::uint32>(storageBufferMinOffset); }

		struct MemoryHeap
		{
			GTSL::Byte Size;
			MemoryTypes HeapType;

			GTSL::Array<MemoryTypes, 16> MemoryTypes;
		};
		
		GTSL::Array<MemoryHeap, 16> GetMemoryHeaps() const;
	

		[[nodiscard]] const VkAllocationCallbacks* GetVkAllocationCallbacks() const { return nullptr; }

		GTSL::uint32 GetLinearNonLinearGranularity() const { return linearNonLinearAlignment; }
		
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

		
		friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};
}
