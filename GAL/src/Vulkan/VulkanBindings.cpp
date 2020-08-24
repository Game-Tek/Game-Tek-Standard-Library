#include "GAL/Vulkan/VulkanBindings.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GAL/Vulkan/VulkanTexture.h"
#include "GAL/Vulkan/VulkanBuffer.h"

GAL::VulkanBindingsPool::VulkanBindingsPool(const CreateInfo& createInfo)
{
	GTSL::Array<VkDescriptorPoolSize, MAX_BINDINGS_PER_SET> descriptor_pool_sizes(createInfo.DescriptorPoolSizes.ElementCount());
	for (auto& descriptor_pool_size : descriptor_pool_sizes)
	{
		descriptor_pool_size.type = static_cast<VkDescriptorType>(createInfo.DescriptorPoolSizes[&descriptor_pool_size - descriptor_pool_sizes.begin()].BindingType);
		//Max number of descriptors of VkDescriptorPoolSize::type we can allocate.
		descriptor_pool_size.descriptorCount = createInfo.DescriptorPoolSizes[&descriptor_pool_size - descriptor_pool_sizes.begin()].Count;
	}

	VkDescriptorPoolCreateInfo vk_descriptor_pool_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	//Is the total number of sets that can be allocated from the pool.
	vk_descriptor_pool_create_info.maxSets = createInfo.MaxSets;
	vk_descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.GetLength();
	vk_descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.begin();
	VK_CHECK(vkCreateDescriptorPool(static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_pool_create_info, static_cast<const VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &descriptorPool));
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
	VkDescriptorSetVariableDescriptorCountAllocateInfo vkDescriptorSetVariableDescriptorCountAllocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO };
	vkDescriptorSetVariableDescriptorCountAllocateInfo.descriptorSetCount = static_cast<GTSL::uint32>(allocateBindingsSetsInfo.BindingsSets.ElementCount());
	vkDescriptorSetVariableDescriptorCountAllocateInfo.pDescriptorCounts = allocateBindingsSetsInfo.BindingsSetDynamicBindingsCounts.begin();
	
	VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	vkDescriptorSetAllocateInfo.pNext = &vkDescriptorSetVariableDescriptorCountAllocateInfo;
	vkDescriptorSetAllocateInfo.descriptorPool = descriptorPool;
	vkDescriptorSetAllocateInfo.descriptorSetCount = allocateBindingsSetsInfo.BindingsSets.ElementCount();
	vkDescriptorSetAllocateInfo.pSetLayouts = reinterpret_cast<const VkDescriptorSetLayout*>(allocateBindingsSetsInfo.BindingsSetLayouts.begin());
	VK_CHECK(vkAllocateDescriptorSets(allocateBindingsSetsInfo.RenderDevice->GetVkDevice(), &vkDescriptorSetAllocateInfo, reinterpret_cast<VkDescriptorSet*>(allocateBindingsSetsInfo.BindingsSets.begin())));

	if constexpr (_DEBUG)
	{
		for(GTSL::uint32 i = 0; i < allocateBindingsSetsInfo.BindingsSetCreateInfos.ElementCount(); ++i)
		{
			SET_NAME(allocateBindingsSetsInfo.BindingsSets[i].GetVkDescriptorSet(), VK_OBJECT_TYPE_DESCRIPTOR_SET, allocateBindingsSetsInfo.BindingsSetCreateInfos[i]);
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
	GTSL::Array<VkDescriptorBindingFlags, 16> vkDescriptorBindingFlags(createInfo.BindingsDescriptors.ElementCount());
	VkDescriptorSetLayoutBindingFlagsCreateInfo vkDescriptorSetLayoutBindingFlagsCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
	vkDescriptorSetLayoutBindingFlagsCreateInfo.bindingCount = static_cast<GTSL::uint32>(createInfo.SpecialBindings.ElementCount());
	vkDescriptorSetLayoutBindingFlagsCreateInfo.pBindingFlags = vkDescriptorBindingFlags.begin();
	
	VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	vkDescriptorSetLayoutCreateInfo.pNext = &vkDescriptorSetLayoutBindingFlagsCreateInfo;
	
	GTSL::Array<VkDescriptorSetLayoutBinding, MAX_BINDINGS_PER_SET> vkDescriptorSetLayoutBindings(createInfo.BindingsDescriptors.ElementCount());
	{
		GTSL::uint8 i = 0;

		for (auto& binding : vkDescriptorSetLayoutBindings)
		{
			binding.binding = i;
			binding.descriptorCount = createInfo.BindingsDescriptors[i].UniformCount;
			binding.descriptorType = static_cast<VkDescriptorType>(createInfo.BindingsDescriptors[i].BindingType);
			binding.stageFlags = createInfo.BindingsDescriptors[i].ShaderStage;
			binding.pImmutableSamplers = nullptr;

			++i;
		}
		
		vkDescriptorBindingFlags[i - 1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
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

void GAL::VulkanBindingsSet::Update(const BindingsSetUpdateInfo& bindingsUpdateInfo)
{
	GTSL::Array<VkWriteDescriptorSet, 128> vkWriteDescriptorSets(static_cast<uint32_t>(bindingsUpdateInfo.BindingUpdateInfos.ElementCount()));

	for(GTSL::uint32 binding = 0; binding < bindingsUpdateInfo.BindingUpdateInfos.ElementCount(); ++binding)
	{
		auto& writeSet = vkWriteDescriptorSets[binding];

		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.pNext = nullptr;
		writeSet.dstSet = descriptorSet;
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = bindingsUpdateInfo.BindingUpdateInfos[binding].ArrayElement;
		writeSet.descriptorCount = bindingsUpdateInfo.BindingUpdateInfos[binding].Count;
		writeSet.descriptorType = static_cast<VkDescriptorType>(bindingsUpdateInfo.BindingUpdateInfos[binding].Type);

		switch (bindingsUpdateInfo.BindingUpdateInfos[binding].Type)
		{
		case VulkanBindingType::SAMPLER:
		case VulkanBindingType::COMBINED_IMAGE_SAMPLER:
		case VulkanBindingType::SAMPLED_IMAGE:
		case VulkanBindingType::STORAGE_IMAGE: 
		{
			writeSet.pImageInfo = static_cast<VkDescriptorImageInfo*>(bindingsUpdateInfo.BindingUpdateInfos[binding].BindingsUpdates);
			writeSet.pBufferInfo = nullptr;
			writeSet.pTexelBufferView = nullptr;
			break;	
		}
			
		case VulkanBindingType::UNIFORM_TEXEL_BUFFER: GAL_DEBUG_BREAK;
		case VulkanBindingType::STORAGE_TEXEL_BUFFER: GAL_DEBUG_BREAK;
			
		case VulkanBindingType::UNIFORM_BUFFER:
		case VulkanBindingType::STORAGE_BUFFER:
		case VulkanBindingType::UNIFORM_BUFFER_DYNAMIC:
		case VulkanBindingType::STORAGE_BUFFER_DYNAMIC:
		{				
			writeSet.pImageInfo = nullptr;
			writeSet.pBufferInfo = static_cast<VkDescriptorBufferInfo*>(bindingsUpdateInfo.BindingUpdateInfos[binding].BindingsUpdates);
			writeSet.pTexelBufferView = nullptr;
			break;
		}
			
		case VulkanBindingType::INPUT_ATTACHMENT: GAL_DEBUG_BREAK;
		case VulkanBindingType::ACCELERATION_STRUCTURE: GAL_DEBUG_BREAK;
		default: __debugbreak();
		}
	}
	
	vkUpdateDescriptorSets(static_cast<const VulkanRenderDevice*>(bindingsUpdateInfo.RenderDevice)->GetVkDevice(), vkWriteDescriptorSets.GetLength(), vkWriteDescriptorSets.begin(), 0, nullptr);
}
