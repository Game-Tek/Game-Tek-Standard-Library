#pragma once

#include "Core.h"

#include <initializer_list>
#include "Memory.h"
#include "Assert.h"
#include "Ranger.h"
#include <new>

namespace GTSL
{
	template <typename T, size_t CAPACITY>
	class Array
	{
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

	public:
		typedef T* iterator;
		typedef const T* const_iterator;

		[[nodiscard]] constexpr iterator begin() noexcept { return reinterpret_cast<iterator>(&this->data[0]); }

		[[nodiscard]] constexpr iterator end() noexcept { return reinterpret_cast<iterator>(&this->data[this->length * sizeof(T)]); }

		[[nodiscard]] constexpr const_iterator begin() const noexcept { return reinterpret_cast<const_iterator>(&this->data[0]); }

		[[nodiscard]] constexpr const_iterator end() const noexcept { return reinterpret_cast<const_iterator>(&this->data[this->length * sizeof(T)]); }

		constexpr T& front() noexcept { return this->data[0]; }

		constexpr T& back() noexcept { return this->data[this->length * sizeof(T)]; }

		[[nodiscard]] constexpr const T& front() const noexcept { return this->data[0]; }

		[[nodiscard]] constexpr const T& back() const noexcept { return this->data[this->length * sizeof(T)]; }

		operator GTSL::Ranger<T>() { return Ranger<T>(this->length, this->begin()); }
		operator GTSL::Ranger<const T>() const { return Ranger<const T>(this->length, this->begin()); }

		constexpr Array() noexcept = default;

		constexpr Array(std::initializer_list<T> list) noexcept : length(list.size())
		{
			GTSL_ASSERT(list.size() <= CAPACITY, "Initializer list is bigger than array capacity!")
			copyToData(list.begin(), this->length);
		}

		constexpr explicit Array(const uint32 length) noexcept : length(length)
		{
			GTSL_ASSERT(length <= CAPACITY, "Array is not big enough to insert the elements requested!")
		}

		constexpr Array(const Ranger<T>& ranger) noexcept : length(ranger.ElementCount())
		{
			GTSL_ASSERT(ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!")
			copyToData(ranger.begin(), ranger.ElementCount());
		}

		constexpr Array(const Ranger<const T>& ranger) noexcept : length(static_cast<uint32>(ranger.ElementCount()))
		{
			GTSL_ASSERT(ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!")
			copyToData(ranger.begin(), ranger.ElementCount());
		}

		constexpr Array(const Array& other) noexcept : length(other.length)
		{
			copyToData(other.data, other.length);
		}

		constexpr Array(Array&& other) noexcept : length(other.length)
		{
			copyToData(other.data, other.length);
			for (auto& e : other) { e.~T(); }
			other.length = 0;
		}

		constexpr Array& operator=(const Array& other) noexcept
		{
			copyToData(other.data, other.length);
			length = other.length;
			return *this;
		}

		constexpr Array& operator=(Array&& other) noexcept
		{
			copyToData(other.data, other.length);
			length = other.length;
			for (auto& e : other) { e.~T(); }
			other.length = 0;
			return *this;
		}

		~Array()
		{
			for (auto& e : *this)
			{
				e.~T();
			}
		}

		constexpr T& operator[](const uint32 i) noexcept
		{
			GTSL_ASSERT(i < CAPACITY, "Out of Bounds! Requested index is greater than the Array's statically allocated size!");
			return this->data[i];
		}

		constexpr const T& operator[](const uint32 i) const noexcept
		{
			GTSL_ASSERT(i < CAPACITY, "Out of Bounds! Requested index is greater than the Array's statically allocated size!");
			return this->data[i];
		}

		constexpr bool operator==(const Array& other)
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
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeTransferReference(obj));
			return this->length++;
		}

		constexpr uint32 PushBack(const Ranger<const T>& ranger) noexcept
		{
			GTSL_ASSERT(this->length + ranger.ElementCount() <= CAPACITY, "Array is not big enough to insert the elements requested!")
			copy(ranger.ElementCount(), ranger.begin(), this->data + this->length);
			auto ret = this->length += ranger.ElementCount();
			this->length += ranger.ElementCount();
			return ret;
		}

		constexpr uint32 Insert(const uint32 index, const Ranger<const T>& ranger) noexcept
		{
			copy(ranger.ElementCount(), ranger.begin(), this->data + index);
			auto ret = this->length += ranger.ElementCount();
			this->length += ranger.ElementCount();
			return ret;
		}

		template<typename... ARGS>
		constexpr uint32 EmplaceBack(ARGS&&... args)
		{
			GTSL_ASSERT((this->length + 1) <= CAPACITY, "Array is not long enough to insert any more elements!");
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeForwardReference<ARGS>(args) ...);
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
	};
}