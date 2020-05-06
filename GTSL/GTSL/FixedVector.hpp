#pragma once

#include "Core.h"
#include "Assert.h"
#include "Allocator.h"
#include <initializer_list>
#include "Memory.h"
#include <new>

#include "Ranger.h"

namespace GTSL
{
	template <typename T>
	class FixedVector final
	{
	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef uint32 length_type;
	private:
		length_type capacity{ 0 };
		length_type length{ 0 };
		AllocatorReference* allocatorReference{ nullptr };
		T* data{ nullptr };

		constexpr T* allocate(const length_type _elements)
		{
			T* data{ nullptr };
			uint64 allocatedSize{ 0 };
			this->allocatorReference->Allocate(sizeof(T) * _elements, alignof(T), reinterpret_cast<void**>(&data), &allocatedSize);
			this->capacity = static_cast<length_type>(allocatedSize);
			return data;
		}

		void copyLength(const length_type elements, const void* from)
		{
#undef CopyMemory
			Memory::CopyMemory(sizeof(T) * elements, from, this->data);
		}

		void freeArray()
		{
			this->allocatorReference->Deallocate(this->capacity, alignof(T), this->data);
			this->data = nullptr;
		}

	public:

		/**
		 * \brief Returns an iterator to the beginning of the array.
		 * \return Iterator to the beginning of the array.
		 */
		[[nodiscard]] iterator begin() noexcept { return this->data; }

		/**
		 * \brief Returns an iterator to the end of the array.
		 * \return Iterator to the end of the array.
		 */
		[[nodiscard]] iterator end() noexcept { return &this->data[this->length]; }

		/**
		 * \brief Returns a const_iterator to the beginning of the array.
		 * \return Const_Iterator to the beginning of the array.
		 */
		[[nodiscard]] const_iterator begin() const noexcept { return this->data; }

		/**
		 * \brief Returns a const_iterator to the end of the array.
		 * \return Const_Iterator to the end of the array.
		 */
		[[nodiscard]] const_iterator end() const noexcept { return &this->data[this->length]; }

		/**
		 * \brief Returns a reference to the first element.
		 * \return Reference to the first element.
		 */
		T& front() noexcept { return this->data[0]; }

		/**
		 * \brief Returns a reference to the last element.
		 * \return Reference to the last element.
		 */
		T& back() noexcept { return this->data[this->length]; }

		/**
		 * \brief Returns a const reference to the first element.
		 * \return Const reference to the first element.
		 */
		[[nodiscard]] const T& front() const noexcept { return this->data[0]; }

		/**
		 * \brief Returns a const reference to the last element.
		 * \return Const reference to the last element.
		 */
		[[nodiscard]] const T& back() const noexcept { return this->data[this->length]; }

		operator Ranger<T>() const { return Ranger<T>(this->length, this->data); }
		
		[[nodiscard]] AllocatorReference* GetAllocatorReference() const { return this->allocatorReference; }
		
		FixedVector() = default;

		/**
		 * \brief Constructs a FixedVector with enough space to accomodate capacity T elements.
		 * \param capacity Number of T objects to allocate space for.
		 */
		explicit FixedVector(const length_type capacity, AllocatorReference* allocatorReference) : capacity(capacity), length(0), allocatorReference(allocatorReference), data(this->allocate(capacity))
		{
		}

		/**
		 * \brief Constructs a FixedVector with enough space to accomodate capacity T elements, and considers length elements already occupied.
		 * \param capacity Number of T objects to allocate space for.
		 * \param length Number of elements to consider being already placed in vector.
		 */
		explicit FixedVector(const length_type capacity, const length_type length, AllocatorReference* allocatorReference) : capacity(capacity), length(length), allocatorReference(allocatorReference), data(this->allocate(this->capacity))
		{
		}

		/**
		 * \brief Constructs a FixedVector with enough space to accomodate length T elements, and considers length elements already occupied.
		 * \param length Number T elements to copy from array.
		 * \param array Pointer to an array of type T elements.
		 */
		FixedVector(const length_type length, const T array[], AllocatorReference* allocatorReference) : capacity(length), length(length), allocatorReference(allocatorReference), data(this->allocate(this->capacity))
		{
			copyToData(array, length);
		}

		/**
		 * \brief Constructs a FixedVector that takes ownership of the data at array.
		 * \param length Number of T elements in array.
		 * \param array Pointer to an array of type T elements.
		 */
		FixedVector(const length_type length, const T* array[], AllocatorReference* allocatorReference) : capacity(length), length(length), allocatorReference(allocatorReference), data(*array)
		{
		}

		/**
		 * \brief Constructs a FixedVector that takes ownership of the data at array.
		 * \param capacity Number of elements available at array.
		 * \param length Number of occupied T elements at array.
		 * \param array Pointer to an array of type T elements.
		 */
		FixedVector(const length_type capacity, const length_type length, const T* array[], AllocatorReference* allocatorReference) : capacity(capacity), length(length), allocatorReference(allocatorReference), data(*array)
		{
		}

