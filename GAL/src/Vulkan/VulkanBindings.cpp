#include "GAL/Vulkan/VulkanBindings.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanImage.h"

#include <GTSL/Vector.hpp>
#include <GTSL/FixedVector.hpp>

#include "GAL/Vulkan/VulkanBuffer.h"

GAL::VulkanBindingsPool::VulkanBindingsPool(const CreateInfo& createInfo)
{
	GTSL::Array<VkDescriptorPoolSize, GAL::MAX_BINDINGS_PER_SET> descriptor_pool_sizes;
	descriptor_pool_sizes.Resize(createInfo.BindingsSetLayout.GetLength());
	{
		GTSL::uint8 i = 0;

		for (auto& descriptor_pool_size : descriptor_pool_sizes)
		{
			//Type of the descriptor pool.
			descriptor_pool_size.type = UniformTypeToVkDescriptorType(createInfo.BindingsSetLayout[i].BindingType);
			//Max number of descriptors of VkDescriptorPoolSize::type we can allocate.
			descriptor_pool_size.descriptorCount = createInfo.BindingsSetCount;

			++i;
		}
	}

	VkDescriptorPoolCreateInfo vk_descriptor_pool_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	//Is the total number of sets that can be allocated from the pool.
	vk_descriptor_pool_create_info.maxSets = createInfo.BindingsSetCount;
	vk_descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.GetLength();
	vk_descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.GetData();

	vkCreateDescriptorPool(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_pool_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &vkDescriptorPool);
}

void GAL::VulkanBindingsPool::Destroy(GAL::RenderDevice* renderDevice)
{
	const auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyDescriptorPool(vk_render_device->GetVkDevice(), vkDescriptorPool, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanBindingsPool::FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo)
{
	vkFreeDescriptorSets(static_cast<VulkanRenderDevice*>(freeBindingsSetInfo.RenderDevice)->GetVkDevice(), vkDescriptorPool, static_cast<VulkanBindingsSet*>(freeBindingsSetInfo.BindingsSet)->GetVkDescriptorSets().GetLength(), static_cast<VulkanBindingsSet*>(freeBindingsSetInfo.BindingsSet)->GetVkDescriptorSets().GetData());
}

void GAL::VulkanBindingsPool::FreePool(const FreeBindingsPoolInfo& freeDescriptorPoolInfo)
{
	vkResetDescriptorPool(static_cast<VulkanRenderDevice*>(freeDescriptorPoolInfo.RenderDevice)->GetVkDevice(), vkDescriptorPool, 0);
}

GAL::VulkanBindingsSet::VulkanBindingsSet(const CreateInfo& createInfo)
{
	VkDescriptorSetLayoutCreateInfo vk_descriptor_set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };

	GTSL::Array<VkDescriptorSetLayoutBinding, MAX_BINDINGS_PER_SET> descriptor_set_layout_bindings(createInfo.BindingsSetCount);
	{
		GTSL::uint8 i = 0;

		for (auto& binding : descriptor_set_layout_bindings)
		{
			binding.binding = i;
			binding.descriptorCount = createInfo.BindingsSetBindingCount[i];
			binding.descriptorType = UniformTypeToVkDescriptorType(createInfo.BindingsSetLayout[i].BindingType);
			binding.stageFlags = ShaderTypeToVkShaderStageFlagBits(createInfo.BindingsSetLayout[i].ShaderStage);
			binding.pImmutableSamplers = nullptr;
			++i;
		}
	}

	vk_descriptor_set_layout_create_info.bindingCount = descriptor_set_layout_bindings.GetLength();
	vk_descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.GetData();

	vkCreateDescriptorSetLayout(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_set_layout_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &descriptorSetLayout);

	VkDescriptorSetAllocateInfo vk_descriptor_set_allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	vk_descriptor_set_allocate_info.descriptorPool = static_cast<VulkanBindingsPool*>(createInfo.BindingsPool)->GetVkDescriptorPool();
	vk_descriptor_set_allocate_info.descriptorSetCount = createInfo.BindingsSetCount;

	GTSL::Array<VkDescriptorSetLayout, 32> vk_descriptor_set_layouts(createInfo.BindingsSetCount);
	vk_descriptor_set_allocate_info.pSetLayouts = vk_descriptor_set_layouts.GetData();

	descriptorSets.Resize(vk_descriptor_set_allocate_info.descriptorSetCount);

	vkAllocateDescriptorSets(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(),	&vk_descriptor_set_allocate_info, descriptorSets.GetData());
}

