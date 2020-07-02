#include "GAL/Vulkan/VulkanBindings.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GAL/Vulkan/VulkanImage.h"
#include "GAL/Vulkan/VulkanBuffer.h"

GAL::VulkanBindingsPool::VulkanBindingsPool(const CreateInfo& createInfo)
{
	GTSL::Array<VkDescriptorPoolSize, MAX_BINDINGS_PER_SET> descriptor_pool_sizes(createInfo.BindingsDescriptors.ElementCount());
	for (auto& descriptor_pool_size : descriptor_pool_sizes)
	{
		descriptor_pool_size.type = UniformTypeToVkDescriptorType(createInfo.BindingsDescriptors[&descriptor_pool_size - descriptor_pool_sizes.begin()].BindingType);
		//Max number of descriptors of VkDescriptorPoolSize::type we can allocate.
		descriptor_pool_size.descriptorCount = createInfo.BindingsDescriptors[&descriptor_pool_size - descriptor_pool_sizes.begin()].MaxNumberOfBindingsAllocatable;
	}

	VkDescriptorPoolCreateInfo vk_descriptor_pool_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	//Is the total number of sets that can be allocated from the pool.
	vk_descriptor_pool_create_info.maxSets = createInfo.BindingsSets.ElementCount();
	vk_descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.GetLength();
	vk_descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.begin();

	vkCreateDescriptorPool(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_pool_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &descriptorPool);

	VkDescriptorSetLayoutCreateInfo vk_descriptor_set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };

	GTSL::Array<VkDescriptorSetLayoutBinding, MAX_BINDINGS_PER_SET> descriptor_set_layout_bindings(createInfo.BindingsSets.ElementCount());
	{
		GTSL::uint8 i = 0;

		for (auto& binding : descriptor_set_layout_bindings)
		{
			binding.binding = i;
			binding.descriptorCount = createInfo.BindingsDescriptors[i].MaxNumberOfBindingsAllocatable;
			binding.descriptorType = UniformTypeToVkDescriptorType(createInfo.BindingsDescriptors[i].BindingType);
			binding.stageFlags = ShaderTypeToVkShaderStageFlagBits(createInfo.BindingsDescriptors[i].ShaderStage);
			binding.pImmutableSamplers = nullptr;
			++i;
		}
	}

	vk_descriptor_set_layout_create_info.bindingCount = descriptor_set_layout_bindings.GetLength();
	vk_descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.begin();

	vkCreateDescriptorSetLayout(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_set_layout_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &descriptorSetLayout);

	VkDescriptorSetAllocateInfo vk_descriptor_set_allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	vk_descriptor_set_allocate_info.descriptorPool = descriptorPool;
	vk_descriptor_set_allocate_info.descriptorSetCount = createInfo.BindingsSets.ElementCount();
	vkAllocateDescriptorSets(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_set_allocate_info, reinterpret_cast<VkDescriptorSet*>(createInfo.BindingsSets.begin()));
}

