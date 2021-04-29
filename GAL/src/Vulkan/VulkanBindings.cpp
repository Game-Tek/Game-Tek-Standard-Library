#include "GAL/Vulkan/VulkanBindings.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanBindingsPool::VulkanBindingsPool(const CreateInfo& createInfo)
{
	GTSL::Array<VkDescriptorPoolSize, MAX_BINDINGS_PER_SET> vkDescriptorPoolSizes;

	for(GTSL::uint8 i = 0; i < createInfo.BindingsPoolSizes.ElementCount(); ++i)
	{
		auto& descriptorPoolSize = vkDescriptorPoolSizes.EmplaceBack();
		descriptorPoolSize.type = static_cast<VkDescriptorType>(createInfo.BindingsPoolSizes[i].BindingType);
		//Max number of descriptors of VkDescriptorPoolSize::type we can allocate.
		descriptorPoolSize.descriptorCount = createInfo.BindingsPoolSizes[&descriptorPoolSize - vkDescriptorPoolSizes.begin()].Count;
	}

	VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	//Is the total number of sets that can be allocated from the pool.
	vkDescriptorPoolCreateInfo.maxSets = createInfo.MaxSets;
	vkDescriptorPoolCreateInfo.poolSizeCount = vkDescriptorPoolSizes.GetLength();
	vkDescriptorPoolCreateInfo.pPoolSizes = vkDescriptorPoolSizes.begin();
	VK_CHECK(vkCreateDescriptorPool(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vkDescriptorPoolCreateInfo, static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &descriptorPool));
	SET_NAME(descriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, createInfo);
}

void GAL::VulkanBindingsPool::Destroy(const VulkanRenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<const VulkanRenderDevice*>(renderDevice);
	vkDestroyDescriptorPool(vk_render_device->GetVkDevice(), descriptorPool, vk_render_device->GetVkAllocationCallbacks());
	debugClear(descriptorPool);
}

void GAL::VulkanBindingsPool::AllocateBindingsSets(const AllocateBindingsSetsInfo& allocateBindingsSetsInfo)
{	
	//VkDescriptorSetVariableDescriptorCountAllocateInfo vkDescriptorSetVariableDescriptorCountAllocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO };
	//vkDescriptorSetVariableDescriptorCountAllocateInfo.descriptorSetCount = static_cast<GTSL::uint32>(allocateBindingsSetsInfo.BindingsSetDynamicBindingsCounts.ElementCount());
	//vkDescriptorSetVariableDescriptorCountAllocateInfo.pDescriptorCounts = allocateBindingsSetsInfo.BindingsSetDynamicBindingsCounts.begin();

	GTSL_ASSERT(allocateBindingsSetsInfo.BindingsSets.ElementCount() < 32, "");
	VkDescriptorSet descriptorSets[32];
	
	VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	//vkDescriptorSetAllocateInfo.pNext = &vkDescriptorSetVariableDescriptorCountAllocateInfo;
	vkDescriptorSetAllocateInfo.descriptorPool = descriptorPool;
	vkDescriptorSetAllocateInfo.descriptorSetCount = allocateBindingsSetsInfo.BindingsSets.ElementCount();
	vkDescriptorSetAllocateInfo.pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout*>(allocateBindingsSetsInfo.BindingsSetLayouts.begin());
	VK_CHECK(vkAllocateDescriptorSets(allocateBindingsSetsInfo.RenderDevice->GetVkDevice(), &vkDescriptorSetAllocateInfo, descriptorSets));

	for(GTSL::uint32 i = 0; i < allocateBindingsSetsInfo.BindingsSets.ElementCount(); ++i) {
		allocateBindingsSetsInfo.BindingsSets[i]->descriptorSet = descriptorSets[i];
	}
	
	if constexpr (_DEBUG) {
		for(GTSL::uint32 i = 0; i < allocateBindingsSetsInfo.BindingsSetCreateInfos.ElementCount(); ++i)
		{
			SET_NAME(allocateBindingsSetsInfo.BindingsSets[i]->GetVkDescriptorSet(), VK_OBJECT_TYPE_DESCRIPTOR_SET, allocateBindingsSetsInfo.BindingsSetCreateInfos[i]);
		}
	}
}

void GAL::VulkanBindingsPool::FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo)
{
	vkFreeDescriptorSets(static_cast<const VulkanRenderDevice*>(freeBindingsSetInfo.RenderDevice)->GetVkDevice(), descriptorPool,
	static_cast<GTSL::uint32>(freeBindingsSetInfo.BindingsSet.ElementCount()), reinterpret_cast<VkDescriptorSet*>(freeBindingsSetInfo.BindingsSet.begin()));
}

GAL::VulkanBindingsSetLayout::VulkanBindingsSetLayout(const CreateInfo& createInfo)
{
	GTSL::Array<VkDescriptorBindingFlags, 16> vkDescriptorBindingFlags;
	VkDescriptorSetLayoutBindingFlagsCreateInfo vkDescriptorSetLayoutBindingFlagsCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
	vkDescriptorSetLayoutBindingFlagsCreateInfo.bindingCount = static_cast<GTSL::uint32>(createInfo.BindingsDescriptors.ElementCount());
	vkDescriptorSetLayoutBindingFlagsCreateInfo.pBindingFlags = vkDescriptorBindingFlags.begin();
	
	VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	vkDescriptorSetLayoutCreateInfo.pNext = &vkDescriptorSetLayoutBindingFlagsCreateInfo;
	
	GTSL::Array<VkDescriptorSetLayoutBinding, MAX_BINDINGS_PER_SET> vkDescriptorSetLayoutBindings;

	for (GTSL::uint32 i = 0; i < createInfo.BindingsDescriptors.ElementCount(); ++i) {
		vkDescriptorBindingFlags.EmplaceBack();
		vkDescriptorSetLayoutBindings.EmplaceBack();
	}
	
	{
		GTSL::uint8 i = 0;

		for (auto& binding : vkDescriptorSetLayoutBindings)
		{
			binding.binding = i;
			binding.descriptorCount = createInfo.BindingsDescriptors[i].BindingsCount;
			binding.descriptorType = static_cast<VkDescriptorType>(createInfo.BindingsDescriptors[i].BindingType);
			binding.stageFlags = createInfo.BindingsDescriptors[i].ShaderStage;
			binding.pImmutableSamplers = nullptr;

			vkDescriptorBindingFlags[i] = createInfo.BindingsDescriptors[i].Flags;
			++i;
		}
		
	}

	vkDescriptorSetLayoutCreateInfo.bindingCount = vkDescriptorSetLayoutBindings.GetLength();
	vkDescriptorSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.begin();

	VK_CHECK(vkCreateDescriptorSetLayout(createInfo.RenderDevice->GetVkDevice(), &vkDescriptorSetLayoutCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &descriptorSetLayout));

	SET_NAME(descriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, createInfo);
}

void GAL::VulkanBindingsSetLayout::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyDescriptorSetLayout(renderDevice->GetVkDevice(), descriptorSetLayout, renderDevice->GetVkAllocationCallbacks());
	debugClear(descriptorSetLayout);
}