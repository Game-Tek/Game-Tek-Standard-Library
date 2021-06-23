#pragma once

#include "Core.h"

#include "ArrayCommon.hpp"
#include <initializer_list>
#include "Memory.h"
#include "Assert.h"
#include "Range.h"
#include <new>

#include "Result.h"

namespace GTSL
{
	template <typename T, uint32 CAPACITY>
	class Array
	{
	public:
		typedef T* iterator;
		typedef const T* const_iterator;

		[[nodiscard]] constexpr iterator begin() noexcept { return reinterpret_cast<iterator>(this->data); }

		[[nodiscard]] constexpr iterator end() noexcept { return reinterpret_cast<iterator>(this->data) + this->length; }

		[[nodiscard]] constexpr const_iterator begin() const noexcept { return reinterpret_cast<const_iterator>(this->data); }

		[[nodiscard]] constexpr const_iterator end() const noexcept { return reinterpret_cast<const_iterator>(this->data) + this->length; }

		constexpr T& front() noexcept { return *begin(); }

		constexpr T& back() noexcept { return *(end() - 1); }

		[[nodiscard]] constexpr const T& front() const noexcept { return *begin(); }

		[[nodiscard]] constexpr const T& back() const noexcept { return *(end() - 1); }

		operator GTSL::Range<T*>() { return Range<T*>(this->length, this->begin()); }
		operator GTSL::Range<const T*>() const { return Range<const T*>(this->length, this->begin()); }

		auto GetRange() { return Range<T*>(this->length, this->begin()); }
		
		constexpr Array() noexcept = default;

		constexpr Array(const std::initializer_list<T> list) noexcept : length(static_cast<uint32>(list.size()))
		{
			GTSL_ASSERT(list.size() <= CAPACITY, "Initializer list is bigger than array capacity!")
			for (uint32 i = 0; i < list.size(); ++i) { begin()[i] = MoveRef(*(list.begin() + i)); }
		}

		constexpr Array(const Range<const T*> ranger) noexcept : length(static_cast<uint32>(ranger.ElementCount()))
		{
			GTSL_ASSERT((uint32)ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!");
			for (uint32 i = 0; i < static_cast<uint32>(ranger.ElementCount()); ++i) { begin()[i] = ranger[i]; }
		}

		constexpr Array(const Array& other) noexcept : length(other.length) { for (uint32 i = 0; i < other.length; ++i) { begin()[i] = other[i]; } }

		template<uint32 N>
		constexpr Array(const Array<T, N>& other) noexcept : length(other.GetLength())
		{
			GTSL_ASSERT(other.GetLength() <= CAPACITY, "Other array has more elements than this can handle.")
			for (uint32 i = 0; i < other.length; ++i) { begin()[i] = other[i]; }
		}

		constexpr Array(Array&& other) noexcept : length(other.length)
		{
			for (uint32 i = 0; i < other.length; ++i) { begin()[i] = MoveRef(other[i]); }
			for (auto& e : other) { e.~T(); }
			other.length = 0;
		}

		constexpr Array& operator=(const Array& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { begin()[i] = other[i]; }
			length = other.length;
			return *this;
		}

		template<uint32 CAP>
		constexpr Array& operator=(const Array<T, CAP>& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { begin()[i] = other[i]; }
			length = other.length;
			return *this;
		}

		constexpr Array& operator=(Array&& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { begin()[i] = MoveRef(other[i]); }
			length = other.length;
			for (auto& e : other) { e.~T(); }
			other.length = 0;
			return *this;
		}

		template<uint32 CAP>
		constexpr Array& operator=(Array<T, CAP>&& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { begin()[i] = MoveRef(other[i]); }
			length = other.length;
			for (auto& e : other) { e.~T(); }
			other.length = 0;
			return *this;
		}

		constexpr Array& operator=(const Range<const T*> ranger) noexcept
		{
			for (uint32 i = 0; i < static_cast<uint32>(ranger.ElementCount()); ++i) { begin()[i] = ranger[i]; }
			length = static_cast<uint32>(ranger.ElementCount());
			return *this;
		}

		~Array()
		{
			for (auto& e : *this) { e.~T(); }
		}

		constexpr T& operator[](const uint32 i) noexcept {
			GTSL_ASSERT(i < CAPACITY && i < this->length, "Out of Bounds! Requested index is greater than the Array's size!");
			return begin()[i];
		}

		constexpr const T& operator[](const uint32 i) const noexcept {
			GTSL_ASSERT(i < CAPACITY && i < this->length, "Out of Bounds! Requested index is greater than the Array's size!");
			return begin()[i];
		}

		constexpr bool operator==(const Array& other) const {
			for (iterator begin = this->begin(), begin_other = other.begin(); begin != this->end(); ++begin)
			{
				if (*begin != *begin_other) { return false; }
			}
			return true;
		}

		constexpr bool operator==(const Range<const T*> other) const {
			if (other.ElementCount() != this->length) { return false; }
			
			for (uint32 i = 0; i < this->length; ++i) {
				if (begin()[i] != other[i]) { return false; }
			}
			
			return true;
		}

		constexpr T& PushBack(const T& obj) noexcept {
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			auto* newObject = ::new(end()) T(obj); this->length++;
			return *newObject;
		}

		constexpr T& PushBack(T&& obj) noexcept
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			auto* newObject = ::new(end()) T(GTSL::MoveRef(obj)); this->length++;
			return *newObject;
		}

