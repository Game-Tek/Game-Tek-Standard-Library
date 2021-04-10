#include "GAL/Vulkan/VulkanTexture.h"

#include "GAL/Vulkan/VulkanRenderDevice.h"

#include "GAL/Vulkan/VulkanMemory.h"

void GAL::VulkanTexture::GetMemoryRequirements(const VulkanRenderDevice* renderDevice, MemoryRequirements* memoryRequirements,
	VulkanTextureLayout initialLayout, VulkanTextureUses uses, VulkanTextureFormat format, GTSL::Extent3D extent,
	VulkanTextureTiling tiling, GTSL::uint8 mipLevels)
{
	auto selectType = [](GTSL::Extent3D extent) -> VkImageType
	{
		if (extent.Width != 1) {
			if (extent.Height != 1) {
				if (extent.Depth != 1) {
					return VK_IMAGE_TYPE_3D;
				}

				return VK_IMAGE_TYPE_2D;
			}
		}

		return VK_IMAGE_TYPE_1D;
	};
	
	VkImageCreateInfo vkImageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	vkImageCreateInfo.imageType = selectType(extent);
	vkImageCreateInfo.extent = Extent3DToVkExtent3D(extent);
	vkImageCreateInfo.mipLevels = mipLevels;
	vkImageCreateInfo.arrayLayers = 1;
	vkImageCreateInfo.format = static_cast<VkFormat>(format);
	vkImageCreateInfo.tiling = static_cast<VkImageTiling>(tiling);
	vkImageCreateInfo.initialLayout = static_cast<VkImageLayout>(initialLayout);
	vkImageCreateInfo.usage = static_cast<VkImageUsageFlags>(uses);
	vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateImage(renderDevice->GetVkDevice(), &vkImageCreateInfo, renderDevice->GetVkAllocationCallbacks(), &image))

	VkMemoryRequirements vkMemoryRequirements;
	vkGetImageMemoryRequirements(renderDevice->GetVkDevice(), image, &vkMemoryRequirements);
	memoryRequirements->Size = static_cast<GTSL::uint32>(vkMemoryRequirements.size);
	memoryRequirements->Alignment = static_cast<GTSL::uint32>(vkMemoryRequirements.alignment);
	memoryRequirements->MemoryTypes = vkMemoryRequirements.memoryTypeBits;
}

void GAL::VulkanTexture::Initialize(const VulkanRenderDevice* renderDevice, VulkanDeviceMemory deviceMemory,
	GTSL::uint32 offset)
{
	//SET_NAME(image, VK_OBJECT_TYPE_IMAGE, createInfo);
	VK_CHECK(vkBindImageMemory(renderDevice->GetVkDevice(), image, static_cast<VkDeviceMemory>(deviceMemory.GetVkDeviceMemory()), offset))
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
