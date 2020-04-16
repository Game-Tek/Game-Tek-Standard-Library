#include "GAL/Vulkan/VulkanRenderPass.h"

#include <GTSL/Vector.hpp>
#include "GAL/Vulkan/VulkanRenderDevice.h"

VulkanRenderPass::VulkanRenderPass(class VulkanRenderDevice * vulkanRenderDevice, const GAL::RenderPassCreateInfo& renderPassCreateInfo)
{
	bool DSAA = renderPassCreateInfo.Descriptor.DepthStencilAttachment.AttachmentImage;

	GTSL::Vector<VkAttachmentDescription> Attachments(renderPassCreateInfo.Descriptor.RenderPassColorAttachments.GetLength() + DSAA, renderPassCreateInfo.Descriptor.RenderPassColorAttachments.GetLength() + DSAA, vulkanRenderDevice->GetTransientAllocationsAllocatorReference());
	//Take into account depth/stencil attachment
	{
		//Set color attachments.
		for (GTSL::uint8 i = 0; i < Attachments.GetCapacity() - DSAA; i++)
			//Loop through all color attachments(skip extra element for depth/stencil)
		{
			Attachments[i].format = FormatToVkFormat(renderPassCreateInfo.Descriptor.RenderPassColorAttachments[i]->AttachmentImage->GetFormat());
			Attachments[i].samples = VK_SAMPLE_COUNT_1_BIT; //Should match that of the SwapChain images.
			Attachments[i].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(renderPassCreateInfo.Descriptor.RenderPassColorAttachments[i]->LoadOperation);
			Attachments[i].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(renderPassCreateInfo.Descriptor.RenderPassColorAttachments[i]->StoreOperation);
			Attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			Attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			Attachments[i].initialLayout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.RenderPassColorAttachments[i]->InitialLayout);
			Attachments[i].finalLayout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.RenderPassColorAttachments[i]->FinalLayout);
		}

		if (DSAA)
		{
			//Set depth/stencil element.
			Attachments[Attachments.GetCapacity() - 1].format = FormatToVkFormat(renderPassCreateInfo.Descriptor.DepthStencilAttachment.AttachmentImage->GetFormat());
			Attachments[Attachments.GetCapacity() - 1].samples = VK_SAMPLE_COUNT_1_BIT;
			Attachments[Attachments.GetCapacity() - 1].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(renderPassCreateInfo.Descriptor.DepthStencilAttachment.LoadOperation);
			Attachments[Attachments.GetCapacity() - 1].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(renderPassCreateInfo.Descriptor.DepthStencilAttachment.StoreOperation);
			Attachments[Attachments.GetCapacity() - 1].stencilLoadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(renderPassCreateInfo.Descriptor.DepthStencilAttachment.LoadOperation);
			Attachments[Attachments.GetCapacity() - 1].stencilStoreOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(renderPassCreateInfo.Descriptor.DepthStencilAttachment.StoreOperation);
			Attachments[Attachments.GetCapacity() - 1].initialLayout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.DepthStencilAttachment.InitialLayout);
			Attachments[Attachments.GetCapacity() - 1].finalLayout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.DepthStencilAttachment.FinalLayout);
		}
	}

	const GTSL::uint8 attachments_count = renderPassCreateInfo.Descriptor.SubPasses.GetLength() * renderPassCreateInfo.Descriptor.RenderPassColorAttachments.GetLength();
	GTSL::FixedVector<VkAttachmentReference> WriteAttachmentsReferences(attachments_count, vulkanRenderDevice->GetTransientAllocationsAllocatorReference());
	GTSL::FixedVector<VkAttachmentReference> ReadAttachmentsReferences(attachments_count, vulkanRenderDevice->GetTransientAllocationsAllocatorReference());
	GTSL::FixedVector<GTSL::uint32> PreserveAttachmentsIndices(attachments_count, vulkanRenderDevice->GetTransientAllocationsAllocatorReference());
	GTSL::FixedVector<VkAttachmentReference> depth_attachment_references(renderPassCreateInfo.Descriptor.SubPasses.GetLength(), vulkanRenderDevice->GetTransientAllocationsAllocatorReference());

	GTSL::uint8 WriteAttachmentsCount = 0;
	GTSL::uint8 ReadAttachmentsCount = 0;
	GTSL::uint8 PreserveAttachmentsCount = 0;

	for (GTSL::uint8 SUBPASS = 0; SUBPASS < renderPassCreateInfo.Descriptor.SubPasses.GetLength(); ++SUBPASS)
	{
		GTSL::uint8 written_attachment_references_this_subpass_loop = 0;

		for (GTSL::uint8 ATT = 0; ATT < renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->WriteColorAttachments.GetLength(); ++ATT)
		{
			if (renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->WriteColorAttachments[ATT]->Index == ATTACHMENT_UNUSED)
			{
				WriteAttachmentsReferences[SUBPASS + ATT].attachment = VK_ATTACHMENT_UNUSED;
				WriteAttachmentsReferences[SUBPASS + ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				WriteAttachmentsReferences[SUBPASS + ATT].attachment = renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->WriteColorAttachments[ATT]->Index;
				WriteAttachmentsReferences[SUBPASS + ATT].layout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->WriteColorAttachments[ATT]->Layout);

				++WriteAttachmentsCount;
				++written_attachment_references_this_subpass_loop;
			}
		}

		for (GTSL::uint8 ATT = 0; ATT < renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->ReadColorAttachments.GetLength(); ++ATT)
		{
			if (renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->ReadColorAttachments[ATT]->Index == ATTACHMENT_UNUSED)
			{
				ReadAttachmentsReferences[SUBPASS + ATT].attachment = VK_ATTACHMENT_UNUSED;
				ReadAttachmentsReferences[SUBPASS + ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				ReadAttachmentsReferences[SUBPASS + ATT].attachment = renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->ReadColorAttachments[ATT]->Index;
				ReadAttachmentsReferences[SUBPASS + ATT].layout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->ReadColorAttachments[ATT]->Layout);

				++ReadAttachmentsCount;
				++written_attachment_references_this_subpass_loop;
			}
		}

		for (GTSL::uint8 ATT = 0; ATT < renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->PreserveAttachments.GetLength(); ++ATT)
		{
			if (renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->PreserveAttachments[ATT] == ATTACHMENT_UNUSED)
			{
				PreserveAttachmentsIndices[SUBPASS + ATT] = VK_ATTACHMENT_UNUSED;
			}
			else
			{
				PreserveAttachmentsIndices[SUBPASS + ATT] = renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->PreserveAttachments[ATT];

				PreserveAttachmentsCount++;
				++written_attachment_references_this_subpass_loop;
			}
		}

		if (renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->DepthAttachmentReference)
		{
			depth_attachment_references[SUBPASS].attachment = renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->DepthAttachmentReference->Index;
			depth_attachment_references[SUBPASS].layout = ImageLayoutToVkImageLayout(renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->DepthAttachmentReference->Layout);
		}
		else
		{
			depth_attachment_references[SUBPASS].attachment = VK_ATTACHMENT_UNUSED;
			depth_attachment_references[SUBPASS].layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	//Describe each subpass.
	GTSL::Vector<VkSubpassDescription> Subpasses(renderPassCreateInfo.Descriptor.SubPasses.GetLength(), renderPassCreateInfo.Descriptor.SubPasses.GetLength(), vulkanRenderDevice->GetTransientAllocationsAllocatorReference());
	for (GTSL::uint8 SUBPASS = 0; SUBPASS < Subpasses.GetLength(); SUBPASS++) //Loop through each subpass.
	{
		Subpasses[SUBPASS].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpasses[SUBPASS].colorAttachmentCount = WriteAttachmentsCount;
		Subpasses[SUBPASS].pColorAttachments = WriteAttachmentsReferences.GetData() + SUBPASS;
		Subpasses[SUBPASS].inputAttachmentCount = ReadAttachmentsCount;
		Subpasses[SUBPASS].pInputAttachments = ReadAttachmentsReferences.GetData() + SUBPASS;
		Subpasses[SUBPASS].pResolveAttachments = nullptr;
		Subpasses[SUBPASS].preserveAttachmentCount = 0; //PreserveAttachmentsCount;
		Subpasses[SUBPASS].pPreserveAttachments = PreserveAttachmentsIndices.GetData() + SUBPASS;
		Subpasses[SUBPASS].pDepthStencilAttachment = &depth_attachment_references[SUBPASS];
	}

	GTSL::uint8 ArrayLength = 0;
	for (GTSL::uint8 i = 0; i < renderPassCreateInfo.Descriptor.SubPasses.GetLength(); ++i)
	{
		ArrayLength += renderPassCreateInfo.Descriptor.SubPasses[i]->ReadColorAttachments.GetLength() + renderPassCreateInfo.Descriptor.SubPasses[i]->WriteColorAttachments.GetLength();
	}

	GTSL::FixedVector<VkSubpassDependency> SubpassDependencies(ArrayLength, vulkanRenderDevice->GetTransientAllocationsAllocatorReference());
	for (GTSL::uint8 SUBPASS = 0; SUBPASS < renderPassCreateInfo.Descriptor.SubPasses.GetLength(); ++SUBPASS)
	{
		for (GTSL::uint8 ATT = 0; ATT < renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->ReadColorAttachments.GetLength() + renderPassCreateInfo.Descriptor.SubPasses[SUBPASS]->WriteColorAttachments.GetLength(); ++ATT)
		{
			SubpassDependencies[SUBPASS + ATT].srcSubpass = VK_SUBPASS_EXTERNAL;
			SubpassDependencies[SUBPASS + ATT].dstSubpass = SUBPASS;
			SubpassDependencies[SUBPASS + ATT].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			SubpassDependencies[SUBPASS + ATT].srcAccessMask = 0;
			SubpassDependencies[SUBPASS + ATT].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			SubpassDependencies[SUBPASS + ATT].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			SubpassDependencies[SUBPASS + ATT].dependencyFlags = 0;
		}
	}

	VkRenderPassCreateInfo vk_renderpass_create_info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	vk_renderpass_create_info.attachmentCount = renderPassCreateInfo.Descriptor.RenderPassColorAttachments.GetLength() + DSAA;
	vk_renderpass_create_info.pAttachments = Attachments.GetData();
	vk_renderpass_create_info.subpassCount = renderPassCreateInfo.Descriptor.SubPasses.GetLength();
	vk_renderpass_create_info.pSubpasses = Subpasses.GetData();
	vk_renderpass_create_info.dependencyCount = ArrayLength;
	vk_renderpass_create_info.pDependencies = SubpassDependencies.GetData();

	vkCreateRenderPass(vulkanRenderDevice->GetVkDevice(), &vk_renderpass_create_info, vulkanRenderDevice->GetVkAllocationCallbacks(), &renderPass);
}

void VulkanRenderPass::Destroy(GAL::RenderDevice* renderDevice)
{
	auto vk_render_device = static_cast<VulkanRenderDevice*>(renderDevice);
	vkDestroyRenderPass(vk_render_device->GetVkDevice(), renderPass, vk_render_device->GetVkAllocationCallbacks());
}
