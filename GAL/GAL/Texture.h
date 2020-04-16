#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>

namespace GAL
{
	struct TextureCreateInfo
	{
		void* ImageData = nullptr;
		size_t ImageDataSize = 0;
		ImageLayout Layout = ImageLayout::COLOR_ATTACHMENT;
		ImageFormat ImageFormat = ImageFormat::RGBA_I8;
		GTSL::Extent2D Extent = { 1280, 720 };

		GTSL::uint8 Anisotropy = 0;

		class CommandBuffer* CommandBuffer = nullptr;
	};

	//Represents a resource utilized by the rendering API for storing and referencing textures. Which are images which hold some information loaded from memory.
	class Texture : public GALObject
	{
		ImageLayout layout;
	public:
		virtual ~Texture() = default;

		explicit Texture(const TextureCreateInfo& textureCreateInfo) : layout(textureCreateInfo.Layout)
		{
		}

		[[nodiscard]] ImageLayout GetImageLayout() const { return layout; }
	};

}