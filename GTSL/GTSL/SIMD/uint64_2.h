#pragma once

#include "SIMD.h"

#include <immintrin.h>

namespace GTSL
{
	class uint64_4
	{
		__m256i data;

	public:
		uint64_4(const AlignedPointer<uint64, 16> alignedPointer) : data(_mm256_load_si256(reinterpret_cast<__m256i*>(alignedPointer.Get())))
		{
		}

		uint64_4(const UnalignedPointer<uint64> unalignedPointer) : data(_mm256_loadu_si256(reinterpret_cast<__m256i*>(unalignedPointer.Get())))
		{
		}

		uint64_4(const uint64 a, const uint64 b, const uint64 c, const uint64 d) : data(_mm256_set_epi64x(d, c, b, a))
		{
		}

		uint64_4& operator+=(const uint64_4& other) { data = _mm256_add_epi64(data, other.data); return *this; }
		uint64_4& operator-=(const uint64_4& other) { data = _mm256_sub_epi64(data, other.data); return *this; }
		//uint64_8& operator*=(const uint64_8& other) { data = _mm256_mullo_epi64(data, other.data); return *this; }
		uint64_4& operator/=(const uint64_4& other) { data = _mm256_div_epi64(data, other.data); return *this; }

		void Store(const AlignedPointer<uint64, 16> alignedPointer) const
		{
			_mm256_store_si256(reinterpret_cast<__m256i*>(alignedPointer.Get()), data);
		}

		void Store(const UnalignedPointer<uint64> unalignedPointer) const
		{
			_mm256_store_si256(reinterpret_cast<__m256i*>(unalignedPointer.Get()), data);
		}
		
		uint8 FindEqualElement(const uint64_4& other)
		{
			alignas(16) uint64 array[4]{ 0 };
			_mm256_store_si256(reinterpret_cast<__m256i*>(array), _mm256_cmpeq_epi64(data, other.data));

			_mm256_movemask_epi8(data);
			
			for(uint64* begin = array; begin != array + 4; ++begin)
			{
				//*begin != 
			}
		}
	};
}