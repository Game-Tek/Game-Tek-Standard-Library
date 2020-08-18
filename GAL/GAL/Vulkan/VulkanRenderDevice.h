#pragma once

#include "GAL/RenderDevice.h"

#include "Vulkan.h"
#include <GAL/ext/vulkan/vulkan_beta.h>

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

		void Wait() const;

		struct SubmitInfo final : VulkanRenderInfo
		{
			GTSL::Ranger<const VulkanCommandBuffer> CommandBuffers;
			GTSL::Ranger<const VulkanSemaphore> SignalSemaphores;
			GTSL::Ranger<const VulkanSemaphore> WaitSemaphores;
			GTSL::Ranger<const GTSL::uint64> SignalValues;
			GTSL::Ranger<const GTSL::uint64> WaitValues;
			/**
			 * \brief Pipeline stages at which each corresponding semaphore wait will occur.
			 */
			GTSL::Ranger<const GTSL::uint32> WaitPipelineStages;
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

		enum class Extension
		{
			RAY_TRACING
		};
		
		struct AllocationInfo
		{
			/**
			 * \brief void* UserData. uint64 Size. uint64 Alignment.
			 */
			GTSL::Delegate<void*(void*, GTSL::uint64, GTSL::uint64)> Allocate;
			/**
			 * \brief void* UserData. void* Original Allocation. uint64 Size. uint64 Alignment.
			 */
			GTSL::Delegate<void*(void*, void*, GTSL::uint64, GTSL::uint64)> Reallocate;
			/**
			 * \brief void* UserData. void* Allocation.
			 */
			GTSL::Delegate<void(void*, void*)> Deallocate;

			/**
			 * \brief void* UserData. uint64 Size. uint64 Alignment.
			 */
			GTSL::Delegate<void(void*, GTSL::uint64, GTSL::uint64)> InternalAllocate;
			/**
			* \brief void* UserData. void* Allocation.
			*/
			GTSL::Delegate<void(void*, void*)> InternalDeallocate;

			void* UserData;
		};

		struct RayTracingFeatures
		{
			GTSL::uint32 RecursionDepth = 0;
		};

		struct RayTracingCapabilities
		{
			GTSL::uint32 RecursionDepth = 0;
		};
		
		struct CreateInfo
		{
			GTSL::Ranger<const GTSL::UTF8> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Ranger<const Queue::CreateInfo> QueueCreateInfos;
			GTSL::Ranger<VulkanQueue> Queues;
			GTSL::Delegate<void(const char*, MessageSeverity)> DebugPrintFunction;
			GTSL::Ranger<const Extension> Extensions;
			//GTSL::Ranger<const void*> ExtensionFeatures;
			//GTSL::Ranger<const void*> ExtensionCapabilities;
			AllocationInfo AllocationInfo;
		};
		explicit VulkanRenderDevice(const CreateInfo& createInfo);
		
		~VulkanRenderDevice();

		GPUInfo GetGPUInfo();

		struct FindSupportedImageFormat
		{
			GTSL::Ranger<GTSL::uint32> Candidates;
			GTSL::uint32 ImageUse{ 0 };
			GTSL::uint32 ImageTiling{ 0 };
		};
		[[nodiscard]] GTSL::uint32 FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const;
		
		[[nodiscard]] VkInstance GetVkInstance() const { return instance; }
		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice; }
		[[nodiscard]] VkDevice GetVkDevice() const { return device; }

		void GetBufferMemoryRequirements(const class VulkanBuffer* buffer, MemoryRequirements& bufferMemoryRequirements) const;
		void GetImageMemoryRequirements(const class VulkanImage* image, MemoryRequirements& imageMemoryRequirements) const;

		struct GetAccelerationStructureMemoryRequirementsInfo
		{
			VulkanAccelerationStructureBuildType AccelerationStructureBuildType;
			VulkanAccelerationStructureMemoryRequirementsType AccelerationStructureMemoryRequirementsType;
			const class VulkanAccelerationStructure* AccelerationStructure;
			MemoryRequirements* MemoryRequirements;
		};
		void GetAccelerationStructureMemoryRequirements(const GetAccelerationStructureMemoryRequirementsInfo& accelerationStructureMemoryRequirementsInfo) const;
		
		[[nodiscard]] GTSL::uint32 FindMemoryType(GTSL::uint32 typeFilter, GTSL::uint32 memoryType) const;

		[[nodiscard]] GTSL::uint32 GetMinUniformBufferOffset() const { return static_cast<GTSL::uint32>(deviceProperties.limits.minUniformBufferOffsetAlignment); }
		
		[[nodiscard]] const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return deviceProperties; }

		[[nodiscard]] VkAllocationCallbacks* GetVkAllocationCallbacks() const { return nullptr; }

		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
		PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;

		PFN_vkBuildAccelerationStructureKHR VkBuildAccelerationStructureKHR;
		
		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
		PFN_vkBindAccelerationStructureMemoryKHR vkBindAccelerationStructureMemoryKHR;

		PFN_vkGetAccelerationStructureMemoryRequirementsKHR vkGetAccelerationStructureMemoryRequirementsKHR;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
		
		PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
		PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;

		PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
		PFN_vkCmdBuildAccelerationStructureKHR vkCmdBuildAccelerationStructureKHR;
		PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
		PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
		PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
		PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
		
#if (_DEBUG)
		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
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