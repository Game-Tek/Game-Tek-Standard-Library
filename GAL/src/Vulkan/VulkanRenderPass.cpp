#include "GAL/Vulkan/VulkanRenderPass.h"

#include <GTSL/Vector.hpp>
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

GAL::VulkanRenderPass::VulkanRenderPass(const CreateInfo& createInfo)
{
	GTSL::Array<VkAttachmentDescription, 32> vkAttachmentDescriptions;

	for (GTSL::uint32 i = 0; i < createInfo.RenderPassAttachments.ElementCount(); ++i)
	{
		auto& attachmentDescription = vkAttachmentDescriptions.EmplaceBack();
		
		attachmentDescription.flags = 0;
		attachmentDescription.format = static_cast<VkFormat>(createInfo.RenderPassAttachments[i].Format);
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
		attachmentDescription.loadOp = ToVulkan(createInfo.RenderPassAttachments[i].LoadOperation);
		attachmentDescription.storeOp = ToVulkan(createInfo.RenderPassAttachments[i].StoreOperation);
		attachmentDescription.stencilLoadOp = vkAttachmentDescriptions[i].loadOp;
		attachmentDescription.stencilStoreOp = vkAttachmentDescriptions[i].storeOp;
		attachmentDescription.initialLayout = static_cast<VkImageLayout>(createInfo.RenderPassAttachments[i].InitialLayout);
		attachmentDescription.finalLayout = static_cast<VkImageLayout>(createInfo.RenderPassAttachments[i].FinalLayout);
	}

	GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> writeAttachmentsReferences;
	GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> readAttachmentsReferences;
	GTSL::Array<GTSL::Array<GTSL::uint32, 16>, 16> preserveAttachmentsIndices;
	GTSL::Array<VkAttachmentReference, 16> vkDepthAttachmentReferences;

	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.SubPasses.ElementCount(); ++SUBPASS)
	{
		writeAttachmentsReferences.EmplaceBack(); readAttachmentsReferences.EmplaceBack();
		preserveAttachmentsIndices.EmplaceBack(); vkDepthAttachmentReferences.EmplaceBack();
		
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

	GTSL::Array<VkSubpassDescription, 32> vkSubpassDescriptions;

	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.SubPasses.ElementCount(); ++SUBPASS)
	{
		auto& description = vkSubpassDescriptions.EmplaceBack();
		description.flags = 0;
		description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		description.colorAttachmentCount = writeAttachmentsReferences[SUBPASS].GetLength();
		description.pColorAttachments = writeAttachmentsReferences[SUBPASS].begin();
		description.inputAttachmentCount = readAttachmentsReferences[SUBPASS].GetLength();
		description.pInputAttachments = readAttachmentsReferences[SUBPASS].begin();
		description.pResolveAttachments = nullptr;
		description.preserveAttachmentCount = 0; //PreserveAttachmentsCount;
		description.pPreserveAttachments = preserveAttachmentsIndices[SUBPASS].begin();
		description.pDepthStencilAttachment = &vkDepthAttachmentReferences[SUBPASS];
	}

	GTSL::Array<VkSubpassDependency, 32> vkSubpassDependencies;
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.SubPassDependencies.ElementCount(); ++SUBPASS)
	{
		auto& dependency = vkSubpassDependencies.EmplaceBack();
		dependency.srcSubpass = createInfo.SubPassDependencies[SUBPASS].SourceSubPass == EXTERNAL ? VK_SUBPASS_EXTERNAL : createInfo.SubPassDependencies[SUBPASS].SourceSubPass;
		dependency.dstSubpass = createInfo.SubPassDependencies[SUBPASS].DestinationSubPass == EXTERNAL ? VK_SUBPASS_EXTERNAL : createInfo.SubPassDependencies[SUBPASS].DestinationSubPass;
		dependency.srcStageMask = createInfo.SubPassDependencies[SUBPASS].SourcePipelineStage;
		dependency.dstStageMask = createInfo.SubPassDependencies[SUBPASS].DestinationPipelineStage;
		dependency.srcAccessMask = createInfo.SubPassDependencies[SUBPASS].SourceAccessFlags;
		dependency.dstAccessMask = createInfo.SubPassDependencies[SUBPASS].DestinationAccessFlags;
		dependency.dependencyFlags = 0;
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