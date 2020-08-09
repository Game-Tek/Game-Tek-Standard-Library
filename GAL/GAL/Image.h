#pragma once

#include "RenderCore.h"

#include <GTSL/Extent.h>
#include <GTSL/Memory.h>
#include <GTSL/Math/Math.hpp>
#include <GTSL/SIMD/SIMD128.hpp>

namespace GAL
{
	//Represents a resource utilized by the rendering API for storing and referencing textures. Which are images which hold some information loaded from memory.
	class Image : public GALObject
	{
	public:
		Image() = default;
		~Image() = default;

		static GTSL::uint64 GetImageSize(const GTSL::uint8 textureFormatSize, const GTSL::Extent2D extent)
		{
			return static_cast<GTSL::uint32>(textureFormatSize) * extent.Width * extent.Height;
		}

		/**
		 * \brief Assumes source and target image formats are different(wont't fail if they are the same but it will perform the conversion and copying), assumes target format has a higher channel count that source.
		 * \param sourceImageFormat 
		 * \param targetImageFormat 
		 * \param imageExtent 
		 * \param buffer 
		 */
		static void ConvertImageToFormat(const ImageFormat sourceImageFormat, const ImageFormat targetImageFormat, const GTSL::Extent2D imageExtent, GTSL::AlignedPointer<GTSL::byte, 16> buffer, GTSL::uint32 alphaValue)
		{
			const auto source_format_size{ ImageFormatSize(sourceImageFormat) }; const auto target_format_size{ ImageFormatSize(targetImageFormat) };
			const GTSL::uint64 target_texture_size = GetImageSize(target_format_size, imageExtent);

			auto byte3_channel_swap = [&]()
			{
				GTSL::uint32 quot, rem;
				GTSL::Math::RoundDown(GetImageSize(source_format_size, imageExtent), 16, quot, rem);

				GTSL::byte* begin = buffer;
				
				for (; begin < buffer + quot - 1; begin += GTSL::SIMD128<GTSL::uint8>::TypeElementsCount - 1)
				{
					GTSL::SIMD128<GTSL::uint8> ints{ GTSL::AlignedPointer<GTSL::uint8, 16>(begin) };
					ints = GTSL::SIMD128<GTSL::uint8>::Shuffle<2, 1, 0, 5, 4, 3, 8, 7, 6, 11, 10, 9, 14, 13, 12, 15>(ints);
					ints.CopyTo(GTSL::AlignedPointer<GTSL::uint8, 16>(begin));
				}

				for(; begin < static_cast<GTSL::byte*>(buffer) + quot + rem; begin += 3)
				{
					const auto orig = *begin; *begin = *(begin + 2); *(begin + 2) = orig;
				}

				for (GTSL::byte* target = static_cast<GTSL::byte*>(buffer) + target_texture_size, *source = static_cast<GTSL::byte*>(buffer); target > 0; target -= target_format_size, source += source_format_size)
				{
					GTSL::MemCopy(source_format_size, source, target); *(target + 3) = alphaValue;
				}
			};
			
			switch (ImageFormatChannelCount(sourceImageFormat))
			{
			case 3: byte3_channel_swap(); break;
			default: __debugbreak();
			}
		}
	};

	class ImageView : public GALObject
	{
	public:
		ImageView() = default;
		~ImageView() = default;
	};

	class Sampler : public GALObject
	{
	public:
	};
}
