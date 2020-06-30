#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanBindingsPool final : public BindingsPool
	{
	public:
		VulkanBindingsPool(const CreateInfo& createInfo);

		void Destroy(class RenderDevice* renderDevice);

		void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo);

		[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return vkDescriptorPool; }
		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		VkDescriptorPool vkDescriptorPool = nullptr;
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
	};

	class VulkanBindingsSet final : public BindingsSet
	{
	public:
		VulkanBindingsSet() = default;

		void Update(const BindingsSetUpdateInfo& bindingsUpdateInfo);

		[[nodiscard]] VkDescriptorSet GetVkDescriptorSets() const { return descriptorSet; }

	private:
		VkDescriptorSet descriptorSet{ nullptr };

		friend VulkanBindingsPool;
	};
}