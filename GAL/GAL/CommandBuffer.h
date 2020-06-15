#pragma once

#include <GTSL/Vector.hpp>
#include <GTSL/Extent.h>

#include "GTSL/RGBA.h"

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
		~CommandBuffer() = default;

		struct CommandBufferCreateInfo
		{
			bool IsPrimary = true;
		};
		explicit CommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo)
		{};
		
		struct BeginRecordingInfo
		{};
		//Starts recording of commands.
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);

		
		struct EndRecordingInfo
		{
			
		};
		//Ends recording of commands.
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		// COMMANDS

		//  BIND COMMANDS
		//    BIND BUFFER COMMANDS

		//Adds a BindMesh command to the command queue.
		struct BindMeshInfo
		{
			RenderMesh* Mesh = nullptr;
		};
		void BindMesh(const BindMeshInfo& bindMeshInfo);

		//    BIND PIPELINE COMMANDS

		//Adds a BindBindingsSet to the command queue.

		struct BindBindingsSetInfo
		{
			GTSL::Vector<BindingsSet*>* BindingsSets = nullptr;
			GTSL::Vector<GTSL::uint32>* Offsets = nullptr;
			Pipeline* Pipeline = nullptr;
			GTSL::uint8 BindingsSetIndex = 0;
		};
		void BindBindingsSet(const BindBindingsSetInfo& bindBindingsSetInfo);

		struct UpdatePushConstantsInfo
		{
			GraphicsPipeline* Pipeline = nullptr;
			size_t Offset = 0;
			size_t Size = 0;
			GTSL::byte* Data = nullptr;
		};
		void UpdatePushConstant(const UpdatePushConstantsInfo& updatePushConstantsInfo);

		struct BindGraphicsPipelineInfo
		{
			GraphicsPipeline* GraphicsPipeline = nullptr;
			GTSL::Extent2D RenderExtent;
		};
		//Adds a BindGraphicsPipeline command to the command queue.
		void BindGraphicsPipeline(const BindGraphicsPipelineInfo& bindGraphicsPipelineInfo);

		struct BindComputePipelineInfo
		{
			ComputePipeline* Pipeline = nullptr;
		};
		//Adds a BindComputePipeline to the command queue.
		void BindComputePipeline(const BindComputePipelineInfo& bindComputePipelineInfo);


		//  DRAW COMMANDS

		//Adds a DrawIndexed command to the command queue.
		struct DrawIndexedInfo
		{
			GTSL::uint32 IndexCount = 0;
			GTSL::uint32 InstanceCount = 0;
		};
		void DrawIndexed(const DrawIndexedInfo& drawIndexedInfo);

		//  COMPUTE COMMANDS

		struct DispatchInfo
		{
			GTSL::Extent3D WorkGroups;
		};
		//Adds a Dispatch command to the command queue.
		void Dispatch(const DispatchInfo& dispatchInfo);

		//  RENDER PASS COMMANDS

		struct BeginRenderPassInfo
		{
			RenderPass* RenderPass = nullptr;
			Framebuffer* Framebuffer = nullptr;
			GTSL::Extent2D RenderArea;
			GTSL::Ranger<RGBA> ClearValues;
		};
		//Adds a BeginRenderPass command to the command queue.
		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo);

		struct AdvanceSubpassInfo
		{
			
		};
		//Adds a AdvanceSubPass command to the command buffer.
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo);

		struct EndRenderPassInfo
		{
			
		};
		//Adds a EndRenderPass command to the command queue.
		void EndRenderPass(const EndRenderPassInfo& endRenderPassInfo);

		struct CopyImageInfo
		{};
		void CopyImage(const CopyImageInfo& copyImageInfo);
	};
}
