#pragma once
#include <GTSL/StringCommon.h>

#include "Id.h"
#include "String.hpp"


namespace GTSL {
	struct LUTData {
		Vector3 Min, Max;
		uint32 Size;
	};

	bool ParseLUT(StringView file, LUTData& lutData, auto&& f) {
		uint32 samples = 0;

		ForEachLine(file, [&](StringView line) {
			auto lastToken = 0ull;

			uint8 component = 0; Vector3 vec;

			ForEachSubstring(line, [&](StringView token) {
				if (IsNumber(token)) {
					if(IsDecimalNumber(token)) {
						vec[component] = ToNumber<float32>(token).Get();
						++component;						
					} else {
						if (lastToken == Hash(u8"LUT_3D_SIZE")) {
							lutData.Size = ToNumber<uint32>(token).Get();
						}
					}
				} else {
					lastToken = Hash(token);					
				}

			}, u8' ');

			if(component == 3) {
				if(lastToken == Hash(u8"DOMAIN_MIN")) {
					lutData.Min = vec;									
				} else if(lastToken == Hash(u8"DOMAIN_MAX")) {
					lutData.Max = vec;
				} else {
					f(vec);
					++samples;
				}
			}
		});

		return lutData.Size != 0 and samples == (lutData.Size * lutData.Size * lutData.Size);
	}
}
