#pragma once

#include <GTSL/Vector.hpp>
#include <GTSL/Extent.h>

struct GTSL::Extent3D;

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
		virtual ~CommandBuffer() = default;

		struct CommandBufferCreateInfo
		{
			bool IsPrimary = true;
		};
		explicit CommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo)
		{};
		
		struct BeginRecordingInfo
		{};
		//Starts recording of commands.
		virtual void BeginRecording(const BeginRecordingInfo& beginRecordingInfo) = 0;

		
		struct EndRecordingInfo
		{
			
		};
		//Ends recording of commands.
		virtual void EndRecording(const EndRecordingInfo& endRecordingInfo) = 0;

		// COMMANDS

		//  BIND COMMANDS
		//    BIND BUFFER COMMANDS

		//Adds a BindMesh command to the command queue.
		struct BindMeshInfo
		{
			RenderMesh* Mesh = nullptr;
		};
		virtual void BindMesh(const BindMeshInfo& bindMeshInfo) = 0;

		//    BIND PIPELINE COMMANDS

		//Adds a BindBindingsSet to the command queue.

		struct BindBindingsSetInfo
		{
			GTSL::Vector<BindingsSet*>* BindingsSets = nullptr;
			GTSL::Vector<GTSL::uint32>* Offsets = nullptr;
			Pipeline* Pipeline = nullptr;
			GTSL::uint8 BindingsSetIndex = 0;
		};
		virtual void BindBindingsSet(const BindBindingsSetInfo& bindBindingsSetInfo) = 0;

		struct UpdatePushConstantsInfo
		{
			GraphicsPipeline* Pipeline = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};
		virtual void UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo) = 0;

		struct BindGraphicsPipelineInfo
		{
			GraphicsPipeline* GraphicsPipeline = nullptr;
			GTSL::Extent2D RenderExtent;
		};
		//Adds a BindGraphicsPipeline command to the command queue.
		virtual void BindGraphicsPipeline(const BindGraphicsPipelineInfo& bindGraphicsPipelineInfo) = 0;

		struct BindComputePipelineInfo
		{
			ComputePipeline* Pipeline = nullptr;
		};
		//Adds a BindComputePipeline to the command queue.
		virtual void BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo) = 0;


		//  DRAW COMMANDS

		//Adds a DrawIndexed command to the command queue.
		struct DrawIndexedInfo
		{
			GTSL::uint32 IndexCount = 0;
			GTSL::uint32 InstanceCount = 0;
		};
		virtual void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo) = 0;

		//  COMPUTE COMMANDS

		struct DispatchInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		//Adds a Dispatch command to the command queue.
		virtual void Dispatch(const DispatchInfo& dispatchInfo) = 0;

		//  RENDER PASS COMMANDS

		struct BeginRenderPassInfo
		{
			RenderPass* RenderPass = nullptr;
			Framebuffer* Framebuffer = nullptr;
			GTSL::Extent2D RenderArea;
		};
		//Adds a BeginRenderPass command to the command queue.
		virtual void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo) = 0;

		struct AdvanceSubpassInfo
		{
			
		};
		//Adds a AdvanceSubPass command to the command buffer.
		virtual void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo) = 0;

		struct EndRenderPassInfo
		{
			
		};
		//Adds a EndRenderPass command to the command queue.
		virtual void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo) = 0;

		struct CopyImageInfo
		{};
		virtual void CopyImage(const CopyImageInfo& copyImageInfo) = 0;
	};
}
