#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"

namespace GAL
{
	class VulkanBindingsPool final : public BindingsPool
	{
	public:
		struct DescriptorPoolSize
		{
			VulkanBindingType BindingType;
			GTSL::uint32 Count = 0;
		};
		
		struct CreateInfo : RenderInfo
		{
			GTSL::Ranger<const DescriptorPoolSize> DescriptorPoolSizes;
			GTSL::Ranger<class VulkanBindingsSet> BindingsSets;
		};
		VulkanBindingsPool(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo);

		[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return descriptorPool; }

	private:
		VkDescriptorPool descriptorPool = nullptr;
	};

	class VulkanBindingsSetLayout final
	{
	public:
		struct BindingDescriptor
		{
			VulkanBindingType BindingType = VulkanBindingType::UNIFORM_BUFFER;
			VulkanShaderStage::value_type ShaderStage = VulkanShaderStage::ALL;
			GTSL::uint8 UniformCount = 0;
		};

		struct ImageBindingDescriptor : BindingDescriptor
		{
			GTSL::Ranger<const class VulkanImageView> ImageViews;
			GTSL::Ranger<const class VulkanSampler> Samplers;
			GTSL::Ranger<const ImageLayout> Layouts;
		};

		struct BufferBindingDescriptor : BindingDescriptor
		{
			GTSL::Ranger<const class VulkanBuffer> Buffers;
			GTSL::Ranger<const GTSL::uint32> Offsets;
			GTSL::Ranger<const GTSL::uint32> Sizes;
		};

		VulkanBindingsSetLayout() = default;
		
		struct CreateInfo : VulkanRenderInfo
		{
			GTSL::Ranger<const BindingDescriptor> BindingsDescriptors;
		};
		VulkanBindingsSetLayout(const CreateInfo& createInfo);
		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return descriptorSetLayout; }
		
	private:
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
	};
	
	class VulkanBindingsSet final : public BindingsSet
	{
	public:
		VulkanBindingsSet() = default;

		struct BindingsSetUpdateInfo : RenderInfo
		{
			GTSL::Array<VulkanBindingsSetLayout::ImageBindingDescriptor, MAX_BINDINGS_PER_SET> ImageBindingsSetLayout;
			GTSL::Array<VulkanBindingsSetLayout::BufferBindingDescriptor, MAX_BINDINGS_PER_SET> BufferBindingsSetLayout;
		};
		void Update(const BindingsSetUpdateInfo& bindingsUpdateInfo);

		[[nodiscard]] VkDescriptorSet GetVkDescriptorSets() const { return descriptorSet; }

	private:
		VkDescriptorSet descriptorSet{ nullptr };

		friend VulkanBindingsPool;
	};
}