#include "GAL/Vulkan/VulkanRenderPass.h"

#include <GTSL/Vector.hpp>
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

GAL::VulkanRenderPass::VulkanRenderPass(const CreateInfo& createInfo)
{
	GTSL::Array<VkAttachmentDescription, 32> vkAttachmentDescriptions(createInfo.RenderPassAttachments.ElementCount());

	for (GTSL::uint32 i = 0; i < vkAttachmentDescriptions.GetLength(); ++i)
	{
		vkAttachmentDescriptions[i].flags = 0;
		vkAttachmentDescriptions[i].format = static_cast<VkFormat>(createInfo.RenderPassAttachments[i].Format);
		vkAttachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
		vkAttachmentDescriptions[i].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.RenderPassAttachments[i].LoadOperation);
		vkAttachmentDescriptions[i].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.RenderPassAttachments[i].StoreOperation);
		vkAttachmentDescriptions[i].stencilLoadOp = vkAttachmentDescriptions[i].loadOp;
		vkAttachmentDescriptions[i].stencilStoreOp = vkAttachmentDescriptions[i].storeOp;
		vkAttachmentDescriptions[i].initialLayout = static_cast<VkImageLayout>(createInfo.RenderPassAttachments[i].InitialLayout);
		vkAttachmentDescriptions[i].finalLayout = static_cast<VkImageLayout>(createInfo.RenderPassAttachments[i].FinalLayout);
	}

	GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> writeAttachmentsReferences(createInfo.SubPasses.ElementCount());
	GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> readAttachmentsReferences(createInfo.SubPasses.ElementCount());
	GTSL::Array<GTSL::Array<GTSL::uint32, 16>, 16> preserveAttachmentsIndices(createInfo.SubPasses.ElementCount());
	GTSL::Array<VkAttachmentReference, 16> vkDepthAttachmentReferences(createInfo.SubPasses.ElementCount());

	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.SubPasses.ElementCount(); ++SUBPASS)
	{
		for (GTSL::uint32 ATTACHMENT = 0; ATTACHMENT < createInfo.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATTACHMENT)
		{
			VkAttachmentReference attachmentReference;
			attachmentReference.attachment = createInfo.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index;
			attachmentReference.layout = static_cast<VkImageLayout>(createInfo.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Layout);

			writeAttachmentsReferences[SUBPASS].EmplaceBack(attachmentReference);
		}

		for (GTSL::uint32 A = 0; A < createInfo.SubPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++A)
		{
			VkAttachmentReference attachmentReference;
			attachmentReference.attachment = createInfo.SubPasses[SUBPASS].ReadColorAttachments[A].Index;
			attachmentReference.layout = static_cast<VkImageLayout>(createInfo.SubPasses[SUBPASS].ReadColorAttachments[A].Layout);

			readAttachmentsReferences[SUBPASS].EmplaceBack(attachmentReference);
		}

		for (GTSL::uint32 ATT = 0; ATT < createInfo.SubPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT)
		{
			preserveAttachmentsIndices[SUBPASS].EmplaceBack(createInfo.SubPasses[SUBPASS].PreserveAttachments[ATT]);
		}

		vkDepthAttachmentReferences[SUBPASS].attachment = createInfo.SubPasses[SUBPASS].DepthAttachmentReference.Index == ATTACHMENT_UNUSED ? VK_ATTACHMENT_UNUSED : createInfo.SubPasses[SUBPASS].DepthAttachmentReference.Index;
		vkDepthAttachmentReferences[SUBPASS].layout = static_cast<VkImageLayout>(createInfo.SubPasses[SUBPASS].DepthAttachmentReference.Layout);
	}

	GTSL::Array<VkSubpassDescription, 32> vkSubpassDescriptions(createInfo.SubPasses.ElementCount());

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

	GTSL::Array<VkSubpassDependency, 32> vkSubpassDependencies(createInfo.SubPassDependencies.ElementCount());
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < vkSubpassDependencies.GetLength(); ++SUBPASS)
	{
		vkSubpassDependencies[SUBPASS].srcSubpass = createInfo.SubPassDependencies[SUBPASS].SourceSubPass == EXTERNAL ? VK_SUBPASS_EXTERNAL : createInfo.SubPassDependencies[SUBPASS].SourceSubPass;
		vkSubpassDependencies[SUBPASS].dstSubpass = createInfo.SubPassDependencies[SUBPASS].DestinationSubPass == EXTERNAL ? VK_SUBPASS_EXTERNAL : createInfo.SubPassDependencies[SUBPASS].DestinationSubPass;
		vkSubpassDependencies[SUBPASS].srcStageMask = createInfo.SubPassDependencies[SUBPASS].SourcePipelineStage;
		vkSubpassDependencies[SUBPASS].dstStageMask = createInfo.SubPassDependencies[SUBPASS].DestinationPipelineStage;
		vkSubpassDependencies[SUBPASS].srcAccessMask = createInfo.SubPassDependencies[SUBPASS].SourceAccessFlags;
		vkSubpassDependencies[SUBPASS].dstAccessMask = createInfo.SubPassDependencies[SUBPASS].DestinationAccessFlags;
		vkSubpassDependencies[SUBPASS].dependencyFlags = 0;
	}

	VkRenderPassCreateInfo vkRenderpassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	vkRenderpassCreateInfo.attachmentCount = createInfo.RenderPassAttachments.ElementCount();
	vkRenderpassCreateInfo.pAttachments = vkAttachmentDescriptions.begin();
	vkRenderpassCreateInfo.subpassCount = createInfo.SubPasses.ElementCount();
	vkRenderpassCreateInfo.pSubpasses = vkSubpassDescriptions.begin();
	vkRenderpassCreateInfo.dependencyCount = vkSubpassDependencies.GetLength();
	vkRenderpassCreateInfo.pDependencies = vkSubpassDependencies.begin();

	VK_CHECK(vkCreateRenderPass(createInfo.RenderDevice->GetVkDevice(), &vkRenderpassCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &renderPass))
	SET_NAME(renderPass, VK_OBJECT_TYPE_RENDER_PASS, createInfo);
}

