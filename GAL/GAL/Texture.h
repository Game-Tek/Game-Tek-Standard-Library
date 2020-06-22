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
			ImageFormat SourceFormat{ ImageFormat::RGBA_I8 };
			GTSL::Extent2D Extent{ 1280, 720 };
			GTSL::uint8 Anisotropy = 0;
			ImageTiling ImageTiling;
			class CommandBuffer* CommandBuffer = nullptr;
		};
		explicit Texture(const CreateInfo& textureCreateInfo);

		static GTSL::uint64 GetTextureSize(const GTSL::uint8 textureFormatSize, const GTSL::Extent2D extent)
		{
			return static_cast<GTSL::uint16>(textureFormatSize) * extent.Width * extent.Height;
		}

		static void ConvertImageToFormat(const ImageFormat sourceImageFormat, const ImageFormat supportedImageFormat, const GTSL::Extent2D imageExtent, void* buffer)
		{
			const auto source_format_size{ ImageFormatSize(sourceImageFormat) }; const auto target_format_size{ ImageFormatSize(supportedImageFormat) };
			const GTSL::uint64 target_texture_size = GetTextureSize(target_format_size, imageExtent);

			for (GTSL::byte* target = nullptr, *source = nullptr; target < static_cast<GTSL::byte*>(buffer) + target_texture_size; target += target_format_size, source += source_format_size)
			{
				GTSL::MemCopy(source_format_size, source, target); *(target + 3) = 0;
			}
		}
	};

}