void GAL::VulkanBindingsSet::Destroy(GAL::RenderDevice* renderDevice)
{
	auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyDescriptorSetLayout(vk_render_device->GetVkDevice(), descriptorSetLayout, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanBindingsSet::Update(const BindingsSetUpdateInfo& bindingsUpdateInfo)
{
	GTSL::Array<VkWriteDescriptorSet, 32> write_descriptors;

	GTSL::Array<GTSL::Array<VkDescriptorImageInfo, 16, GTSL::uint8>, 16, GTSL::uint8> vk_descriptor_image_infos;
	GTSL::Array<GTSL::Array<VkDescriptorBufferInfo, 16, GTSL::uint8>, 16, GTSL::uint8> vk_descriptor_buffer_infos;
	
	uint32_t write_descriptor_index = 0;
	uint32_t image_set = 0;
	uint32_t buffer_set = 0;
	
	for(const auto& e : bindingsUpdateInfo.ImageBindingsSetLayout)
	{
		write_descriptor_index = write_descriptors.EmplaceBack();
		
		image_set = vk_descriptor_image_infos.EmplaceBack();
		
		for(GTSL::uint8 i = 0; i < e.Images.ElementCount(); ++i)
		{
			vk_descriptor_image_infos[image_set].EmplaceBack();
			
			vk_descriptor_image_infos[image_set][i].imageView = static_cast<VulkanImage*>(e.Images[i])->GetVkImageView();
			vk_descriptor_image_infos[image_set][i].imageLayout = ImageLayoutToVkImageLayout(e.Layouts[i]);
			vk_descriptor_image_infos[image_set][i].sampler = static_cast<VulkanSampler*>(e.Samplers[i])->GetVkSampler();
		}
		
		write_descriptors[write_descriptor_index].pNext = nullptr;
		write_descriptors[write_descriptor_index].dstSet = descriptorSets[bindingsUpdateInfo.DestinationSet];
		write_descriptors[write_descriptor_index].dstBinding = write_descriptor_index;
		write_descriptors[write_descriptor_index].dstArrayElement = 0;
		write_descriptors[write_descriptor_index].descriptorCount = e.Images.ElementCount();
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
			
			vk_descriptor_buffer_infos[buffer_set][i].buffer = static_cast<VulkanBuffer*>(e.Buffers[i])->GetVkBuffer();
			vk_descriptor_buffer_infos[buffer_set][i].range = e.Sizes[i];
			vk_descriptor_buffer_infos[buffer_set][i].offset = e.Offsets[i];
		}
		
		write_descriptors[write_descriptor_index].pNext = nullptr;
		write_descriptors[write_descriptor_index].dstSet = descriptorSets[bindingsUpdateInfo.DestinationSet];
		write_descriptors[write_descriptor_index].dstBinding = write_descriptor_index;
		write_descriptors[write_descriptor_index].dstArrayElement = 0;
		write_descriptors[write_descriptor_index].descriptorCount = e.Buffers.ElementCount();
		write_descriptors[write_descriptor_index].descriptorType = UniformTypeToVkDescriptorType(e.BindingType);
		write_descriptors[write_descriptor_index].pImageInfo = nullptr;
		write_descriptors[write_descriptor_index].pBufferInfo = vk_descriptor_buffer_infos[buffer_set].begin();
		write_descriptors[write_descriptor_index].pTexelBufferView = nullptr;
	}

	vkUpdateDescriptorSets(static_cast<VulkanRenderDevice*>(bindingsUpdateInfo.RenderDevice)->GetVkDevice(), write_descriptors.GetCapacity(), write_descriptors.GetData(), 0, nullptr);
}
