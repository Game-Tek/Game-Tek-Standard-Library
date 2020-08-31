#include "GAL/Vulkan/VulkanRenderPass.h"

#include <GTSL/Vector.hpp>
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include "GTSL/Array.hpp"

GAL::VulkanRenderPass::VulkanRenderPass(const CreateInfo& createInfo)
{
	const bool depthAttachment = createInfo.Descriptor.DepthStencilAttachmentAvailable;

	GTSL::Array<VkAttachmentDescription, 64> vkAttachmentDescriptions(createInfo.Descriptor.RenderPassColorAttachments.ElementCount() + depthAttachment);

	for (GTSL::uint32 i = 0; i < vkAttachmentDescriptions.GetLength() - depthAttachment; ++i)
	{
		vkAttachmentDescriptions[i].flags = 0;
		vkAttachmentDescriptions[i].format = static_cast<VkFormat>(createInfo.Descriptor.RenderPassColorAttachments[i].Format);
		vkAttachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
		vkAttachmentDescriptions[i].loadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.Descriptor.RenderPassColorAttachments[i].LoadOperation);
		vkAttachmentDescriptions[i].storeOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.Descriptor.RenderPassColorAttachments[i].StoreOperation);
		vkAttachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vkAttachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vkAttachmentDescriptions[i].initialLayout = static_cast<VkImageLayout>(createInfo.Descriptor.RenderPassColorAttachments[i].InitialLayout);
		vkAttachmentDescriptions[i].finalLayout = static_cast<VkImageLayout>(createInfo.Descriptor.RenderPassColorAttachments[i].FinalLayout);
	}
	if (depthAttachment)
	{
		//Set depth/stencil element.
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].flags = 0;
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].format = static_cast<VkFormat>(createInfo.Descriptor.DepthStencilAttachment.Format);
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].samples = VK_SAMPLE_COUNT_1_BIT;
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].stencilLoadOp = RenderTargetLoadOperationsToVkAttachmentLoadOp(createInfo.Descriptor.DepthStencilAttachment.LoadOperation);
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].stencilStoreOp = RenderTargetStoreOperationsToVkAttachmentStoreOp(createInfo.Descriptor.DepthStencilAttachment.StoreOperation);
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].initialLayout = static_cast<VkImageLayout>(createInfo.Descriptor.DepthStencilAttachment.InitialLayout);
		vkAttachmentDescriptions[vkAttachmentDescriptions.GetLength() - 1].finalLayout = static_cast<VkImageLayout>(createInfo.Descriptor.DepthStencilAttachment.FinalLayout);
	}


	const GTSL::uint8 attachmentsCount = createInfo.Descriptor.SubPasses.ElementCount() * createInfo.Descriptor.RenderPassColorAttachments.ElementCount();
	GTSL::Array<VkAttachmentReference, 64> writeAttachmentsReferences(attachmentsCount);
	GTSL::Array<VkAttachmentReference, 64> readAttachmentsReferences(attachmentsCount);
	GTSL::Array<GTSL::uint32, 64> preserveAttachmentsIndices(attachmentsCount);
	GTSL::Array<VkAttachmentReference, 64> vkDepthAttachmentReferences(createInfo.Descriptor.SubPasses.ElementCount());

	GTSL::uint8 writeAttachmentsCount = 0;
	GTSL::uint8 readAttachmentsCount = 0;
	GTSL::uint8 preserveAttachmentsCount = 0;

	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
	{
		GTSL::uint32 writtenAttachmentReferencesThisSubpassLoop = 0;

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATT)
		{
			if (createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATT].Index == ATTACHMENT_UNUSED)
			{
				writeAttachmentsReferences[SUBPASS + ATT].attachment = VK_ATTACHMENT_UNUSED;
				writeAttachmentsReferences[SUBPASS + ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				writeAttachmentsReferences[SUBPASS + ATT].attachment = createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATT].Index;
				writeAttachmentsReferences[SUBPASS + ATT].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments[ATT].Layout);

				++writeAttachmentsCount;
				++writtenAttachmentReferencesThisSubpassLoop;
			}
		}

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++ATT)
		{
			if (createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index == ATTACHMENT_UNUSED)
			{
				readAttachmentsReferences[SUBPASS + ATT].attachment = VK_ATTACHMENT_UNUSED;
				readAttachmentsReferences[SUBPASS + ATT].layout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				readAttachmentsReferences[SUBPASS + ATT].attachment = createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Index;
				readAttachmentsReferences[SUBPASS + ATT].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments[ATT].Layout);

				++readAttachmentsCount;
				++writtenAttachmentReferencesThisSubpassLoop;
			}
		}

		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT)
		{
			if (createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT] == ATTACHMENT_UNUSED)
			{
				preserveAttachmentsIndices[SUBPASS + ATT] = VK_ATTACHMENT_UNUSED;
			}
			else
			{
				preserveAttachmentsIndices[SUBPASS + ATT] = createInfo.Descriptor.SubPasses[SUBPASS].PreserveAttachments[ATT];

				++preserveAttachmentsCount;
				++writtenAttachmentReferencesThisSubpassLoop;
			}
		}

		if (createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference)
		{
			vkDepthAttachmentReferences[SUBPASS].attachment = createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference->Index;
			vkDepthAttachmentReferences[SUBPASS].layout = static_cast<VkImageLayout>(createInfo.Descriptor.SubPasses[SUBPASS].DepthAttachmentReference->Layout);
		}
		else
		{
			vkDepthAttachmentReferences[SUBPASS].attachment = VK_ATTACHMENT_UNUSED;
			vkDepthAttachmentReferences[SUBPASS].layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	GTSL::Array<VkSubpassDescription, 64> vkSubpassDescriptions(createInfo.Descriptor.SubPasses.ElementCount());
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < vkSubpassDescriptions.GetLength(); ++SUBPASS)
	{
		vkSubpassDescriptions[SUBPASS].flags = 0;
		vkSubpassDescriptions[SUBPASS].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vkSubpassDescriptions[SUBPASS].colorAttachmentCount = writeAttachmentsCount;
		vkSubpassDescriptions[SUBPASS].pColorAttachments = writeAttachmentsReferences.begin() + SUBPASS;
		vkSubpassDescriptions[SUBPASS].inputAttachmentCount = readAttachmentsCount;
		vkSubpassDescriptions[SUBPASS].pInputAttachments = readAttachmentsReferences.begin() + SUBPASS;
		vkSubpassDescriptions[SUBPASS].pResolveAttachments = nullptr;
		vkSubpassDescriptions[SUBPASS].preserveAttachmentCount = 0; //PreserveAttachmentsCount;
		vkSubpassDescriptions[SUBPASS].pPreserveAttachments = preserveAttachmentsIndices.begin() + SUBPASS;
		vkSubpassDescriptions[SUBPASS].pDepthStencilAttachment = &vkDepthAttachmentReferences[SUBPASS];
	}

	GTSL::uint32 arrayLength = 0;	
	for (GTSL::uint32 i = 0; i < createInfo.Descriptor.SubPasses.ElementCount(); ++i)
	{
		arrayLength += createInfo.Descriptor.SubPasses[i].ReadColorAttachments.ElementCount() + createInfo.Descriptor.SubPasses[i].WriteColorAttachments.ElementCount();
	}

	GTSL::Array<VkSubpassDependency, 64> vkSubpassDependencies(arrayLength);
	for (GTSL::uint32 SUBPASS = 0; SUBPASS < createInfo.Descriptor.SubPasses.ElementCount(); ++SUBPASS)
	{
		for (GTSL::uint32 ATT = 0; ATT < createInfo.Descriptor.SubPasses[SUBPASS].ReadColorAttachments.ElementCount() + createInfo.Descriptor.SubPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATT)
		{
			vkSubpassDependencies[SUBPASS + ATT].srcSubpass = VK_SUBPASS_EXTERNAL;
			vkSubpassDependencies[SUBPASS + ATT].dstSubpass = SUBPASS;
			vkSubpassDependencies[SUBPASS + ATT].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vkSubpassDependencies[SUBPASS + ATT].srcAccessMask = 0;
			vkSubpassDependencies[SUBPASS + ATT].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vkSubpassDependencies[SUBPASS + ATT].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vkSubpassDependencies[SUBPASS + ATT].dependencyFlags = 0;
		}
	}

	VkRenderPassCreateInfo vkRenderpassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	vkRenderpassCreateInfo.attachmentCount = createInfo.Descriptor.RenderPassColorAttachments.ElementCount() + depthAttachment;
	vkRenderpassCreateInfo.pAttachments = vkAttachmentDescriptions.begin();
	vkRenderpassCreateInfo.subpassCount = createInfo.Descriptor.SubPasses.ElementCount();
	vkRenderpassCreateInfo.pSubpasses = vkSubpassDescriptions.begin();
	vkRenderpassCreateInfo.dependencyCount = arrayLength;
	vkRenderpassCreateInfo.pDependencies = vkSubpassDependencies.begin();

	VK_CHECK(vkCreateRenderPass(createInfo.RenderDevice->GetVkDevice(), &vkRenderpassCreateInfo, createInfo.RenderDevice->GetVkAllocationCallbacks(), &renderPass));
	SET_NAME(renderPass, VK_OBJECT_TYPE_RENDER_PASS, createInfo);
}

void GAL::VulkanRenderPass::Destroy(const VulkanRenderDevice* renderDevice)
{
	vkDestroyRenderPass(renderDevice->GetVkDevice(), renderPass, renderDevice->GetVkAllocationCallbacks());
	debugClear(renderPass);
}
