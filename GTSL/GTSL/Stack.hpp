#pragma once

#include "Allocator.h"
#include "Core.h"
#include <new>

namespace GTSL
{
	template<typename T, typename LT = uint32>
	class BasicStack
	{
		T* data{ nullptr };
		LT capacity{ 0 };
		LT elementCount{ 0 };

		void resize(const LT newLength, AllocatorReference* allocatorReference)
		{
			uint64 new_capacity{ 0 };
			void* new_data{ nullptr };
			allocatorReference->Allocate(newLength, alignof(T), &new_data, &new_capacity);
			Memory::MemCopy(elementCount, this->data, new_data);
			allocatorReference->Deallocate(capacity, alignof(T), reinterpret_cast<void*>(data));
			this->capacity = static_cast<LT>(new_capacity);
			this->data = static_cast<T*>(new_data);
		}
		
		void resizeIfNeeded(LT newElementsCount, AllocatorReference* allocatorReference)
		{
			if(this->elementCount + newElementsCount > this->capacity) [[unlikely]]
			{
				resize(this->capacity * 2, allocatorReference);
			}
		}
	public:
		BasicStack() = default;

		BasicStack(const LT capacity, AllocatorReference* allocatorReference)
		{
			allocatorReference->Allocate(capacity, alignof(T), reinterpret_cast<void**>(&this->data), reinterpret_cast<uint64*>(&capacity));
		}

		BasicStack(const LT capacity, const LT length, AllocatorReference* allocatorReference) : elementCount(length)
		{
			allocatorReference->Allocate(capacity, alignof(T), reinterpret_cast<void**>(&this->data), reinterpret_cast<uint64*>(&capacity));
		}

		~BasicStack() = default;

		void Free(AllocatorReference* allocatorReference)
		{
			allocatorReference->Deallocate(this->capacity, alignof(T), this->data);
		}
		
		LT Insert(const T& obj, AllocatorReference* allocatorReference)
		{
			resizeIfNeeded(1, allocatorReference);
			::new(static_cast<void*>(this->data + this->elementCount)) T(obj);
			return ++this->elementCount;
		}

		template<typename... ARGS>
		LT EmplaceBack(AllocatorReference* allocatorReference, ARGS&& args...)
		{
			resizeIfNeeded(1, allocatorReference);
			::new(static_cast<void*>(this->data + this->elementCount)) T(GTSL::MakeForwardReference<ARGS>(args)...);
			return ++this->elementCount;
		}
		
		LT Pop() { return --this->elementCount; }

		[[nodiscard]] LT GetLength() const { return this->elementCount; }
		[[nodiscard]] LT GetCapacity() const { return this->capacity; }

		[[nodiscard]] uint64 GetLengthSize() const { return this->elementCount * sizeof(T); }
		[[nodiscard]] uint64 GetCapacitySize() const { return this->capacity * sizeof(T); }

		T& operator[](const LT index) { return this->data[index]; }
		const T& operator[](const LT index) const { return this->data[index]; }

		T& front() const { return this->data[0]; }
		T& back() const { return this->data[this->elementCount]; }
		
		T* begin() const { return this->data; }
		T* end() const { return this->data[this->elementCount]; }
	};
}
