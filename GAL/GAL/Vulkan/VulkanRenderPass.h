#pragma once

#include "GAL/RenderPass.h"

#include "GAL/Vulkan/Vulkan.h"

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
			GTSL::uint32 Format{ 0 };
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
			GTSL::Ranger<AttachmentReference> ReadColorAttachments;

			//Array of AttachmentsReferences for attachments which the subpass writes to.
			GTSL::Ranger<AttachmentReference> WriteColorAttachments;

			//Array of indices identifying attachments that are not used by this subpass, but whose contents MUST be preserved throughout the subpass.
			GTSL::Ranger<GTSL::uint8> PreserveAttachments;

			AttachmentReference* DepthAttachmentReference = nullptr;
		};

		//Describes a render pass.
		struct RenderPassDescriptor
		{
			//Array of pointer to images that will be used as attachments in the render pass.
			GTSL::Ranger<AttachmentDescriptor> RenderPassColorAttachments;
			//Pointer to an image that will be used as the depth stencil attachment in the render pass.
			AttachmentDescriptor DepthStencilAttachment;
			bool DepthStencilAttachmentAvailable = false;

			//Array of SubpassDescriptor used to describes the properties of every subpass in the renderpass.
			GTSL::Ranger<SubPassDescriptor> SubPasses;
		};
		
		struct CreateInfo final : VulkanCreateInfo
		{
			RenderPassDescriptor Descriptor;
		};
		explicit VulkanRenderPass(const CreateInfo& createInfo);
		~VulkanRenderPass() = default;

		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkRenderPass GetVkRenderPass() const { return renderPass; }
	private:
		VkRenderPass renderPass = nullptr;
	};
}