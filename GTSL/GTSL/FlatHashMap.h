#pragma once

#include "Core.h"

#include "Allocator.h"
#include "Memory.h"
#include "Ranger.h"
#include <new>

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{
	template<typename T>
	class FlatHashMap
	{
	public:
		using key_type = uint64;
		using ref = uint64;

		FlatHashMap(const uint32 size, AllocatorReference* allocatorReference) : capacity(size)
		{
			GTSL_ASSERT(size != 0 && (size & (size - 1)) == 0, "Size is not a power of two!")
				this->data = allocate(size, allocatorReference);
			build(this->data, 0, size);
		}

		void Free(AllocatorReference* allocatorReference)
		{
			deallocate(allocatorReference);
			this->data = nullptr;
		}

		~FlatHashMap()
		{
			GTSL_ASSERT(!this->data, "Data was not freed!")
		}

		template<typename... ARGS>
		ref Emplace(AllocatorReference* allocatorReference, const key_type key, ARGS&&... args)
		{
			const auto index = modulo(key, this->capacity);
			uint64& vector_length = getVectorLength(index);

#if (_DEBUG)
			for (auto& key_in_collection : getKeysRange(index))
			{
				GTSL_ASSERT(key_in_collection != key, "Key already exists!")
			}
#endif
			
			if (vector_length + 1 > this->capacity)
			{
				resize(allocatorReference);
				vector_length = getVectorLength(index);
			}

			getKeysRange(index)[vector_length] = key;
			::new(getValuesVector(index) + vector_length) T(MakeForwardReference<ARGS>(args)...);

			auto result = makeRef(index, vector_length);
			vector_length += 1;
			return result;
		}

		T& At(const key_type key)
		{
			const auto index = modulo(key, capacity);

			for (auto& key_candidate : getKeysRange(index))
			{
				if (key_candidate == key) { return getValuesRange(index)[&key_candidate - getKeysRange(index).begin()]; }
			}

			GTSL_ASSERT(false, "No element with that key!")
		}

		T& operator[](const ref reference) { return *(getValuesVector(static_cast<uint32>(reference)) + (reference >> 32)); }
	private:
		uint32 capacity = 0;
		byte* data = nullptr;

		Ranger<key_type> getKeysRange(const uint32 index) { return Ranger<key_type>(getVectorLength(index), getKeyVector(index) + 1); }

		Ranger<T> getValuesRange(const uint32 index) { return Ranger<T>(getVectorLength(index), getValuesVector(index)); }

		static constexpr uint32 modulo(const key_type key, const uint32 size) { return key & (size - 1); }

		void resize(AllocatorReference* allocatorReference)
		{
			auto new_length = this->capacity * 2;
			auto new_alloc = allocate(new_length, allocatorReference);
			copy(new_length, new_alloc);
			deallocate(allocatorReference);
			build(new_alloc, this->capacity, new_length);
			this->data = new_alloc;
			this->capacity = new_length;
		}

		static uint64 getKeyVectorAllocationSize(const uint32 length) { return (length + 1) * sizeof(key_type); }
		static uint64 getKeysAllocationSize(const uint32 length) { return getKeyVectorAllocationSize(length) * length; }
		
		static uint64 getValuesVectorAllocationsSize(const uint32 length) { return length * sizeof(T); }
		static uint64 getValuesAllocationsSize(const uint32 length) { return getValuesVectorAllocationsSize(length) * length; }

		static uint64 getTotalAllocationSize(const uint32 length) { return getKeysAllocationSize(length) + getValuesAllocationsSize(length); }

		static uint64& getVectorLength(byte* data, const uint32 capacity, const uint32 index) { return *(reinterpret_cast<key_type*>(data) + ((capacity + 1) * index)); }
		
		byte* allocate(const uint64 newLength, AllocatorReference* allocatorReference)
		{
			uint64 allocated_size{ 0 };
			void* memory{ nullptr };
			allocatorReference->Allocate(getTotalAllocationSize(newLength), alignof(T), &memory, &allocated_size);
			return static_cast<byte*>(memory);
		}

		void deallocate(AllocatorReference* allocatorReference)
		{
			allocatorReference->Deallocate(getTotalAllocationSize(this->capacity), alignof(T), this->data);
		}

		void build(byte* data, const uint32 oldLength, const uint32 newLength)
		{
			for (uint32 i = oldLength; i < newLength; ++i) { getVectorLength(data, newLength, i) = 0; }
		}

		void copy(const uint64 newLength, byte* to)
		{
			for (uint32 i = 0; i < this->capacity; ++i)
			{
				const auto length = getVectorLength(i);
				Memory::MemCopy(getKeysAllocationSize(this->capacity), getKeyVector(i), to + getKeyVectorAllocationSize(newLength) * i);
				Memory::MemCopy(getValuesAllocationsSize(this->capacity), getValuesVector(i), to + getKeysAllocationSize(newLength) + getValuesVectorAllocationsSize(newLength) * i);
			}
		}

		key_type* getKeyVector(const uint32 index) { return &getVectorLength(this->data, this->capacity, index); }

		uint64& getVectorLength(const uint32 index) { return getVectorLength(this->data, this->capacity, index); }

		T* getValuesVector(const uint32 index) { return reinterpret_cast<T*>(this->data + getKeysAllocationSize(this->capacity)) + index * this->capacity; }

		static ref makeRef(const uint32 vector, const uint32 index) { return static_cast<uint64>(index) << 32 | vector; }
	};
}
