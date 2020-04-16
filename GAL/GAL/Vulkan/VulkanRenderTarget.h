#pragma once

#include "GAL/RenderTarget.h"

#include "GAL/Vulkan/Vulkan.h"

class VulkanRenderDevice;

class VulkanRenderTargetBase : public GAL::RenderTarget
{
protected:
	VkImageView imageView = nullptr;
public:
	VulkanRenderTargetBase(const RenderTargetCreateInfo& imageCreateInfo);
	[[nodiscard]] virtual const VkImageView& GetVkImageView() const { return imageView; }
};

class VulkanRenderTarget final : public VulkanRenderTargetBase
{
	VkImage image = nullptr;
	VkDeviceMemory imageMemory = nullptr;

public:
	VulkanRenderTarget(VulkanRenderDevice* device, const RenderTargetCreateInfo& imageCreateInfo);

	[[nodiscard]] VkImage GetVkImage() const { return image; }
};