void GAL::VulkanRenderPass::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyRenderPass(renderDevice->GetVkDevice(), renderPass, renderDevice->GetVkAllocationCallbacks());
	debugClear(renderPass);
}

//for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.SubPasses.ElementCount(); ++SUBPASS)
//{
//	for (GTSL::uint32 ATTACHMENT = 0; ATTACHMENT < createInfo.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATTACHMENT)
//	{
//		if (createInfo.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index == ATTACHMENT_UNUSED)
//		{
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].attachment = VK_ATTACHMENT_UNUSED;
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
//		}
//		else
//		{
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].attachment = createInfo.SubPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index;
//			writeAttachmentsReferences[SUBPASS][ATTACHMENT].layout = static_cast<VkImageLayout>(createInfo.SubPasses[SUBPASS].WriteColorAttachments[ATTACH
//		}
//	}
//
//	for (GTSL::uint32 ATT = 0; ATT < createInfo.SubPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++ATT)
//	{
//		if (createInfo.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index == ATTACHMENT_UNUSED)
//		{
//			readAttachmentsReferences[SUBPASS][ATT].attachment = VK_ATTACHMENT_UNUSED;
//			readAttachmentsReferences[SUBPASS][ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
//		}
//		else
//		{
//			readAttachmentsReferences[SUBPASS][ATT].attachment = createInfo.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index;
//			readAttachmentsReferences[SUBPASS][ATT].layout = static_cast<VkImageLayout>(createInfo.SubPasses[SUBPASS].ReadColorAttachments[ATT].Layout);
//		}
//	}
//
//	for (GTSL::uint32 ATT = 0; ATT < createInfo.SubPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT)
//	{
//		if (createInfo.SubPasses[SUBPASS].PreserveAttachments[ATT] == ATTACHMENT_UNUSED)
//		{
//			preserveAttachmentsIndices[SUBPASS][ATT] = VK_ATTACHMENT_UNUSED;
//		}
//		else
//		{
//			preserveAttachmentsIndices[SUBPASS][ATT] = createInfo.SubPasses[SUBPASS].PreserveAttachments[ATT];
//		}
//	}
//
//	if (createInfo.SubPasses[SUBPASS].DepthAttachmentReference.Index == ATTACHMENT_UNUSED)
//	{
//		vkDepthAttachmentReferences[SUBPASS].attachment = VK_ATTACHMENT_UNUSED;
//		vkDepthAttachmentReferences[SUBPASS].layout = VK_IMAGE_LAYOUT_UNDEFINED;
//	}
//	else
//	{
//		vkDepthAttachmentReferences[SUBPASS].attachment = createInfo.SubPasses[SUBPASS].DepthAttachmentReference.Index;
//		vkDepthAttachmentReferences[SUBPASS].layout = static_cast<VkImageLayout>(createInfo.SubPasses[SUBPASS].DepthAttachmentReference.Layout);
//	}
//}