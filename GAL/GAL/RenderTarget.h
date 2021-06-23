#pragma once

#include "RenderCore.h"

#include "GTSL/Extent.h"

namespace GAL
{
	class RenderTarget : public GALObject
	{
	public:
		struct CreateInfo : RenderInfo
		{
			GTSL::Extent3D Extent { 0, 0, 1 };
			ImageFormat	Format { ImageFormat::RGBA_I8 };
			ImageType Type { ImageType::COLOR };
			ImageDimensions Dimensions { ImageDimensions::IMAGE_2D };
			ImageUse Use { ImageUse::INPUT_ATTACHMENT };
		};
		explicit RenderTarget(const CreateInfo& createInfo) : Format(createInfo.Format)
		{}

		RenderTarget() = default;

		[[nodiscard]] ImageFormat GetFormat() const { return Format; }

	protected:
		GAL::ImageFormat Format{ GAL::ImageFormat::RGBA_I8 };
	};
}
