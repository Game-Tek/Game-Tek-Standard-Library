#pragma once

#include "GAL/RenderPass.h"

#include "GAL/Vulkan/Vulkan.h"
#include <GTSL/Range.h>

namespace GAL
{
	class VulkanRenderPass final : public RenderPass
	{
	public:
		VulkanRenderPass() = default;

		//Describes the reference to a render pass attachment for a sub pass.
		struct AttachmentReference
		{
			GTSL::uint8 Index = ATTACHMENT_UNUSED;
			//Layout of the attachment during the sub pass.
			VulkanTextureLayout Layout = VulkanTextureLayout::COLOR_ATTACHMENT;
		};

		struct AttachmentDescriptor
		{
			VulkanTextureFormat Format;
			//Defines the operation that should be run when the attachment is loaded for rendering.
			RenderTargetLoadOperations LoadOperation = RenderTargetLoadOperations::UNDEFINED;
			//Defines the operation that should be run when the attachment is done being rendered to.
			RenderTargetStoreOperations StoreOperation = RenderTargetStoreOperations::STORE;
			//Layout of the attachment when first used in the render pass.
			VulkanTextureLayout InitialLayout = VulkanTextureLayout::GENERAL;
			//Layout of the attachment after use in the render pass.
			VulkanTextureLayout FinalLayout = VulkanTextureLayout::GENERAL;
		};

		//Describes a subpass.
		struct SubPassDescriptor
		{
			//Array of AttachmentsReferences for attachments which the subpass reads from.
			GTSL::Range<const AttachmentReference*> ReadColorAttachments;

			//Array of AttachmentsReferences for attachments which the subpass writes to.
			GTSL::Range<const AttachmentReference*> WriteColorAttachments;

			//Array of indices identifying attachments that are not used by this subpass, but whose contents MUST be preserved throughout the subpass.
			GTSL::Range<const GTSL::uint8*> PreserveAttachments;

			AttachmentReference DepthAttachmentReference;
		};

		static constexpr uint8_t EXTERNAL = 255;
		
		struct SubPassDependency
		{
			uint8_t SourceSubPass, DestinationSubPass;
			VulkanPipelineStage::value_type SourcePipelineStage, DestinationPipelineStage;
			VulkanAccessFlags::value_type SourceAccessFlags = 0, DestinationAccessFlags;
		};
		
		struct CreateInfo final : VulkanCreateInfo
		{
			//Array of pointer to images that will be used as attachments in the render pass.
			GTSL::Range<const AttachmentDescriptor*> RenderPassAttachments;

			//Array of SubpassDescriptor used to describes the properties of every subpass in the renderpass.
			GTSL::Range<const SubPassDescriptor*> SubPasses;
			
			GTSL::Range<const SubPassDependency*> SubPassDependencies;
		};
		explicit VulkanRenderPass(const CreateInfo& createInfo);
		~VulkanRenderPass() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkRenderPass GetVkRenderPass() const { return renderPass; }
	private:
		VkRenderPass renderPass = nullptr;
	};
}
