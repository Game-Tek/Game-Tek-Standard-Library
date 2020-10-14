#include "GAL/Vulkan/VulkanTexture.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanMemory.h"

void GAL::VulkanTexture::GetMemoryRequirements(const GetMemoryRequirementsInfo& info)
{	
	VkImageCreateInfo vkImageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vkImageCreateInfo.imageType = static_cast<VkImageType>(info.CreateInfo.Dimensions);
	vkImageCreateInfo.extent = Extent3DToVkExtent3D(info.CreateInfo.Extent);
	vkImageCreateInfo.mipLevels = info.CreateInfo.MipLevels;
	vkImageCreateInfo.arrayLayers = 1;
	vkImageCreateInfo.format = static_cast<VkFormat>(info.CreateInfo.Format);
	vkImageCreateInfo.tiling = static_cast<VkImageTiling>(info.CreateInfo.Tiling);
	vkImageCreateInfo.initialLayout = static_cast<VkImageLayout>(info.CreateInfo.InitialLayout);
	vkImageCreateInfo.usage = info.CreateInfo.Uses;
	vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateImage(info.RenderDevice->GetVkDevice(), &vkImageCreateInfo, info.RenderDevice->GetVkAllocationCallbacks(), &image))
	
	VkMemoryRequirements vkMemoryRequirements;
	vkGetImageMemoryRequirements(info.RenderDevice->GetVkDevice(), image, &vkMemoryRequirements);
	info.MemoryRequirements->Size = static_cast<GTSL::uint32>(vkMemoryRequirements.size);
	info.MemoryRequirements->Alignment = static_cast<GTSL::uint32>(vkMemoryRequirements.alignment);
	info.MemoryRequirements->MemoryTypes = vkMemoryRequirements.memoryTypeBits;
}

void GAL::VulkanTexture::Initialize(const CreateInfo& createInfo)
{
	SET_NAME(image, VK_OBJECT_TYPE_IMAGE, createInfo)
	VK_CHECK(vkBindImageMemory(createInfo.RenderDevice->GetVkDevice(), image, static_cast<VkDeviceMemory>(createInfo.Memory.GetVkDeviceMemory()), createInfo.Offset))
}

void GAL::VulkanTexture::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyImage(renderDevice->GetVkDevice(), image, renderDevice->GetVkAllocationCallbacks());
	debugClear(image);
}

GAL::VulkanTextureView::VulkanTextureView(const CreateInfo& createInfo)
{
	VkImageViewCreateInfo vkImageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	vkImageViewCreateInfo.image = createInfo.Texture.GetVkImage();
	vkImageViewCreateInfo.viewType = static_cast<VkImageViewType>(createInfo.Dimensions);
	vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	vkImageViewCreateInfo.format = static_cast<VkFormat>(createInfo.Format);
	vkImageViewCreateInfo.subresourceRange.aspectMask = createInfo.Type;
	vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	vkImageViewCreateInfo.subresourceRange.levelCount = createInfo.MipLevels;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount = 1;
	
	VK_CHECK(vkCreateImageView(createInfo.RenderDevice->GetVkDevice(), &vkImageViewCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &imageView))
	SET_NAME(imageView, VK_OBJECT_TYPE_IMAGE_VIEW, createInfo)
}

void GAL::VulkanTextureView::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyImageView(renderDevice->GetVkDevice(), imageView, renderDevice->GetVkAllocationCallbacks());
	debugClear(imageView);
}

GAL::VulkanSampler::VulkanSampler(const CreateInfo& createInfo)
{
	VkSamplerCreateInfo vkSamplerCreateInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	vkSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;

	vkSamplerCreateInfo.maxAnisotropy = static_cast<float>(createInfo.Anisotropy == 0 ? 1 : createInfo.Anisotropy);
	vkSamplerCreateInfo.anisotropyEnable = static_cast<VkBool32>(createInfo.Anisotropy);

	vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	vkSamplerCreateInfo.compareEnable = VK_FALSE;
	vkSamplerCreateInfo.mipLodBias = 0.0f;
	vkSamplerCreateInfo.minLod = 0.0f;
	vkSamplerCreateInfo.maxLod = 0.0f;

	VK_CHECK(vkCreateSampler(createInfo.RenderDevice->GetVkDevice(), &vkSamplerCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &sampler))
	SET_NAME(sampler, VK_OBJECT_TYPE_SAMPLER, createInfo)
}

void GAL::VulkanSampler::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroySampler(renderDevice->GetVkDevice(), sampler, renderDevice->GetVkAllocationCallbacks());
	debugClear(sampler);
}
