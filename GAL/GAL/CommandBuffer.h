#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/Ranger.h>

#include "GTSL/RGBA.h"

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

	class CommandBuffer
	{
	public:
		~CommandBuffer() = default;

		struct CreateInfo : RenderInfo
		{
			bool IsPrimary = true;
		};
		explicit CommandBuffer(const CreateInfo& commandBufferCreateInfo)
		{};
		
		struct BeginRecordingInfo : RenderInfo
		{
			/**
			 * \brief Pointer to primary/parent command buffer, can be null if this command buffer is primary/has no children.
			 */
			CommandBuffer* PrimaryCommandBuffer{ nullptr };
		};
		//Starts recording of commands.
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);

		
		struct EndRecordingInfo : RenderInfo
		{
		};
		//Ends recording of commands.
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		// COMMANDS

		//  BIND COMMANDS
		//    BIND BUFFER COMMANDS

		//Adds a BindMesh command to the command queue.
		struct BindMeshInfo : RenderInfo
		{
			RenderMesh* Mesh = nullptr;
		};
		void BindMesh(const BindMeshInfo& bindMeshInfo);

		//    BIND PIPELINE COMMANDS

		//Adds a BindBindingsSet to the command queue.

		struct BindBindingsSetInfo : RenderInfo
		{
			GTSL::Ranger<BindingsSet*> BindingsSets;
			GTSL::Ranger<GTSL::uint32> Offsets;
			Pipeline* Pipeline = nullptr;
			GTSL::uint8 BindingsSetIndex = 0;
		};
		void BindBindingsSet(const BindBindingsSetInfo& bindBindingsSetInfo);

		struct UpdatePushConstantsInfo : RenderInfo
		{
			GraphicsPipeline* Pipeline = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};
		void UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo);

		struct BindGraphicsPipelineInfo : RenderInfo
		{
			GraphicsPipeline* GraphicsPipeline = nullptr;
			GTSL::Extent2D RenderExtent;
		};
		//Adds a BindGraphicsPipeline command to the command queue.
		void BindGraphicsPipeline(const BindGraphicsPipelineInfo& bindGraphicsPipelineInfo);

		struct BindComputePipelineInfo : RenderInfo
		{
			ComputePipeline* Pipeline = nullptr;
		};
		//Adds a BindComputePipeline to the command queue.
		void BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo);


		//  DRAW COMMANDS

		//Adds a DrawIndexed command to the command queue.
		struct DrawIndexedInfo : RenderInfo
		{
			GTSL::uint32 IndexCount = 0;
			GTSL::uint32 InstanceCount = 0;
		};
		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo);

		//  COMPUTE COMMANDS

		struct DispatchInfo : RenderInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		//Adds a Dispatch command to the command queue.
		void Dispatch(const DispatchInfo& dispatchInfo);

		//  RENDER PASS COMMANDS

		struct BeginRenderPassInfo : RenderInfo
		{
			RenderPass* RenderPass = nullptr;
			Framebuffer* Framebuffer = nullptr;
			GTSL::Extent2D RenderArea;
			GTSL::Ranger<RGBA> ClearValues;
		};
		//Adds a BeginRenderPass command to the command queue.
		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo);

		struct AdvanceSubpassInfo : RenderInfo
		{
			
		};
		//Adds a AdvanceSubPass command to the command buffer.
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo);

		struct EndRenderPassInfo : RenderInfo
		{
			
		};
		//Adds a EndRenderPass command to the command queue.
		void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo);

		struct CopyImageInfo : RenderInfo
		{};
		void CopyImage(const CopyImageInfo& copyImageInfo);

		struct CopyBufferToImageInfo : RenderInfo
		{
			class Buffer* SourceBuffer{ nullptr };
			ImageFormat SourceImageFormat;
			class Texture* DestinationImage{ nullptr };

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToImage(const CopyBufferToImageInfo& copyImageToBufferInfo);

		struct TransitionImageInfo : RenderInfo
		{
		};
		void TransitionImage(const TransitionImageInfo& transitionImageInfo);
	};
}
