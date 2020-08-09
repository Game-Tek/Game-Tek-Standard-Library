#include "GAL/Vulkan/VulkanRenderPass.h"

#include <GTSL/Vector.hpp>
#include "GAL/Vulkan/VulkanRenderDevice.h"

GAL::VulkanRenderPass::VulkanRenderPass(const CreateInfo& createInfo)
{
	const bool depth_attachment = createInfo.Descriptor.DepthStencilAttachmentAvailable;

	GTSL::Array<VkAttachmentDescription, 64> vk_attachment_descriptions(createInfo.Descriptor.RenderPassColorAttachments.ElementCount() + depth_attachment);

	for (GTSL::uint32 i = 0; i < vk_attachment_descriptions.GetLength() - depth_attachment; ++i)
	{
		vk_attachment_descriptions[i].flags = 0;
		vk_attachment_descriptions[i].format = static_cast<VkFormat>(createInfo.Descriptor.RenderPassColorAttachments[i].Format);
		vk_attachment_descriptions[i].samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
		vk_attachment_descriptions[i].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.Descriptor.RenderPassColorAttachments[i].LoadOperation);
		vk_attachment_descriptions[i].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.Descriptor.RenderPassColorAttachments[i].StoreOperation);
		vk_attachment_descriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vk_attachment_descriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_attachment_descriptions[i].initialLayout = ImageLayoutToVkImageLayout(createInfo.Descriptor.RenderPassColorAttachments[i].InitialLayout);
		vk_attachment_descriptions[i].finalLayout = ImageLayoutToVkImageLayout(createInfo.Descriptor.RenderPassColorAttachments[i].FinalLayout);
	}
	if (depth_attachment)
	{
		//Set depth/stencil element.
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].flags = 0;
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].format = static_cast<VkFormat>(createInfo.Descriptor.DepthStencilAttachment.Format);
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].samples = VK_SAMPLE_COUNT_1_BIT;
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.Descriptor.DepthStencilAttachment.LoadOperation);
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.Descriptor.DepthStencilAttachment.StoreOperation);
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].stencilLoadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.Descriptor.DepthStencilAttachment.LoadOperation);
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].stencilStoreOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.Descriptor.DepthStencilAttachment.StoreOperation);
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].initialLayout = ImageLayoutToVkImageLayout(createInfo.Descriptor.DepthStencilAttachment.InitialLayout);
		vk_attachment_descriptions[vk_attachment_descriptions.GetLength() - 1].finalLayout = ImageLayoutToVkImageLayout(createInfo.Descriptor.DepthStencilAttachment.FinalLayout);
	}


	const GTSL::uint8 attachments_count = createInfo.Descriptor.SubPasses.ElementCount() * createInfo.Descriptor.RenderPassColorAttachments.ElementCount();
	GTSL::Array<VkAttachmentReference, 64> write_attachments_references(attachments_count);
	GTSL::Array<VkAttachmentReference, 64> read_attachments_references(attachments_count);
	GTSL::Array<GTSL::uint32, 64> preserve_attachments_indices(attachments_count);
	GTSL::Array<VkAttachmentReference, 64> depth_attachment_references(createInfo.Descriptor.SubPasses.ElementCount());

	GTSL::uint8 write_attachments_count = 0;
	GTSL::uint8 read_attachments_count = 0;
	GTSL::uint8 preserve_attachments_count = 0;

	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
	{
		GTSL::uint32 written_attachment_references_this_subpass_loop = 0;

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATT)
		{
			if (createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATT].Index == ATTACHMENT_UNUSED)
			{
				write_attachments_references[SUBPASS + ATT].attachment = VK_ATTACHMENT_UNUSED;
				write_attachments_references[SUBPASS + ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				write_attachments_references[SUBPASS + ATT].attachment = createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATT].Index;
				write_attachments_references[SUBPASS + ATT].layout = ImageLayoutToVkImageLayout(createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATT].Layout);

				++write_attachments_count;
				++written_attachment_references_this_subpass_loop;
			}
		}

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++ATT)
		{
			if (createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index == ATTACHMENT_UNUSED)
			{
				read_attachments_references[SUBPASS + ATT].attachment = VK_ATTACHMENT_UNUSED;
				read_attachments_references[SUBPASS + ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				read_attachments_references[SUBPASS + ATT].attachment = createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index;
				read_attachments_references[SUBPASS + ATT].layout = ImageLayoutToVkImageLayout(createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Layout);

				++read_attachments_count;
				++written_attachment_references_this_subpass_loop;
			}
		}

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT)
		{
			if (createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT] == ATTACHMENT_UNUSED)
			{
				preserve_attachments_indices[SUBPASS + ATT] = VK_ATTACHMENT_UNUSED;
			}
			else
			{
				preserve_attachments_indices[SUBPASS + ATT] = createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT];

				++preserve_attachments_count;
				++written_attachment_references_this_subpass_loop;
			}
		}

		if (createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference)
		{
			depth_attachment_references[SUBPASS].attachment = createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference->Index;
			depth_attachment_references[SUBPASS].layout = ImageLayoutToVkImageLayout(createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference->Layout);
		}
		else
		{
			depth_attachment_references[SUBPASS].attachment = VK_ATTACHMENT_UNUSED;
			depth_attachment_references[SUBPASS].layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	GTSL::Array<VkSubpassDescription, 64> vk_subpass_descriptions(createInfo.Descriptor.SubPasses.ElementCount());
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < vk_subpass_descriptions.GetLength(); ++SUBPASS)
	{
		vk_subpass_descriptions[SUBPASS].flags = 0;
		vk_subpass_descriptions[SUBPASS].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vk_subpass_descriptions[SUBPASS].colorAttachmentCount = write_attachments_count;
		vk_subpass_descriptions[SUBPASS].pColorAttachments = write_attachments_references.begin() + SUBPASS;
		vk_subpass_descriptions[SUBPASS].inputAttachmentCount = read_attachments_count;
		vk_subpass_descriptions[SUBPASS].pInputAttachments = read_attachments_references.begin() + SUBPASS;
		vk_subpass_descriptions[SUBPASS].pResolveAttachments = nullptr;
		vk_subpass_descriptions[SUBPASS].preserveAttachmentCount = 0; //PreserveAttachmentsCount;
		vk_subpass_descriptions[SUBPASS].pPreserveAttachments = preserve_attachments_indices.begin() + SUBPASS;
		vk_subpass_descriptions[SUBPASS].pDepthStencilAttachment = &depth_attachment_references[SUBPASS];
	}

	GTSL::uint32 array_length = 0;
	for (GTSL::uint32 i = 0; i < createInfo.Descriptor.SubPasses.ElementCount(); ++i)
	{
		array_length += createInfo.Descriptor.SubPasses[i].ReadColorAttachments.ElementCount() + createInfo.Descriptor.SubPasses[i].WriteColorAttachments.ElementCount();
	}

	GTSL::Array<VkSubpassDependency, 64> vk_subpass_dependencies(array_length);
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
	{
		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments.ElementCount() + createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATT)
		{
			vk_subpass_dependencies[SUBPASS + ATT].srcSubpass = VK_SUBPASS_EXTERNAL;
			vk_subpass_dependencies[SUBPASS + ATT].dstSubpass = SUBPASS;
			vk_subpass_dependencies[SUBPASS + ATT].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vk_subpass_dependencies[SUBPASS + ATT].srcAccessMask = 0;
			vk_subpass_dependencies[SUBPASS + ATT].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vk_subpass_dependencies[SUBPASS + ATT].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vk_subpass_dependencies[SUBPASS + ATT].dependencyFlags = 0;
		}
	}

	VkRenderPassCreateInfo vk_renderpass_create_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	vk_renderpass_create_info.attachmentCount = createInfo.Descriptor.RenderPassColorAttachments.ElementCount() + depth_attachment;
	vk_renderpass_create_info.pAttachments = vk_attachment_descriptions.begin();
	vk_renderpass_create_info.subpassCount = createInfo.Descriptor.SubPasses.ElementCount();
	vk_renderpass_create_info.pSubpasses = vk_subpass_descriptions.begin();
	vk_renderpass_create_info.dependencyCount = array_length;
	vk_renderpass_create_info.pDependencies = vk_subpass_dependencies.begin();

	VK_CHECK(vkCreateRenderPass(createInfo.RenderDevice->GetVkDevice(), &vk_renderpass_create_info, createInfo.RenderDevice->GetVkAllocationCallbacks(), &renderPass));
	SET_NAME(renderPass, VK_OBJECT_TYPE_RENDER_PASS, createInfo);
}

void GAL::VulkanRenderPass::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyRenderPass(renderDevice->GetVkDevice(), renderPass, renderDevice->GetVkAllocationCallbacks());
}
