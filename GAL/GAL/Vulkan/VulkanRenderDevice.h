#pragma once

#include "GAL/RenderDevice.h"

#include <GAL/ext/vulkan/vulkan.h>

namespace GAL
{
	class VulkanQueue final : public Queue
	{
	public:
		VulkanQueue() = default;
		~VulkanQueue() = default;

		void Wait() const;
		
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

		struct CreateInfo
		{
			GTSL::Ranger<const GTSL::UTF8> ApplicationName;
			GTSL::uint16 ApplicationVersion[3];
			GTSL::Ranger<const Queue::CreateInfo> QueueCreateInfos;
			GTSL::Ranger<VulkanQueue> Queues;
			GTSL::Delegate<void(const char*, MessageSeverity)> DebugPrintFunction;
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

		void GetBufferMemoryRequirements(const class VulkanBuffer* buffer, BufferMemoryRequirements& bufferMemoryRequirements) const;
		void GetImageMemoryRequirements(const class VulkanImage* image, ImageMemoryRequirements& imageMemoryRequirements) const;
		
		[[nodiscard]] GTSL::uint32 FindMemoryType(GTSL::uint32 typeFilter, GTSL::uint32 memoryType) const;

		[[nodiscard]] const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return deviceProperties; }

		[[nodiscard]] VkAllocationCallbacks* GetVkAllocationCallbacks() const { return nullptr; }

	private:
#if (_DEBUG)
		PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsFunction = nullptr;
		VkDebugUtilsMessengerEXT debugMessenger = nullptr;
		PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsFunction = nullptr;
#endif

		VkInstance instance = nullptr;
		VkPhysicalDevice physicalDevice = nullptr;
		VkDevice device = nullptr;

		VkAllocationCallbacks allocationCallbacks;

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceMemoryProperties memoryProperties;

		friend VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};
}