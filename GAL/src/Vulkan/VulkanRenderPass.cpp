#include "GAL/Vulkan/VulkanRenderPass.h"

#include <GTSL/Vector.hpp>
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

GAL::VulkanRenderPass::VulkanRenderPass(const CreateInfo& createInfo)
{
	GTSL::Array<VkAttachmentDescription, 32> vkAttachmentDescriptions(createInfo.Descriptor.RenderPassAttachments.ElementCount());

	for (GTSL::uint32 i = 0; i < vkAttachmentDescriptions.GetLength(); ++i)
	{
		vkAttachmentDescriptions[i].flags = 0;
		vkAttachmentDescriptions[i].format = static_cast<VkFormat>(createInfo.Descriptor.RenderPassAttachments[i].Format);
		vkAttachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
		vkAttachmentDescriptions[i].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.Descriptor.RenderPassAttachments[i].LoadOperation);
		vkAttachmentDescriptions[i].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.Descriptor.RenderPassAttachments[i].StoreOperation);
		vkAttachmentDescriptions[i].stencilLoadOp = vkAttachmentDescriptions[i].loadOp;
		vkAttachmentDescriptions[i].stencilStoreOp = vkAttachmentDescriptions[i].storeOp;
		vkAttachmentDescriptions[i].initialLayout = static_cast<VkImageLayout>(createInfo.Descriptor.RenderPassAttachments[i].InitialLayout);
		vkAttachmentDescriptions[i].finalLayout = static_cast<VkImageLayout>(createInfo.Descriptor.RenderPassAttachments[i].FinalLayout);
	}

	GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> writeAttachmentsReferences(createInfo.Descriptor.SubPasses.ElementCount());
	GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> readAttachmentsReferences(createInfo.Descriptor.SubPasses.ElementCount());
	GTSL::Array<GTSL::Array<GTSL::uint32, 16>, 16> preserveAttachmentsIndices(createInfo.Descriptor.SubPasses.ElementCount());
	GTSL::Array<VkAttachmentReference, 16> vkDepthAttachmentReferences(createInfo.Descriptor.SubPasses.ElementCount());

	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
	{
		for (GTSL::uint32 ATTACHMENT = 0; ATTACHMENT < createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATTACHMENT)
		{
			VkAttachmentReference attachmentReference;
			attachmentReference.attachment = createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index;
			attachmentReference.layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Layout);
			
			writeAttachmentsReferences[SUBPASS].EmplaceBack(attachmentReference);
		}

		for (GTSL::uint32 A = 0; A < createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++A)
		{
			VkAttachmentReference attachmentReference;
			attachmentReference.attachment = createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[A].Index;
			attachmentReference.layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[A].Layout);

			readAttachmentsReferences[SUBPASS].EmplaceBack(attachmentReference);
		}

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT)
		{
			preserveAttachmentsIndices[SUBPASS].EmplaceBack(createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT]);
		}

		vkDepthAttachmentReferences[SUBPASS].attachment = createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference.Index == ATTACHMENT_UNUSED  ? VK_ATTACHMENT_UNUSED : createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference.Index;
		vkDepthAttachmentReferences[SUBPASS].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference.Layout);
	}

	GTSL::Array<VkSubpassDescription, 32> vkSubpassDescriptions(createInfo.Descriptor.SubPasses.ElementCount());
	
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < vkSubpassDescriptions.GetLength(); ++SUBPASS)
	{
		vkSubpassDescriptions[SUBPASS].flags = 0;
		vkSubpassDescriptions[SUBPASS].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vkSubpassDescriptions[SUBPASS].colorAttachmentCount = writeAttachmentsReferences[SUBPASS].GetLength();
		vkSubpassDescriptions[SUBPASS].pColorAttachments = writeAttachmentsReferences[SUBPASS].begin();
		vkSubpassDescriptions[SUBPASS].inputAttachmentCount = readAttachmentsReferences[SUBPASS].GetLength();
		vkSubpassDescriptions[SUBPASS].pInputAttachments = readAttachmentsReferences[SUBPASS].begin();
		vkSubpassDescriptions[SUBPASS].pResolveAttachments = nullptr;
		vkSubpassDescriptions[SUBPASS].preserveAttachmentCount = 0; //PreserveAttachmentsCount;
		vkSubpassDescriptions[SUBPASS].pPreserveAttachments = preserveAttachmentsIndices[SUBPASS].begin();
		vkSubpassDescriptions[SUBPASS].pDepthStencilAttachment = &vkDepthAttachmentReferences[SUBPASS];
	}

	GTSL::Array<VkSubpassDependency, 32> vkSubpassDependencies(createInfo.Descriptor.SubPasses.ElementCount());
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
	{
		vkSubpassDependencies[SUBPASS].srcSubpass = VK_SUBPASS_EXTERNAL;
		vkSubpassDependencies[SUBPASS].dstSubpass = SUBPASS;
		vkSubpassDependencies[SUBPASS].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSubpassDependencies[SUBPASS].srcAccessMask = 0;
		vkSubpassDependencies[SUBPASS].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSubpassDependencies[SUBPASS].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		vkSubpassDependencies[SUBPASS].dependencyFlags = 0;
	}

	VkRenderPassCreateInfo vkRenderpassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	vkRenderpassCreateInfo.attachmentCount = createInfo.Descriptor.RenderPassAttachments.ElementCount();
	vkRenderpassCreateInfo.pAttachments = vkAttachmentDescriptions.begin();
	vkRenderpassCreateInfo.subpassCount = createInfo.Descriptor.SubPasses.ElementCount();
	vkRenderpassCreateInfo.pSubpasses = vkSubpassDescriptions.begin();
	vkRenderpassCreateInfo.dependencyCount = vkSubpassDependencies.GetLength();
	vkRenderpassCreateInfo.pDependencies = vkSubpassDependencies.begin();

	VK_CHECK(vkCreateRenderPass(createInfo.RenderDevice->GetVkDevice(), &vkRenderpassCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &renderPass));
	SET_NAME(renderPass, VK_OBJECT_TYPE_RENDER_PASS, createInfo);
}

