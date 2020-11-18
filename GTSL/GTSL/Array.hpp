#pragma once

#include "Core.h"

#include "ArrayCommon.hpp"
#include <initializer_list>
#include "Memory.h"
#include "Assert.h"
#include "Range.h"
#include <new>

namespace GTSL
{
	template <typename T, uint32 CAPACITY>
	class Array
	{
	public:
		typedef T* iterator;
		typedef const T* const_iterator;

		[[nodiscard]] constexpr iterator begin() noexcept { return reinterpret_cast<iterator>(this->data); }

		[[nodiscard]] constexpr iterator end() noexcept { return reinterpret_cast<iterator>(this->data + this->length); }

		[[nodiscard]] constexpr const_iterator begin() const noexcept { return reinterpret_cast<const_iterator>(this->data); }

		[[nodiscard]] constexpr const_iterator end() const noexcept { return reinterpret_cast<const_iterator>(this->data + this->length); }

		constexpr T& front() noexcept { return this->data[0]; }

		constexpr T& back() noexcept { return this->data[this->length - 1]; }

		[[nodiscard]] constexpr const T& front() const noexcept { return this->data[0]; }

		[[nodiscard]] constexpr const T& back() const noexcept { return this->data[this->length - 1]; }

		operator GTSL::Range<T*>() { return Range<T*>(this->length, this->begin()); }
		operator GTSL::Range<const T*>() const { return Range<const T*>(this->length, this->begin()); }


		auto GetRange() { return Range<T*>(this->length, this->begin()); }
		
		constexpr Array() noexcept = default;

		constexpr Array(const std::initializer_list<T> list) noexcept : length(static_cast<uint32>(list.size()))
		{
			GTSL_ASSERT(list.size() <= CAPACITY, "Initializer list is bigger than array capacity!")
			for (uint32 i = 0; i < list.size(); ++i) { this->data[i] = MoveRef(*(list.begin() + i)); }
		}

		constexpr explicit Array(const uint32 length) noexcept : length(length)
		{
			GTSL_ASSERT(length <= CAPACITY, "Array is not big enough to insert the elements requested!")
		}

		constexpr Array(const Range<const T*> ranger) noexcept : length(static_cast<uint32>(ranger.ElementCount()))
		{
			GTSL_ASSERT(ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!");
			for (uint32 i = 0; i < ranger.ElementCount(); ++i) { this->data[i] = ranger[i]; }
		}

		constexpr Array(const Array& other) noexcept : length(other.length) { for (uint32 i = 0; i < other.length; ++i) { this->data[i] = other[i]; } }

		template<uint32 N>
		constexpr Array(const Array<T, N>& other) noexcept : length(other.GetLength())
		{
			GTSL_ASSERT(other.GetLength() <= CAPACITY, "Other array has more elements than this can handle.")
			for (uint32 i = 0; i < other.length; ++i) { this->data[i] = other[i]; }
		}

		constexpr Array(Array&& other) noexcept : length(other.length)
		{
			for (uint32 i = 0; i < other.length; ++i) { this->data[i] = MoveRef(other[i]); }
			for (auto& e : other) { e.~T(); }
			other.length = 0;
		}

		constexpr Array& operator=(const Array& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { this->data[i] = other[i]; }
			length = other.length;
			return *this;
		}

		template<uint32 CAP>
		constexpr Array& operator=(const Array<T, CAP>& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { this->data[i] = other[i]; }
			length = other.length;
			return *this;
		}

		constexpr Array& operator=(Array&& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { this->data[i] = MoveRef(other[i]); }
			length = other.length;
			for (auto& e : other) { e.~T(); }
			other.length = 0;
			return *this;
		}

		template<uint32 CAP>
		constexpr Array& operator=(Array<T, CAP>&& other) noexcept
		{
			for (uint32 i = 0; i < other.length; ++i) { this->data[i] = MoveRef(other[i]); }
			length = other.length;
			for (auto& e : other) { e.~T(); }
			other.length = 0;
			return *this;
		}

		constexpr Array& operator=(const Range<const T*> ranger) noexcept
		{
			for (uint32 i = 0; i < ranger.ElementCount(); ++i) { this->data[i] = ranger[i]; }
			length = ranger.ElementCount();
			return *this;
		}

		~Array()
		{
			for (auto& e : *this) { e.~T(); }
		}

		constexpr T& operator[](const uint32 i) noexcept
		{
			GTSL_ASSERT(i < CAPACITY && i < this->length, "Out of Bounds! Requested index is greater than the Array's size!");
			return this->data[i];
		}

		constexpr const T& operator[](const uint32 i) const noexcept
		{
			GTSL_ASSERT(i < CAPACITY && i < this->length, "Out of Bounds! Requested index is greater than the Array's size!");
			return this->data[i];
		}

		constexpr bool operator==(const Array& other) const
		{
			for (iterator begin = this->begin(), begin_other = other.begin(); begin != this->end(); ++begin)
			{
				if (*begin != *begin_other) { return false; }
			}
			return true;
		}

		constexpr uint32 PushBack(const T& obj) noexcept
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			::new(static_cast<void*>(this->data + this->length)) T(obj);
			return this->length++;
		}

		constexpr uint32 PushBack(T&& obj) noexcept
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MoveRef(obj));
			return this->length++;
		}

		constexpr uint32 PushBack(const Range<const T*> ranger) noexcept
		{
			GTSL_ASSERT(this->length + ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!")
			for (uint32 i = 0, p = length; i < ranger.ElementCount(); ++i, ++p) { this->data[p] = ranger[i]; }
			auto ret = this->length;
			this->length += static_cast<uint32>(ranger.ElementCount());
			return ret;
		}

		void Pop(const uint32 index)
		{
			GTSL_ASSERT(length != 0, "Length can't be 0");
			popElement(GetRange(), index);
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

		template<typename... ARGS>
		constexpr uint32 EmplaceBack(ARGS&&... args)
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			::new(this->data + this->length) T(GTSL::ForwardRef<ARGS>(args)...);
			return this->length++;
		}

		constexpr void Resize(uint32 size)
		{
			GTSL_ASSERT(size <= CAPACITY, "Requested size for array Resize is greater than Array's statically allocated size!");
			this->length = size;
		}

		constexpr void PopBack()
		{
			GTSL_ASSERT(this->length != 0, "Array's length is already 0. Cannot pop any more elements!");
			reinterpret_cast<T&>(this->data[this->length]).~T();
			--this->length;
		}

		[[nodiscard]] constexpr uint32 GetLength() const noexcept { return this->length; }

		[[nodiscard]] constexpr uint32 GetCapacity() const noexcept { return CAPACITY; }

	private:
		uint32 length = 0;
		T data[CAPACITY];

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