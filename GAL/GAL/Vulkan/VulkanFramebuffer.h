#pragma once

#include "GAL/Framebuffer.h"

#include "vulkan/vulkan.h"

#include <GTSL/Vector.hpp>

class VulkanFramebuffer final : public GAL::Framebuffer
{
	GTSL::Vector<VkClearValue> clearValues;
	VkFramebuffer framebuffer = nullptr;

public:
	VulkanFramebuffer(class VulkanRenderDevice* vulkanRenderDevice, const GAL::FramebufferCreateInfo& framebufferCreateInfo);
	~VulkanFramebuffer() = default;

	void Destroy(GAL::RenderDevice* renderDevice) override;

	[[nodiscard]] VkFramebuffer GetVkFramebuffer() const { return framebuffer; }
	[[nodiscard]] const GTSL::Vector<VkClearValue>& GetClearValues() const { return clearValues; }
};
