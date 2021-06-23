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
		
		void Initialize(const VulkanRenderDevice* renderDevice, GTSL::Range<const RenderPassTargetDescription*> attachments,
			GTSL::Range<const SubPassDescriptor*> subPasses, const GTSL::Range<const SubPassDependency*> subPassDependencies) {
			GTSL::Array<VkAttachmentDescription, 32> vkAttachmentDescriptions;

			for (GTSL::uint32 i = 0; i < static_cast<GTSL::uint32>(attachments.ElementCount()); ++i) {
				auto& attachmentDescription = vkAttachmentDescriptions.EmplaceBack();

				attachmentDescription.flags = 0;
				attachmentDescription.format = ToVulkan(MakeFormatFromFormatDescriptor(attachments[i].FormatDescriptor));
				attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT; //TODO: Should match that of the SwapChain images.
				attachmentDescription.loadOp = ToVkAttachmentLoadOp(attachments[i].LoadOperation);
				attachmentDescription.storeOp = ToVkAttachmentStoreOp(attachments[i].StoreOperation);
				attachmentDescription.stencilLoadOp = vkAttachmentDescriptions[i].loadOp;
				attachmentDescription.stencilStoreOp = vkAttachmentDescriptions[i].storeOp;
				attachmentDescription.initialLayout = ToVulkan(attachments[i].Start, attachments[i].FormatDescriptor);
				attachmentDescription.finalLayout = ToVulkan(attachments[i].End, attachments[i].FormatDescriptor);
			}

			GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> writeAttachmentsReferences;
			GTSL::Array<GTSL::Array<VkAttachmentReference, 16>, 16> readAttachmentsReferences;
			GTSL::Array<GTSL::Array<GTSL::uint32, 16>, 16> preserveAttachmentsIndices;
			GTSL::Array<VkAttachmentReference, 16> vkDepthAttachmentReferences;

			for (GTSL::uint32 subpass = 0; subpass < static_cast<GTSL::uint32>(subPasses.ElementCount()); ++subpass) {
				writeAttachmentsReferences.EmplaceBack(); readAttachmentsReferences.EmplaceBack();
				preserveAttachmentsIndices.EmplaceBack();

				auto& depthAttachment = vkDepthAttachmentReferences.EmplaceBack();
				depthAttachment.attachment = VK_ATTACHMENT_UNUSED; depthAttachment.layout = VK_IMAGE_LAYOUT_UNDEFINED;

				for (GTSL::uint32 a = 0; a < static_cast<GTSL::uint32>(subPasses[subpass].WriteAttachments.ElementCount()); ++a) {
					if (attachments[subPasses[subpass].WriteAttachments[a].Index].FormatDescriptor.Type == TextureType::COLOR) {
						VkAttachmentReference attachmentReference;
						attachmentReference.attachment = subPasses[subpass].WriteAttachments[a].Index;
						attachmentReference.layout = ToVulkan(subPasses[subpass].WriteAttachments[a].Layout, attachments[subPasses[subpass].WriteAttachments[a].Index].FormatDescriptor);
						writeAttachmentsReferences[subpass].EmplaceBack(attachmentReference);
					} else {
						depthAttachment.attachment = subPasses[subpass].WriteAttachments[a].Index;
						depthAttachment.layout = ToVulkan(subPasses[subpass].WriteAttachments[a].Layout, attachments[subPasses[subpass].WriteAttachments[a].Index].FormatDescriptor);
					}
				}

				for (GTSL::uint32 a = 0; a < static_cast<GTSL::uint32>(subPasses[subpass].ReadAttachments.ElementCount()); ++a) {
					if (attachments[subPasses[subpass].ReadAttachments[a].Index].FormatDescriptor.Type == TextureType::COLOR) {
						VkAttachmentReference attachmentReference;
						attachmentReference.attachment = subPasses[subpass].ReadAttachments[a].Index;
						attachmentReference.layout = ToVulkan(subPasses[subpass].ReadAttachments[a].Layout, attachments[subPasses[subpass].ReadAttachments[a].Index].FormatDescriptor);
						readAttachmentsReferences[subpass].EmplaceBack(attachmentReference);
					} else {
						depthAttachment.attachment = subPasses[subpass].ReadAttachments[a].Index;
						depthAttachment.layout = ToVulkan(subPasses[subpass].ReadAttachments[a].Layout, attachments[subPasses[subpass].ReadAttachments[a].Index].FormatDescriptor);
					}
				}

				for (GTSL::uint32 a = 0; a < static_cast<GTSL::uint32>(subPasses[subpass].PreserveAttachments.ElementCount()); ++a) {
					preserveAttachmentsIndices[subpass].EmplaceBack(subPasses[subpass].PreserveAttachments[a]);
				}
			}

			GTSL::Array<VkSubpassDescription, 32> vkSubpassDescriptions;

			for (GTSL::uint32 SUBPASS = 0; SUBPASS < static_cast<GTSL::uint32>(subPasses.ElementCount()); ++SUBPASS) {
				auto& description = vkSubpassDescriptions.EmplaceBack();
				description.flags = 0;
				description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				description.colorAttachmentCount = writeAttachmentsReferences[SUBPASS].GetLength();
				description.pColorAttachments = writeAttachmentsReferences[SUBPASS].begin();
				description.inputAttachmentCount = readAttachmentsReferences[SUBPASS].GetLength();
				description.pInputAttachments = readAttachmentsReferences[SUBPASS].begin();
				description.pResolveAttachments = nullptr;
				description.preserveAttachmentCount = preserveAttachmentsIndices[SUBPASS].GetLength();
				description.pPreserveAttachments = preserveAttachmentsIndices[SUBPASS].begin();
				description.pDepthStencilAttachment = &vkDepthAttachmentReferences[SUBPASS];
			}

			GTSL::Array<VkSubpassDependency, 32> vkSubpassDependencies;
			for (GTSL::uint32 SUBPASS = 0; SUBPASS < static_cast<GTSL::uint32>(subPassDependencies.ElementCount()); ++SUBPASS) {
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
			vkRenderpassCreateInfo.attachmentCount = static_cast<GTSL::uint32>(attachments.ElementCount());
			vkRenderpassCreateInfo.pAttachments = vkAttachmentDescriptions.begin();
			vkRenderpassCreateInfo.subpassCount = static_cast<GTSL::uint32>(subPasses.ElementCount());
			vkRenderpassCreateInfo.pSubpasses = vkSubpassDescriptions.begin();
			vkRenderpassCreateInfo.dependencyCount = vkSubpassDependencies.GetLength();
			vkRenderpassCreateInfo.pDependencies = vkSubpassDependencies.begin();

			renderDevice->VkCreateRenderPass(renderDevice->GetVkDevice(), &vkRenderpassCreateInfo, renderDevice->GetVkAllocationCallbacks(), &renderPass);
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