		constexpr FixedVector(const std::initializer_list<T>& list) : capacity(list.size()), length(list.size()), data(this->allocate(list.size()))
		{
			copyLength(this->length, list.begin());
		}

		/**
		 * \brief Constructs a FixedVector from an start and end iterators(pointers).
		 * \param start Iterator from where to start copying.
		 * \param end Iterator to where to stop copying.
		 */
		FixedVector(const_iterator start, const_iterator end, AllocatorReference* allocatorReference) : capacity(end - start), length(this->capacity), allocatorReference(allocatorReference), data(this->allocate(this->capacity))
		{
			copyLength(this->length, start);
		}

		/**
		 * \brief Constructs a FixedVector from a reference to another FixedVector.
		 * \param other Reference to another Vector.
		 */
		FixedVector(const FixedVector<T>& other) : capacity(other.capacity), length(other.length), data(this->allocate(this->capacity))
		{
			copyLength(this->capacity, other.data);
		}

		/**
		 * \brief Constructs a FixedVector from an r-value reference to another FixedVector.
		 * \param other R-Value reference to other FixedVector to transfer from.
		 */
		FixedVector(FixedVector&& other) noexcept : capacity(other.capacity), length(other.length), data(other.data)
		{
			other.capacity = 0;
			other.length = 0;
			other.data = nullptr;
		}

		~FixedVector()
		{
			for (auto& e : *this) { e.~T(); }
			freeArray();
		}

		FixedVector& operator=(const FixedVector<T>& other)
		{
			freeArray();
			this->capacity = other.capacity;
			this->length = other.length;
			this->data = allocate(this->capacity);
			copyLength(this->capacity, other.data);
			return *this;
		}

		FixedVector& operator=(FixedVector&& other) noexcept
		{
			this->capacity = other.capacity;
			this->length = other.length;
			this->data = other.data;
			other.capacity = 0;
			other.length = 0;
			other.data = nullptr;
			return *this;
		}

		/**
		* \brief Returns a reference to the element at index.
		* \param index Index to element.
		* \return Reference to the element at index.
		*/
		T& operator[](const length_type index) noexcept
		{
			GTSL_ASSERT(index > this->capacity, "Out of Bounds! Requested index is greater than the array's allocated(current) size!");
			return this->data[index];
		}

		/**
		 * \brief Returns a const reference to the element at index.
		 * \param index Index to element.
		 * \return Const reference to the element at index.
		 */
		const T& operator[](const length_type index) const noexcept
		{
			GTSL_ASSERT(index > this->capacity, "Out of Bounds! Requested index is greater than the array's allocated(current) size!");
			return this->data[index];
		}

		/**
		* \brief Places a copy of obj At the back of the vector.
		* \param obj Object to Insert back.
		* \return Length of vector after inserting, also index At which obj was inserted.
		*/
		length_type PushBack(const T& obj)
		{
			::new(static_cast<void*>(this->data + this->length)) T(obj);
			return ++this->length;
		}

		/**
		* \brief Places a copy of obj At the back of the vector.
		* \param obj Object to Insert back.
		* \return Length of vector after inserting, also index At which obj was inserted.
		*/
		length_type PushBack(T&& obj)
		{
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeTransferReference(obj));
			return ++this->length;
		}

		template<typename ...ARGS>
		length_type EmplaceBack(ARGS&&... args)
		{
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeForwardReference<ARGS>(args)...);
			return ++this->length;
		}

		/**
		* \brief Returns the number of occupied slots.
		* \return Number of occupied slots.
		*/
		[[nodiscard]] length_type GetLength() const noexcept { return this->length; }

		/**
		 * \brief Returns the number of allocated slots.
		 * \return Number of allocated slots.
		 */
		[[nodiscard]] length_type GetCapacity() const noexcept { return this->capacity; }


		void Resize(const length_type length) noexcept
		{
			GTSL_ASSERT(length > this->length, "New length cannot be more than allocated length.");
			this->length = length;
		}

		/**
		* \brief Return a pointer to the internal array.
		* \return Pointer to the internal array.
		*/
		T* GetData() noexcept { return this->data; }

		/**
		 * \brief Return a pointer to the internal array.
		 * \return Const pointer to the internal array.
		 */
		[[nodiscard]] const T* GetData() const noexcept { return this->data; }

		/**
		 * \brief Returns the size(bytes) of the occupied slots.
		 * \return Size of occupied slots.
		 */
		[[nodiscard]] size_t GetLengthSize() const noexcept { return this->length * sizeof(T); }

		/**
		 * \brief Returns the size(bytes) of the allocated slots.
		 * \return Size of allocated slots.
		 */
		[[nodiscard]] size_t GetCapacitySize() const noexcept { return this->capacity * sizeof(T); }
	};
}
