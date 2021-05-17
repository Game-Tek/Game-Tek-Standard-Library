#pragma once

#include "GAL/RenderPass.h"

#include "GAL/Vulkan/Vulkan.h"
#include "GAL/Vulkan/VulkanRenderDevice.h"
#include <GTSL/Range.h>
#include <GTSL/Array.hpp>

namespace GAL
{
	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass() = default;
		
		void Initialize(const VulkanRenderDevice* renderDevice, GTSL::Range<const RenderPassTargetDescription*> renderPassAttachments,
			GTSL::Range<const SubPassDescriptor*> subPasses, const GTSL::Range<const SubPassDependency*> subPassDependencies) {
			GTSL::Array<VkAttachmentDescription, 32> vkAttachmentDescriptions;

			for (GTSL::uint32 i = 0; i < renderPassAttachments.ElementCount(); ++i) {
				auto& attachmentDescription = vkAttachmentDescriptions.EmplaceBack();

				attachmentDescription.flags = 0;
				attachmentDescription.format = ToVulkan(MakeFormatFromFormatDescriptor(renderPassAttachments[i].FormatDescriptor));
				attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
				attachmentDescription.loadOp = ToVkAttachmentLoadOp(renderPassAttachments[i].LoadOperation);
				attachmentDescription.storeOp = ToVkAttachmentStoreOp(renderPassAttachments[i].StoreOperation);
				attachmentDescription.stencilLoadOp = vkAttachmentDescriptions[i].loadOp;
				attachmentDescription.stencilStoreOp = vkAttachmentDescriptions[i].storeOp;
				attachmentDescription.initialLayout = ToVulkan(renderPassAttachments[i].Start, renderPassAttachments[i].FormatDescriptor);
				attachmentDescription.finalLayout = ToVulkan(renderPassAttachments[i].End, renderPassAttachments[i].FormatDescriptor);
			}

			GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> writeAttachmentsReferences;
			GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> readAttachmentsReferences;
			GTSL::Array<GTSL::Array<GTSL::uint32, 16>, 16> preserveAttachmentsIndices;
			GTSL::Array<VkAttachmentReference, 16> vkDepthAttachmentReferences;

			for (GTSL::uint32 SUBPASS = 0; SUBPASS < subPasses.ElementCount(); ++SUBPASS) {
				writeAttachmentsReferences.EmplaceBack(); readAttachmentsReferences.EmplaceBack();
				preserveAttachmentsIndices.EmplaceBack(); vkDepthAttachmentReferences.EmplaceBack();

				for (GTSL::uint32 ATTACHMENT = 0; ATTACHMENT < subPasses[SUBPASS].WriteColorAttachments.ElementCount(); ++ATTACHMENT) {
					VkAttachmentReference attachmentReference;
					attachmentReference.attachment = subPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index;
					attachmentReference.layout = ToVulkan(subPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Layout, renderPassAttachments[subPasses[SUBPASS].WriteColorAttachments[ATTACHMENT].Index].FormatDescriptor);

					writeAttachmentsReferences[SUBPASS].EmplaceBack(attachmentReference);
				}

				for (GTSL::uint32 A = 0; A < subPasses[SUBPASS].ReadColorAttachments.ElementCount(); ++A) {
					VkAttachmentReference attachmentReference;
					attachmentReference.attachment = subPasses[SUBPASS].ReadColorAttachments[A].Index;
					attachmentReference.layout = ToVulkan(subPasses[SUBPASS].ReadColorAttachments[A].Layout, renderPassAttachments[subPasses[SUBPASS].ReadColorAttachments[A].Index].FormatDescriptor);

					readAttachmentsReferences[SUBPASS].EmplaceBack(attachmentReference);
				}

				for (GTSL::uint32 ATT = 0; ATT < subPasses[SUBPASS].PreserveAttachments.ElementCount(); ++ATT) {
					preserveAttachmentsIndices[SUBPASS].EmplaceBack(subPasses[SUBPASS].PreserveAttachments[ATT]);
				}

				if (subPasses[SUBPASS].DepthAttachmentReference.Index != ATTACHMENT_UNUSED) {
					vkDepthAttachmentReferences[SUBPASS].attachment = subPasses[SUBPASS].DepthAttachmentReference.Index;
					vkDepthAttachmentReferences[SUBPASS].layout = ToVulkan(subPasses[SUBPASS].DepthAttachmentReference.Layout, renderPassAttachments[subPasses[SUBPASS].DepthAttachmentReference.Index].FormatDescriptor);
				} else {
					vkDepthAttachmentReferences[SUBPASS].attachment = VK_ATTACHMENT_UNUSED;
					vkDepthAttachmentReferences[SUBPASS].layout = VK_IMAGE_LAYOUT_UNDEFINED;
				}
			}

			GTSL::Array<VkSubpassDescription, 32> vkSubpassDescriptions;

			for (GTSL::uint32 SUBPASS = 0; SUBPASS < subPasses.ElementCount(); ++SUBPASS) {
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
			for (GTSL::uint32 SUBPASS = 0; SUBPASS < subPassDependencies.ElementCount(); ++SUBPASS) {
				auto& dependency = vkSubpassDependencies.EmplaceBack();
				dependency.srcSubpass = subPassDependencies[SUBPASS].SourceSubPass == EXTERNAL ? VK_SUBPASS_EXTERNAL : subPassDependencies[SUBPASS].SourceSubPass;
				dependency.dstSubpass = subPassDependencies[SUBPASS].DestinationSubPass == EXTERNAL ? VK_SUBPASS_EXTERNAL : subPassDependencies[SUBPASS].DestinationSubPass;
				dependency.srcStageMask = ToVulkan(subPassDependencies[SUBPASS].SourcePipelineStage);
				dependency.dstStageMask = ToVulkan(subPassDependencies[SUBPASS].DestinationPipelineStage);
				//dependency.srcAccessMask = ToVulkan(subPassDependencies[SUBPASS].SourceAccessType, subPassDependencies);
				//dependency.dstAccessMask = ToVulkan(subPassDependencies[SUBPASS].DestinationAccessFlags);
				dependency.dependencyFlags = 0;
			}

			VkRenderPassCreateInfo vkRenderpassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
			vkRenderpassCreateInfo.attachmentCount = renderPassAttachments.ElementCount();
			vkRenderpassCreateInfo.pAttachments = vkAttachmentDescriptions.begin();
			vkRenderpassCreateInfo.subpassCount = subPasses.ElementCount();
			vkRenderpassCreateInfo.pSubpasses = vkSubpassDescriptions.begin();
			vkRenderpassCreateInfo.dependencyCount = vkSubpassDependencies.GetLength();
			vkRenderpassCreateInfo.pDependencies = vkSubpassDependencies.begin();

			VK_CHECK(renderDevice->VkCreateRenderPass(renderDevice->GetVkDevice(), &vkRenderpassCreateInfo, renderDevice->GetVkAllocationCallbacks(), &renderPass))
			//setName(createInfo.RenderDevice, renderPass, VK_OBJECT_TYPE_RENDER_PASS, createInfo.Name);
		}
		

		void Destroy(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkDestroyRenderPass(renderDevice->GetVkDevice(), renderPass, renderDevice->GetVkAllocationCallbacks());
			debugClear(renderPass);
		}
		
		~VulkanRenderPass() = default;

		[[nodiscard]] VkRenderPass GetVkRenderPass() const { return renderPass; }
		[[nodiscard]] uint64_t GetHandle() const { return reinterpret_cast<uint64_t>(renderPass); }
	private:
		VkRenderPass renderPass = nullptr;
	};
}
