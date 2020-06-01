#pragma once

#include "Core.h"

#include <initializer_list>
#include "Memory.h"
#include "Assert.h"
#include "Ranger.h"
#include <new>

namespace GTSL
{
	template <typename T, size_t CAPACITY, typename LT = uint32>
	class Array
	{
		byte data[CAPACITY * sizeof(T)];
		LT length = 0;

		constexpr void copyToData(const void* from, const LT length) noexcept
		{
			Memory::MemCopy(length * sizeof(T), from, this->data);
		}

		void copy(const LT length, const void* from, void* to)
		{
			Memory::MemCopy(length * sizeof(T), from, to);
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

		Ranger<T> GetRanger() { return Ranger<T>(this->length, this->begin()); }
		operator GTSL::Ranger<T>() const { return Ranger<T>(this->length, this->begin()); }

		constexpr Array() noexcept = default;

		constexpr Array(const std::initializer_list<T>& list) noexcept : length(list.size())
		{
			copyToData(list.begin(), this->length);
		}

		constexpr explicit Array(const LT length) noexcept : length(length)
		{
		}

		constexpr Array(const Ranger<T>& ranger) noexcept : length(ranger.ElementCount())
		{
			GTSL_ASSERT(ranger.ElementCount() < CAPACITY, "Array is not big enough to insert the elements requested!")
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

		constexpr T& operator[](const LT i) noexcept
		{
			GTSL_ASSERT(i < CAPACITY, "Out of Bounds! Requested index is greater than the Array's statically allocated size!");
			return reinterpret_cast<T&>(this->data[i]);
		}

		constexpr const T& operator[](const LT i) const noexcept
		{
			GTSL_ASSERT(i < CAPACITY, "Out of Bounds! Requested index is greater than the Array's statically allocated size!");
			return reinterpret_cast<T&>(const_cast<byte&>(this->data[i]));
		}

		constexpr bool operator==(const Array& other)
		{
			for (iterator begin = this->begin(), begin_other = other.begin(); begin != this->end(); ++begin)
			{
				if (*begin != *begin_other) { return false; }
			}
			return true;
		}

		constexpr T* GetData() noexcept { return reinterpret_cast<T*>(&this->data); }

		[[nodiscard]] constexpr const T* GetData() const noexcept { return reinterpret_cast<const T*>(this->data); }

		constexpr LT PushBack(const T& obj) noexcept
		{
			GTSL_ASSERT((this->length + 1) < CAPACITY, "Array is not long enough to insert any more elements!");
			::new(static_cast<void*>(this->data + this->length)) T(obj);
			return ++this->length;
		}

		constexpr LT PushBack(T&& obj) noexcept
		{
			GTSL_ASSERT((this->length + 1) < CAPACITY, "Array is not long enough to insert any more elements!");
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeTransferReference(obj));
			return ++this->length;
		}

		constexpr LT PushBack(const Ranger<T>& ranger) noexcept
		{
			GTSL_ASSERT(ranger.ElementCount() < CAPACITY, "Array is not big enough to insert the elements requested!")
			copy(ranger.ElementCount(), ranger.begin(), this->data + this->length);
			return this->length += ranger.ElementCount();
		}

		constexpr LT Insert(const LT index, const Ranger<T>& ranger) noexcept
		{
			copy(ranger.ElementCount(), ranger.begin(), this->data + index);
			return this->length += ranger.ElementCount();
		}

		template<typename... ARGS>
		constexpr LT EmplaceBack(ARGS&&... args)
		{
			GTSL_ASSERT((this->length + 1) < CAPACITY, "Array is not long enough to insert any more elements!");
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeForwardReference<ARGS>(args) ...);
			return ++this->length;
		}

		constexpr void Resize(LT size)
		{
			GTSL_ASSERT(size < CAPACITY, "Requested size for array Resize is greater than Array's statically allocated size!");
			this->length = size;
		}

		constexpr void PopBack()
		{
			GTSL_ASSERT(this->length != 0, "Array's length is already 0. Cannot pop any more elements!");
			reinterpret_cast<T&>(this->data[this->length]).~T();
			--this->length;
		}

		[[nodiscard]] constexpr LT GetLength() const noexcept { return this->length; }

		[[nodiscard]] constexpr LT GetCapacity() const noexcept { return CAPACITY; }
	};
}