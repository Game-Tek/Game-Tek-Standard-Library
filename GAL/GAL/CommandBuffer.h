#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/Ranger.h>
#include <GTSL/RGB.h>

namespace GAL
{
	class BindingsSet;
	class RenderPass;
	class Framebuffer;
	struct RenderPassBeginInfo;
	struct DrawInfo;
	class Pipeline;
	class GraphicsPipeline;
	class ComputePipeline;
	struct PushConstantsInfo;
	struct BindBindingsSet;
	class RenderMesh;
	class Queue;
	
	class CommandBuffer : public GALObject
	{
	public:
		CommandBuffer() = default;
		~CommandBuffer() = default;

		struct CreateInfo : RenderInfo
		{
			bool IsPrimary = true;
		};
		
		struct BeginRecordingInfo : RenderInfo
		{
			/**
			 * \brief Pointer to primary/parent command buffer, can be null if this command buffer is primary/has no children.
			 */
			CommandBuffer* PrimaryCommandBuffer{ nullptr };
		};
		//Starts recording of commands.

		
		struct EndRecordingInfo : RenderInfo
		{
		};
		//Ends recording of commands.

		// COMMANDS

		//  BIND COMMANDS
		//    BIND BUFFER COMMANDS

		//Adds a BindMesh command to the command queue.
		struct BindMeshInfo : RenderInfo
		{
			RenderMesh* Mesh = nullptr;
		};

		struct BindIndexBufferInfo final : RenderInfo
		{
			const class Buffer* Buffer{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};

		struct BindVertexBufferInfo final : RenderInfo
		{
			const class Buffer* Buffer{ nullptr };
			GTSL::uint32 Offset{ 0 };
		};
		
		//    BIND PIPELINE COMMANDS

		//Adds a BindBindingsSet to the command queue.

		struct BindBindingsSetInfo : RenderInfo
		{
			GTSL::Ranger<BindingsSet> BindingsSets;
			GTSL::Ranger<GTSL::uint32> Offsets;
			Pipeline* Pipeline = nullptr;
			GTSL::uint8 BindingsSetIndex = 0;
		};

		struct UpdatePushConstantsInfo : RenderInfo
		{
			GraphicsPipeline* Pipeline = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};

		struct BindGraphicsPipelineInfo : RenderInfo
		{
			GraphicsPipeline* GraphicsPipeline = nullptr;
			GTSL::Extent2D RenderExtent;
		};
		//Adds a BindGraphicsPipeline command to the command queue.

		struct BindComputePipelineInfo : RenderInfo
		{
			ComputePipeline* Pipeline = nullptr;
		};
		//Adds a BindComputePipeline to the command queue.


		//  DRAW COMMANDS

		//Adds a DrawIndexed command to the command queue.
		struct DrawIndexedInfo : RenderInfo
		{
			GTSL::uint32 IndexCount = 0;
			GTSL::uint32 InstanceCount = 0;
		};

		//  COMPUTE COMMANDS

		struct DispatchInfo : RenderInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		//Adds a Dispatch command to the command queue.

		//  RENDER PASS COMMANDS

		struct BeginRenderPassInfo : RenderInfo
		{
			RenderPass* RenderPass = nullptr;
			Framebuffer* Framebuffer = nullptr;
			GTSL::Extent2D RenderArea;
			GTSL::Ranger<GTSL::RGBA> ClearValues;
		};
		//Adds a BeginRenderPass command to the command queue.

		struct AdvanceSubpassInfo : RenderInfo
		{
		};
		//Adds a AdvanceSubPass command to the command buffer.

		struct EndRenderPassInfo : RenderInfo
		{
		};

		struct CopyImageInfo : RenderInfo
		{};
		void CopyImage(const CopyImageInfo& copyImageInfo);

		struct CopyBufferToImageInfo : RenderInfo
		{
			class Buffer* SourceBuffer{ nullptr };
			ImageFormat SourceImageFormat;
			ImageLayout ImageLayout;
			class Image* DestinationImage{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};

		struct TransitionImageInfo : RenderInfo
		{
			const Image* Texture{ nullptr };
			ImageLayout SourceLayout, DestinationLayout;
			GTSL::uint32 SourceStage, DestinationStage;
			AccessFlags SourceAccessFlags, DestinationAccessFlags;
		};

		struct CopyBuffersInfo : RenderInfo
		{
			class Buffer* Source{ nullptr };
			GTSL::uint32 SourceOffset{ 0 };
			class Buffer* Destination{ nullptr };
			GTSL::uint32 DestinationOffset{ 0 };

			GTSL::uint32 Size{ 0 };
		};
	};

	class CommandPool : public GALObject
	{
	public:
		struct CreateInfo final : RenderInfo
		{
			const Queue* Queue{ nullptr };
			bool IsPrimary = true;
			GTSL::Ranger<class CommandBuffer> CommandBuffers;
		};
	};
}
