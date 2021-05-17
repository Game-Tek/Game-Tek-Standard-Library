#pragma once

#include "RenderCore.h"

namespace GAL
{
	constexpr GTSL::uint8 ATTACHMENT_UNUSED = 255;

	class RenderPass : public GALObject
	{
	public:
		RenderPass() = default;

		//Describes the reference to a render pass attachment for a sub pass.
		struct AttachmentReference
		{
			GTSL::uint8 Index = ATTACHMENT_UNUSED;
			//Layout of the attachment during the sub pass.
			TextureLayout Layout = TextureLayout::ATTACHMENT;
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
			PipelineStage SourcePipelineStage, DestinationPipelineStage;
			AccessType SourceAccessType, DestinationAccessType;
		};
		
		~RenderPass() = default;
	};

}
