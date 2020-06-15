#include "GAL/Vulkan/VulkanBindings.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanTexture.h"

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

	GTSL::Array<VkDescriptorSetLayoutBinding, GAL::MAX_BINDINGS_PER_SET> descriptor_set_layout_bindings;
	descriptor_set_layout_bindings.Resize(createInfo.BindingsSetLayout.GetLength());
	{
		GTSL::uint8 i = 0;

		for (auto& binding : descriptor_set_layout_bindings)
		{
			binding.binding = i;
			binding.descriptorCount = createInfo.BindingsSetLayout[i].ArrayLength;
			binding.descriptorType = UniformTypeToVkDescriptorType(createInfo.BindingsSetLayout[i].BindingType);
			binding.stageFlags = ShaderTypeToVkShaderStageFlagBits(createInfo.BindingsSetLayout[i].ShaderStage);
			binding.pImmutableSamplers = nullptr;

			++i;
		}
	}

	vk_descriptor_set_layout_create_info.bindingCount = descriptor_set_layout_bindings.GetLength();
	vk_descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.GetData();

	vkCreateDescriptorSetLayout(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(), &vk_descriptor_set_layout_create_info, static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkAllocationCallbacks(), &vkDescriptorSetLayout);

	VkDescriptorSetAllocateInfo vk_descriptor_set_allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	vk_descriptor_set_allocate_info.descriptorPool = static_cast<VulkanBindingsPool*>(createInfo.BindingsPool)->GetVkDescriptorPool();
	vk_descriptor_set_allocate_info.descriptorSetCount = createInfo.BindingsSetCount;

	GTSL::Array<VkDescriptorSetLayout, 32> vk_descriptor_set_layouts(createInfo.BindingsSetCount);
	vk_descriptor_set_allocate_info.pSetLayouts = vk_descriptor_set_layouts.GetData();

	vkDescriptorSets.Resize(vk_descriptor_set_allocate_info.descriptorSetCount);

	vkAllocateDescriptorSets(static_cast<VulkanRenderDevice*>(createInfo.RenderDevice)->GetVkDevice(),	&vk_descriptor_set_allocate_info, vkDescriptorSets.GetData());
}

void GAL::VulkanBindingsSet::Destroy(GAL::RenderDevice* renderDevice)
{
	auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyDescriptorSetLayout(vk_render_device->GetVkDevice(), vkDescriptorSetLayout, vk_render_device->GetVkAllocationCallbacks());
}

void GAL::VulkanBindingsSet::Update(const BindingsSetUpdateInfo& uniformLayoutUpdateInfo)
{
	GTSL::FixedVector<VkWriteDescriptorSet> write_descriptors(uniformLayoutUpdateInfo.BindingsSetLayout.GetLength(), uniformLayoutUpdateInfo.RenderDevice->GetTransientAllocationsAllocatorReference());

	for (GTSL::uint8 i = 0; i < uniformLayoutUpdateInfo.BindingsSetLayout.GetLength(); ++i)
	{
		switch (uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingType)
		{
		case GAL::BindingType::SAMPLER:
		case GAL::BindingType::COMBINED_IMAGE_SAMPLER:
		case GAL::BindingType::SAMPLED_IMAGE:

			VkDescriptorImageInfo vk_descriptor_image_info;
			vk_descriptor_image_info.imageView = static_cast<VulkanTexture*>(uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingResource)->GetImageView();
			vk_descriptor_image_info.imageLayout = static_cast<VulkanTexture*>(uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingResource)->GetVkImageLayout();
			vk_descriptor_image_info.sampler = static_cast<VulkanTexture*>(uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingResource)->GetImageSampler();

			write_descriptors[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptors[i].pNext = nullptr;
			write_descriptors[i].dstSet = vkDescriptorSets[uniformLayoutUpdateInfo.DestinationSet];
			write_descriptors[i].dstBinding = i;
			write_descriptors[i].dstArrayElement = 0;
			write_descriptors[i].descriptorCount = uniformLayoutUpdateInfo.BindingsSetLayout[i].ArrayLength;
			write_descriptors[i].descriptorType = UniformTypeToVkDescriptorType(uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingType);
			write_descriptors[i].pImageInfo = &vk_descriptor_image_info;
			write_descriptors[i].pTexelBufferView = nullptr;
			write_descriptors[i].pBufferInfo = nullptr;

			break;

			//case BindingType::STORAGE_IMAGE: break;

			//case BindingType::UNIFORM_TEXEL_BUFFER: break;
			//case BindingType::STORAGE_TEXEL_BUFFER: break;

		case GAL::BindingType::UNIFORM_BUFFER:
		case GAL::BindingType::STORAGE_BUFFER:

			VkDescriptorBufferInfo vk_descriptor_buffer_info;
			vk_descriptor_buffer_info.buffer = static_cast<VulkanBuffer*>(uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingResource)->GetVkBuffer();
			vk_descriptor_buffer_info.offset = 0; //TODO: Get offset from buffer itself
			vk_descriptor_buffer_info.range = VK_WHOLE_SIZE;

			write_descriptors[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_descriptors[i].pNext = nullptr;
			write_descriptors[i].dstSet = vkDescriptorSets[uniformLayoutUpdateInfo.DestinationSet];
			write_descriptors[i].dstBinding = i;
			write_descriptors[i].dstArrayElement = 0;
			write_descriptors[i].descriptorCount = uniformLayoutUpdateInfo.BindingsSetLayout[i].ArrayLength;
			write_descriptors[i].descriptorType = UniformTypeToVkDescriptorType(uniformLayoutUpdateInfo.BindingsSetLayout[i].BindingType);
			write_descriptors[i].pImageInfo = nullptr;
			write_descriptors[i].pTexelBufferView = nullptr;
			write_descriptors[i].pBufferInfo = &vk_descriptor_buffer_info;

			break;

			//case BindingType::UNIFORM_BUFFER_DYNAMIC: break;
			//case BindingType::STORAGE_BUFFER_DYNAMIC: break;
			//case BindingType::INPUT_ATTACHMENT: break;
		default: ;
		}
	}

	vkUpdateDescriptorSets(static_cast<VulkanRenderDevice*>(uniformLayoutUpdateInfo.RenderDevice)->GetVkDevice(), write_descriptors.GetCapacity(), write_descriptors.GetData(), 0, nullptr);
}
