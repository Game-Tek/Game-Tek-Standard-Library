#pragma once

#include "GAL/RenderDevice.h"

#include "VulkanFramebuffer.h"

namespace GAL
{
	class VulkanRenderDevice final : public RenderDevice
	{
	public:
		class VulkanQueue : public Queue
		{
		public:
			VulkanQueue(const CreateInfo& queueCreateInfo);
			~VulkanQueue() = default;

			void Dispatch(const DispatchInfo& dispatchInfo);

			[[nodiscard]] VkQueue GetVkQueue() const { return queue; }
			[[nodiscard]] GTSL::uint32 GetQueueIndex() const { return queueIndex; }
			[[nodiscard]] GTSL::uint32 GetFamilyIndex() const { return familyIndex; }

		private:
			VkQueue queue = nullptr;
			GTSL::uint32 queueIndex = 0;
			GTSL::uint32 familyIndex = 0;

			friend VulkanRenderDevice;
		};

		explicit VulkanRenderDevice(const CreateInfo& createInfo);
		~VulkanRenderDevice();

		GPUInfo GetGPUInfo();

		struct FindSupportedImageFormat
		{
			GTSL::Ranger<ImageFormat> Candidates;
			ImageUse ImageUse;
			ImageTiling ImageTiling;
		};
		[[nodiscard]] ImageFormat FindNearestSupportedImageFormat(const FindSupportedImageFormat& findSupportedImageFormat) const;
		
		[[nodiscard]] VkInstance GetVkInstance() const { return instance; }
		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice; }
		[[nodiscard]] VkDevice GetVkDevice() const { return device; }

		[[nodiscard]] GTSL::uint32 FindMemoryType(GTSL::uint32 memoryType, GTSL::uint32 memoryFlags) const;
		[[nodiscard]] VkFormat FindSupportedVkFormat(GTSL::Ranger<VkFormat> formats, VkFormatFeatureFlags formatFeatureFlags, VkImageTiling imageTiling) const;

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

	};
}