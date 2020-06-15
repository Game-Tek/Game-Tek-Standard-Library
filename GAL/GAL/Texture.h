#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>

#include "GTSL/Memory.h"

namespace GAL
{
	//Represents a resource utilized by the rendering API for storing and referencing textures. Which are images which hold some information loaded from memory.
	class Texture : public GALObject
	{
	public:
		~Texture() = default;

		struct CreateInfo : RenderInfo
		{	
			ImageLayout Layout{ ImageLayout::COLOR_ATTACHMENT };
			ImageFormat SourceImageFormat{ ImageFormat::RGBA_I8 };
			GTSL::Extent2D Extent{ 1280, 720 };
			GTSL::uint8 Anisotropy = 0;
			ImageTiling ImageTiling;
			class CommandBuffer* CommandBuffer = nullptr;
		};
		explicit Texture(const CreateInfo& textureCreateInfo);

		static GTSL::uint64 GetTextureSize(const ImageFormat imageFormat, const GTSL::Extent2D extent)
		{
			return ImageFormatSize(imageFormat) * extent.Width * extent.Height;
		}

		static void ConvertImageFormat(const ImageFormat sourceImageFormat, const ImageFormat supportedImageFormat, const GTSL::Extent2D imageExtent, void* buffer)
		{
			const GTSL::uint64 originalTextureSize = GetTextureSize(sourceImageFormat, imageExtent);
			const GTSL::uint64 supportedTextureSize = (originalTextureSize / ImageFormatSize(sourceImageFormat)) * ImageFormatSize(supportedImageFormat);

			for (GTSL::uint64 i_target = 0, i_source = 0; i_target < supportedTextureSize; i_target += ImageFormatSize(supportedImageFormat), i_source += ImageFormatSize(sourceImageFormat))
			{
				GTSL::Memory::MemCopy(ImageFormatSize(sourceImageFormat), static_cast<char*>(buffer) + i_source, static_cast<char*>(buffer) + i_target);
				static_cast<char*>(buffer)[i_target + 3] = 0;
			}
		}
	};

}
