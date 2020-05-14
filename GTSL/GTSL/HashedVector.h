#pragma once

#include "Core.h"
#include "Allocator.h"
#include "Memory.h"

namespace GTSL
{
	template<typename T>
	class HashedVector
	{
		uint32 capacity{ 0 };
		uint32 length{ 0 };
		T* array{ nullptr };
		uint64* hashes{ nullptr };
		
		T* allocateArray(const uint32 elements, uint32& allocatedSize, AllocatorReference* allocatorReference)
		{
			uint64 allocated_size{ 0 }; allocatorReference->Allocate(elements * sizeof(T), alignof(T), reinterpret_cast<void**>(&array), &allocated_size); allocatedSize = static_cast<uint32>(allocated_size);
			return array;
		}

		T* allocateArray(const uint32 elements, AllocatorReference* allocatorReference)
		{
			uint32 allocated_size{ 0 };
			return allocateArray(elements, allocated_size, allocatorReference);
		}

		uint64* allocateHashes(const uint32 elements, uint32& allocatedSize, AllocatorReference* allocatorReference)
		{
			uint64 allocated_size{ 0 };	allocatorReference->Allocate(elements * sizeof(T), alignof(T), reinterpret_cast<void**>(&hashes), &allocated_size);	allocatedSize = static_cast<uint32>(allocated_size);
			return hashes;
		}

		uint64* allocateHashes(const uint32 elements, AllocatorReference* allocatorReference)
		{
			uint32 allocated_size{ 0 };
			return allocateHashes(elements, allocated_size, allocatorReference);
		}

		void deallocateArray(AllocatorReference* allocatorReference) { allocatorReference->Deallocate(this->capacity, alignof(T), this->hashes); }
		
		void deallocateHashes(AllocatorReference* allocatorReference) { allocatorReference->Deallocate(this->capacity, alignof(T), this->hashes); }
		
		[[nodiscard]] bool shouldResize(const uint32 newElements) const { return this->length + newElements > this->capacity; }

		void resize(const uint32 newElements, AllocatorReference* allocatorReference)
		{
			const uint32 new_intended_capacity = this->capacity * 2; uint32 allocated_capacity{ 0 };
			T* new_array = allocateArray(new_intended_capacity, allocated_capacity, allocatorReference); uint64* new_hashes = allocateHashes(new_intended_capacity, allocated_capacity, allocatorReference);
			GTSL::Memory::MemCopy(this->capacity * sizeof(T), this->arrayBegin(), new_array); GTSL::Memory::MemCopy(this->capacity * sizeof(uint64), this->hashBegin(), new_hashes);
			deallocateArray(allocatorReference); deallocateHashes(allocatorReference);
			this->array = new_array; this->hashes = new_hashes;	this->capacity = allocated_capacity;
		}

		[[nodiscard]] void* getArrayLoc(const uint32 pos) const { return this->array + pos; } [[nodiscard]] void* getHashLoc(const uint32 pos) const { return this->hashes + pos; }

		[[nodiscard]] uint64* hashBegin() const { return this->hashes; } [[nodiscard]] uint64* hashEnd() const { return this->hashes + this->length; }
		[[nodiscard]] T* arrayBegin() const { return this->array; }	[[nodiscard]] T* arrayEnd() const { return this->array + this->length; }
	public:
		HashedVector() = default;
		
		HashedVector(const uint32 capacity, AllocatorReference* allocatorReference) : capacity(capacity), array(allocateArray(capacity, allocatorReference)), hashes(allocateHashes(capacity, allocatorReference))
		{}

		T* begin() { return arrayBegin(); } T* end() { return arrayEnd(); }
		
		void Free(AllocatorReference* allocatorReference) { deallocateHashes(allocatorReference); deallocateArray(allocatorReference); }
		
		uint32 PushBack(const T& obj, uint64 hash, AllocatorReference* allocatorReference)
		{
			if (shouldResize(1)) [[unlikely]] { resize(1, allocatorReference); }
			::new(getArrayLoc(this->length)) T(obj); ::new(getHashLoc(this->length)) uint64(hash);
			return ++this->length;
		}

		void PopBack() { --this->length; }
		
		T* Find(const uint64 hash)
		{
			for(uint64* begin = this->hashBegin(); begin != hashEnd(); ++begin)
			{
				if (*begin == hash) { return begin; }
			}

			return end();
		}
	};
}
