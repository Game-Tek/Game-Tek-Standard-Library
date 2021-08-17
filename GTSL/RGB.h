#pragma once

#include "Core.h"

namespace GTSL
{
	//Used to specify a RGB color with floating point precision.
	struct RGB
	{
		float32& R() { return array[0]; }
		float32& G() { return array[1];	}
		float32& B() { return array[2];	}

		[[nodiscard]] float32 R() const { return array[0]; }
		[[nodiscard]] float32 G() const { return array[1]; }
		[[nodiscard]] float32 B() const { return array[2]; }

	private:
		float32 array[3]{ 0.0f };
	};

	struct alignas(16) RGBA
	{
		RGBA() = default;
		RGBA(const float32 r, const float32 g, const float32 b, const float32 a) : array{r, g, b, a} {}
		
		float32& R() { return array[0]; }
		float32& G() { return array[1];	}
		float32& B() { return array[2];	}
		float32& A() { return array[3];	}

		[[nodiscard]] float32 R() const { return array[0]; }
		[[nodiscard]] float32 G() const { return array[1]; }
		[[nodiscard]] float32 B() const { return array[2]; }
		[[nodiscard]] float32 A() const { return array[3]; }

	private:
		float32 array[4]{ 0.0f };
	};
}