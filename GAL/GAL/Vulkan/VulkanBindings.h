#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"

#include <GTSL/Array.hpp>

class VulkanBindingsPool final : public GAL::BindingsPool
{
	VkDescriptorPool vkDescriptorPool = nullptr;

public:
	VulkanBindingsPool(class VulkanRenderDevice* device, const GAL::BindingsPoolCreateInfo& descriptorPoolCreateInfo);

	void Destroy(class GAL::RenderDevice* renderDevice) override;

	void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo) override;
	void FreePool(const FreeBindingsPoolInfo& freeDescriptorPoolInfo) override;

	[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return vkDescriptorPool; }
};

class VulkanBindingsSet final : public GAL::BindingsSet
{
	VkDescriptorSetLayout vkDescriptorSetLayout = nullptr;
	GTSL::Array<VkDescriptorSet, 4> vkDescriptorSets;

public:
	VulkanBindingsSet(class VulkanRenderDevice* device, const GAL::BindingsSetCreateInfo& descriptorSetCreateInfo);

	void Destroy(class GAL::RenderDevice* renderDevice) override;

	void Update(const GAL::BindingsSetUpdateInfo& uniformLayoutUpdateInfo) override;

	[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return vkDescriptorSetLayout; }
	[[nodiscard]] const GTSL::Array<VkDescriptorSet, 4>& GetVkDescriptorSets() const { return vkDescriptorSets; }
};
