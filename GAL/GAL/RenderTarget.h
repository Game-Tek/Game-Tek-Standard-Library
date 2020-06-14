#pragma once

#include "RenderCore.h"

#include "GTSL/Extent.h"

namespace GAL
{
	class RenderTarget
	{
	public:

		struct CreateInfo : RenderInfo
		{
			GTSL::Extent3D		Extent		{ 0, 0, 0 };
			ImageFormat		Format		{ ImageFormat::RGBA_I8 };
			ImageType		Type		{ ImageType::COLOR };
			ImageDimensions Dimensions	{ ImageDimensions::IMAGE_2D };
			ImageUse		Use			{ ImageUse::INPUT_ATTACHMENT };
		};
		explicit RenderTarget(const CreateInfo& renderTargetCreateInfo) :
			Extent(renderTargetCreateInfo.Extent),
			Format(renderTargetCreateInfo.Format),
			Type(renderTargetCreateInfo.Type),
			Dimensions(renderTargetCreateInfo.Dimensions)
		{
		}

		RenderTarget() = default;

		[[nodiscard]] GTSL::Extent3D GetExtent() const { return Extent; }
		[[nodiscard]] GAL::ImageFormat GetFormat() const { return Format; }
		[[nodiscard]] ImageType	GetType() const { return Type; }
		[[nodiscard]] ImageDimensions GetDimensions() const { return Dimensions; }

	protected:
		GTSL::Extent3D Extent{ 0, 0, 1 };
		GAL::ImageFormat Format{ GAL::ImageFormat::RGBA_I8 };
		ImageType Type{ ImageType::COLOR };
		ImageDimensions Dimensions{ ImageDimensions::IMAGE_1D };

	};
}
