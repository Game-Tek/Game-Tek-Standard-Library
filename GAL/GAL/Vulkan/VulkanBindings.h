#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"

#include <GTSL/Array.hpp>

namespace GAL
{
	class VulkanBindingsPool final : public BindingsPool
	{
	public:
		VulkanBindingsPool(const CreateInfo& createInfo);

		void Destroy(class GAL::RenderDevice* renderDevice);

		void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo);
		void FreePool(const FreeBindingsPoolInfo& freeDescriptorPoolInfo);

		[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return vkDescriptorPool; }

	private:
		VkDescriptorPool vkDescriptorPool = nullptr;

	};

	class VulkanBindingsSet final : public BindingsSet
	{
	public:
		VulkanBindingsSet(const CreateInfo& createInfo);

		void Destroy(class RenderDevice* renderDevice);

		void Update(const BindingsSetUpdateInfo& bindingsUpdateInfo);

		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return descriptorSetLayout; }
		[[nodiscard]] const GTSL::Array<VkDescriptorSet, 4>& GetVkDescriptorSets() const { return descriptorSets; }

	private:
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
		GTSL::Array<VkDescriptorSet, 4> descriptorSets;

	};
}