#pragma once

#include "GAL/RenderDevice.h"

#include "VulkanFramebuffer.h"

#include <GTSL/FixedVector.hpp>

class VulkanRenderDevice final : public GAL::RenderDevice
{
public:
	class VulkanQueue : public GAL::Queue
	{
		VkQueue queue = nullptr;
		GTSL::uint32 queueIndex = 0;
		GTSL::uint32 familyIndex = 0;

	public:
		struct VulkanQueueCreateInfo
		{
			VkQueue Queue = nullptr;
			GTSL::uint32 QueueIndex = 0;
			GTSL::uint32 FamilyIndex = 0;
		};
		VulkanQueue(const QueueCreateInfo& queueCreateInfo, const VulkanQueueCreateInfo& vulkanQueueCreateInfo);
		~VulkanQueue() = default;

		void Dispatch(const DispatchInfo& dispatchInfo) override;

		[[nodiscard]] VkQueue GetVkQueue() const { return queue; }
		[[nodiscard]] GTSL::uint32 GetQueueIndex() const { return queueIndex; }
	};

private:
#if (_DEBUG)
	PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsFunction = nullptr;
	VkDebugUtilsMessengerEXT debugMessenger = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsFunction = nullptr;
#endif

	VkInstance instance = nullptr;
	VkPhysicalDevice physicalDevice = nullptr;
	VkDevice device = nullptr;

	GTSL::Vector<VulkanQueue> vulkanQueues;

	VkAllocationCallbacks allocationCallbacks;

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceMemoryProperties memoryProperties;

public:
	VulkanRenderDevice(const RenderDeviceCreateInfo& renderDeviceCreateInfo);
	~VulkanRenderDevice();

	static bool IsVulkanSupported();

	GAL::GPUInfo GetGPUInfo() override;

	GAL::RenderMesh* CreateRenderMesh(const GAL::RenderMesh::RenderMeshCreateInfo& renderMeshCreateInfo) override;
	GAL::UniformBuffer* CreateUniformBuffer(const GAL::UniformBufferCreateInfo& uniformBufferCreateInfo) override;
	GAL::RenderTarget* CreateRenderTarget(const GAL::RenderTarget::RenderTargetCreateInfo& _ICI) override;
	GAL::Texture* CreateTexture(const GAL::TextureCreateInfo& textureCreateInfo) override;
	GAL::BindingsPool* CreateBindingsPool(const GAL::BindingsPoolCreateInfo& bindingsPoolCreateInfo) override;
	GAL::BindingsSet* CreateBindingsSet(const GAL::BindingsSetCreateInfo& bindingsSetCreateInfo) override;
	GAL::GraphicsPipeline* CreateGraphicsPipeline(const GAL::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) override;
	GAL::RenderPass* CreateRenderPass(const GAL::RenderPassCreateInfo& renderPasCreateInfo) override;
	GAL::ComputePipeline* CreateComputePipeline(const GAL::ComputePipelineCreateInfo& computePipelineCreateInfo) override;
	GAL::Framebuffer* CreateFramebuffer(const GAL::FramebufferCreateInfo& frameBufferCreateInfo) override;
	GAL::RenderContext* CreateRenderContext(const GAL::RenderContextCreateInfo& renderContextCreateInfo) override;
	GAL::CommandBuffer* CreateCommandBuffer(const GAL::CommandBuffer::CommandBufferCreateInfo& commandBufferCreateInfo) override;

	[[nodiscard]] VkInstance GetVkInstance() const { return instance; }
	[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice; }
	[[nodiscard]] VkDevice GetVkDevice() const { return device; }

	[[nodiscard]] GTSL::uint32 FindMemoryType(GTSL::uint32 memoryType, GTSL::uint32 memoryFlags) const;
	VkFormat FindSupportedFormat(const GTSL::FixedVector<VkFormat>& formats, VkFormatFeatureFlags formatFeatureFlags, VkImageTiling imageTiling) const;

	[[nodiscard]] const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return deviceProperties; }
	void AllocateMemory(VkMemoryRequirements* memoryRequirements, GTSL::uint32 memoryPropertyFlag, VkDeviceMemory* deviceMemory) const;

	void AllocateAndBindBuffer();
	void AllocateAndBindImage();

	[[nodiscard]] VkAllocationCallbacks* GetVkAllocationCallbacks() const { return nullptr; }
};
