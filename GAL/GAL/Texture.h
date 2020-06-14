#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>

#include "Buffer.h"

namespace GAL
{
	//Represents a resource utilized by the rendering API for storing and referencing textures. Which are images which hold some information loaded from memory.
	class Texture : public GALObject
	{
	public:
		virtual ~Texture() = default;

		struct CreateInfo : RenderInfo
		{	
			ImageLayout Layout{ ImageLayout::COLOR_ATTACHMENT };
			ImageFormat SourceImageFormat{ ImageFormat::RGBA_I8 };
			GTSL::Extent2D Extent{ 1280, 720 };
			GTSL::uint8 Anisotropy = 0;
			class CommandBuffer* CommandBuffer = nullptr;
		};
		explicit Texture(const CreateInfo& textureCreateInfo);
	};

}