void GAL::VulkanRenderPass::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyRenderPass(renderDevice->GetVkDevice(), renderPass, renderDevice->GetVkAllocationCallbacks());
	debugClear(renderPass);
}

//for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
//{
//	for (GTSL::uint32 ATTACHMENT = 0; ATTACHMENT < createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATTACHMENT)
//	{
//		if (createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index == ATTACHMENT_UNUSED)
//		{
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].attachment = VK_ATTACHMENT_UNUSED;
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
//		}
//		else
//		{
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].attachment = createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index;
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATTACH
//		}
//	}
//
//	for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++ATT)
//	{
//		if (createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index == ATTACHMENT_UNUSED)
//		{
//			readAttachmentsReferences[SUBPASS][ATT].attachment = VK_ATTACHMENT_UNUSED;
//			readAttachmentsReferences[SUBPASS][ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
//		}
//		else
//		{
//			readAttachmentsReferences[SUBPASS][ATT].attachment = createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index;
//			readAttachmentsReferences[SUBPASS][ATT].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Layout);
//		}
//	}
//
//	for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT)
//	{
//		if (createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT] == ATTACHMENT_UNUSED)
//		{
//			preserveAttachmentsIndices[SUBPASS][ATT] = VK_ATTACHMENT_UNUSED;
//		}
//		else
//		{
//			preserveAttachmentsIndices[SUBPASS][ATT] = createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT];
//		}
//	}
//
//	if (createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference.Index == ATTACHMENT_UNUSED)
//	{
//		vkDepthAttachmentReferences[SUBPASS].attachment = VK_ATTACHMENT_UNUSED;
//		vkDepthAttachmentReferences[SUBPASS].layout = VK_IMAGE_LAYOUT_UNDEFINED;
//	}
//	else
//	{
//		vkDepthAttachmentReferences[SUBPASS].attachment = createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference.Index;
//		vkDepthAttachmentReferences[SUBPASS].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference.Layout);
//	}
//}