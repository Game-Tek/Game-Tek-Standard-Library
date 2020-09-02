#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace GAL
{
	class VulkanBindingsPool final : public BindingsPool
	{
	public:
		VulkanBindingsPool() = default;
		struct DescriptorPoolSize
		{
			VulkanBindingType BindingType;
			GTSL::uint32 Count = 0;
		};
		
		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::Ranger<const DescriptorPoolSize> DescriptorPoolSizes;
			GTSL::uint32 MaxSets = 0;
		};
		VulkanBindingsPool(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		struct AllocateBindingsSetsInfo : VulkanRenderInfo
		{
			GTSL::Ranger<const VulkanCreateInfo> BindingsSetCreateInfos;
			GTSL::Ranger<class VulkanBindingsSet> BindingsSets;
			GTSL::Ranger<const class VulkanBindingsSetLayout> BindingsSetLayouts;
			GTSL::Ranger<const GTSL::uint32> BindingsSetDynamicBindingsCounts;
		};
		void AllocateBindingsSets(const AllocateBindingsSetsInfo& allocateBindingsSetsInfo);
		
		struct FreeBindingsSetInfo : VulkanRenderInfo
		{
			GTSL::Ranger<class VulkanBindingsSet> BindingsSet;
		};
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
			GTSL::uint32 UniformCount = 0;
			VulkanBindingFlags::value_type Flags;
		};

		struct ImageBindingDescriptor : BindingDescriptor
		{
			GTSL::Ranger<const class VulkanTextureView> ImageViews;
			GTSL::Ranger<const class VulkanSampler> Samplers;
			GTSL::Ranger<const VulkanTextureLayout> Layouts;
		};

		struct BufferBindingDescriptor : BindingDescriptor
		{
			GTSL::Ranger<const class VulkanBuffer> Buffers;
			GTSL::Ranger<const GTSL::uint32> Offsets;
			GTSL::Ranger<const GTSL::uint32> Sizes;
		};

		VulkanBindingsSetLayout() = default;
		
		struct CreateInfo : VulkanCreateInfo
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

		struct TextureBindingsUpdateInfo
		{
			VulkanSampler Sampler;
			VulkanTextureView TextureView;
			VulkanTextureLayout TextureLayout;
		};

		struct BufferBindingsUpdateInfo
		{
			VulkanBuffer Buffer;
			GTSL::uint64 Offset, Range;
		};

		struct BindingUpdateInfo
		{
			VulkanBindingType Type;

			GTSL::uint32 ArrayElement = 0;
			GTSL::uint32 Count;
			
			void* BindingsUpdates;
		};
		
		struct BindingsSetUpdateInfo final : VulkanRenderInfo
		{
			GTSL::Ranger<const BindingUpdateInfo> BindingUpdateInfos;
		};
		void Update(const BindingsSetUpdateInfo& bindingsUpdateInfo);

		[[nodiscard]] VkDescriptorSet GetVkDescriptorSet() const { return descriptorSet; }

	private:
		VkDescriptorSet descriptorSet{ nullptr };

		friend VulkanBindingsPool;
	};
}
