#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanBindingsPool final : public BindingsPool
	{
	public:
		struct BindingDescriptor
		{
			VulkanBindingType BindingType = VulkanBindingType::UNIFORM_BUFFER;
			VulkanShaderStage::value_type ShaderStage = VulkanShaderStage::ALL;
			GTSL::uint8 MaxNumberOfBindingsAllocatable{ 0 };
		};

		struct ImageBindingDescriptor : BindingDescriptor
		{
			GTSL::Ranger<const class VulkanImageView> ImageViews;
			GTSL::Ranger<const class VulkanSampler> Samplers;
			GTSL::Ranger<ImageLayout> Layouts;
		};

		struct BufferBindingDescriptor : BindingDescriptor
		{
			GTSL::Ranger<const class VulkanBuffer> Buffers;
			GTSL::Ranger<GTSL::uint32> Offsets;
			GTSL::Ranger<GTSL::uint32> Sizes;
		};
		
		struct CreateInfo : RenderInfo
		{
			GTSL::Ranger<BindingDescriptor> BindingsDescriptors;
			GTSL::Ranger<class VulkanBindingsSet> BindingsSets;
		};
		VulkanBindingsPool(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo);

		[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return descriptorPool; }
		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		VkDescriptorPool descriptorPool = nullptr;
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
	};

	class VulkanBindingsSet final : public BindingsSet
	{
	public:
		VulkanBindingsSet() = default;

		struct BindingsSetUpdateInfo : RenderInfo
		{
			GTSL::Array<VulkanBindingsPool::ImageBindingDescriptor, MAX_BINDINGS_PER_SET> ImageBindingsSetLayout;
			GTSL::Array<VulkanBindingsPool::BufferBindingDescriptor, MAX_BINDINGS_PER_SET> BufferBindingsSetLayout;
		};
		void Update(const BindingsSetUpdateInfo& bindingsUpdateInfo);

		[[nodiscard]] VkDescriptorSet GetVkDescriptorSets() const { return descriptorSet; }

	private:
		VkDescriptorSet descriptorSet{ nullptr };

		friend VulkanBindingsPool;
	};
}