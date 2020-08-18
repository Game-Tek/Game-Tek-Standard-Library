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
		//Starts recording of commands.

		
		//Ends recording of commands.

		// COMMANDS

		//  BIND COMMANDS
		//    BIND BUFFER COMMANDS

		//Adds a BindMesh command to the command queue.
		
		//    BIND PIPELINE COMMANDS

		//Adds a BindBindingsSets to the command queue.

		//Adds a BindGraphicsPipeline command to the command queue.
		//Adds a BindComputePipeline to the command queue.

		//  DRAW COMMANDS

		//Adds a DrawIndexed command to the command queue.

		//  COMPUTE COMMANDS

		//Adds a Dispatch command to the command queue.

		//  RENDER PASS COMMANDS

		//Adds a BeginRenderPass command to the command queue.

		//Adds a AdvanceSubPass command to the command buffer.
	};

	class CommandPool : public GALObject
	{
	public:
	};
}