void GAL::VulkanBindingsPool::Destroy(RenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyDescriptorSetLayout(vk_render_device->GetVkDevice(), descriptorSetLayout, vk_render_device->GetVkAllocationCallbacks());
	vkDestroyDescriptorPool(vk_render_device->GetVkDevice(), descriptorPool, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanBindingsPool::FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo)
{
	vkFreeDescriptorSets(static_cast<VulkanRenderDevice*>(freeBindingsSetInfo.RenderDevice)->GetVkDevice(), descriptorPool,
	static_cast<GTSL::uint32>(freeBindingsSetInfo.BindingsSet.ElementCount()), reinterpret_cast<VkDescriptorSet*>(freeBindingsSetInfo.BindingsSet.begin()));
}

void GAL::VulkanBindingsSet::Update(const BindingsSetUpdateInfo& bindingsUpdateInfo)
{
	GTSL::Array<VkWriteDescriptorSet, 32> write_descriptors;

	GTSL::Array<GTSL::Array<VkDescriptorImageInfo, 16>, 16> vk_descriptor_image_infos;
	GTSL::Array<GTSL::Array<VkDescriptorBufferInfo, 16>, 16> vk_descriptor_buffer_infos;
	
	uint32_t write_descriptor_index = 0;
	uint32_t image_set = 0;
	uint32_t buffer_set = 0;
	
	for(const auto& e : bindingsUpdateInfo.ImageBindingsSetLayout)
	{
		write_descriptor_index = write_descriptors.EmplaceBack();
		
		image_set = vk_descriptor_image_infos.EmplaceBack();
		
		for(GTSL::uint8 i = 0; i < e.ImageViews.ElementCount(); ++i)
		{
			vk_descriptor_image_infos[image_set].EmplaceBack();
			
			vk_descriptor_image_infos[image_set][i].imageView = static_cast<const VulkanImageView&>(e.ImageViews[i]).GetVkImageView();
			vk_descriptor_image_infos[image_set][i].imageLayout = ImageLayoutToVkImageLayout(e.Layouts[i]);
			vk_descriptor_image_infos[image_set][i].sampler = static_cast<const VulkanSampler&>(e.Samplers[i]).GetVkSampler();
		}
		
		write_descriptors[write_descriptor_index].pNext = nullptr;
		write_descriptors[write_descriptor_index].dstSet = descriptorSet;
		write_descriptors[write_descriptor_index].dstBinding = write_descriptor_index;
		write_descriptors[write_descriptor_index].dstArrayElement = 0;
		write_descriptors[write_descriptor_index].descriptorCount = e.ImageViews.ElementCount();
		write_descriptors[write_descriptor_index].descriptorType = UniformTypeToVkDescriptorType(e.BindingType);
		write_descriptors[write_descriptor_index].pImageInfo = vk_descriptor_image_infos[image_set].begin();
		write_descriptors[write_descriptor_index].pBufferInfo = nullptr;
		write_descriptors[write_descriptor_index].pTexelBufferView = nullptr;
	}

	for(const auto& e : bindingsUpdateInfo.BufferBindingsSetLayout)
	{
		write_descriptor_index = write_descriptors.EmplaceBack();
		
		buffer_set = vk_descriptor_buffer_infos.EmplaceBack();
		
		for(GTSL::uint8 i = 0; i < e.Buffers.ElementCount(); ++i)
		{
			vk_descriptor_buffer_infos[buffer_set].EmplaceBack();
			
			vk_descriptor_buffer_infos[buffer_set][i].buffer = static_cast<const VulkanBuffer&>(e.Buffers[i]).GetVkBuffer();
			vk_descriptor_buffer_infos[buffer_set][i].range = e.Sizes[i];
			vk_descriptor_buffer_infos[buffer_set][i].offset = e.Offsets[i];
		}
		
		write_descriptors[write_descriptor_index].pNext = nullptr;
		write_descriptors[write_descriptor_index].dstSet = descriptorSet;
		write_descriptors[write_descriptor_index].dstBinding = write_descriptor_index;
		write_descriptors[write_descriptor_index].dstArrayElement = 0;
		write_descriptors[write_descriptor_index].descriptorCount = e.Buffers.ElementCount();
		write_descriptors[write_descriptor_index].descriptorType = UniformTypeToVkDescriptorType(e.BindingType);
		write_descriptors[write_descriptor_index].pImageInfo = nullptr;
		write_descriptors[write_descriptor_index].pBufferInfo = vk_descriptor_buffer_infos[buffer_set].begin();
		write_descriptors[write_descriptor_index].pTexelBufferView = nullptr;
	}

	vkUpdateDescriptorSets(static_cast<VulkanRenderDevice*>(bindingsUpdateInfo.RenderDevice)->GetVkDevice(), write_descriptors.GetCapacity(), write_descriptors.begin(), 0, nullptr);
}