		constexpr uint32 PushBack(const Range<const T*> ranger) noexcept
		{
			GTSL_ASSERT(this->length + (uint32)ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!")
			for (uint32 i = 0, p = length; i < static_cast<uint32>(ranger.ElementCount()); ++i, ++p) { begin()[p] = ranger[i]; }
			auto ret = this->length;
			this->length += static_cast<uint32>(ranger.ElementCount());
			return ret;
		}

		void Pop(const uint32 index)
		{
			GTSL_ASSERT(length != 0, "Length can't be 0");
			popElement(GetRange(), index); --length;
		}
		
		//constexpr uint32 Insert(const uint32 index, const Range<const T*> ranger) noexcept
		//{
		//	copy(ranger.ElementCount(), ranger.begin(), this->data + index);
		//	auto ret = this->length;
		//	this->length += static_cast<uint32>(ranger.ElementCount());
		//	return ret;
		//}

		void Remove(const uint32 from, const uint32 to) noexcept
		{
			copy(this->length - (to - from), this->data + to, this->data + from);
			this->length -= to - from;
		}

		
		constexpr T& EmplaceBack()
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			auto* newObject = ::new(end()) T(); ++length;
			return *newObject;
		}

		template<typename... ARGS>
		constexpr T& EmplaceBack(ARGS&&... args)
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			auto* newObject = ::new(end()) T(GTSL::ForwardRef<ARGS>(args)...); ++length;
			return *newObject;
		}

		constexpr void Resize(uint32 size)
		{
			GTSL_ASSERT(size <= CAPACITY, "Requested size for array Resize is greater than Array's statically allocated size!");
			this->length = size;
		}

		constexpr void PopBack()
		{
			GTSL_ASSERT(this->length != 0, "Array's length is already 0. Cannot pop any more elements!");
			--length; back().~T();
		}

		Result<uint32> Find(const T& obj) const
		{
			for (uint32 i = 0; i < GetLength(); ++i) { if (begin()[i] == obj) { return Result<uint32>(MoveRef(i), true); } }
			return Result<uint32>(false);
		}
		
		[[nodiscard]] constexpr uint32 GetLength() const noexcept { return this->length; }

		[[nodiscard]] constexpr uint32 GetCapacity() const noexcept { return CAPACITY; }

	private:
		uint32 length = 0;
		alignas(T) byte data[CAPACITY * sizeof(T)];

		constexpr void copyToData(const void* from, const uint64 length) noexcept
		{
			MemCopy(length * sizeof(T), from, this->data);
		}

		void copy(const uint64 length, const void* from, void* to)
		{
			MemCopy(length * sizeof(T), from, to);
		}

		friend class Array;
	};
}
