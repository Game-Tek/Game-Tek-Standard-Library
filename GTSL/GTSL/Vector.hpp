#pragma once

#include "Core.h"

#include "Memory.h"
#include <initializer_list>
#include "Assert.h"
#include "Allocator.h"
#include <new>

#include "Ranger.h"

namespace GTSL
{
	template <typename T>
	class Vector
	{
	public:
		typedef uint32 length_type;

		typedef T* iterator;
		typedef const T* const_iterator;
		typedef T value_type;

		Vector() = default;

		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements.
		 * \param capacity Number of T objects to allocate space for.
		 */
		explicit Vector(const length_type capacity, const AllocatorReference& allocatorReference) : capacity(capacity), length(0),  data(this->allocate(this->capacity, allocatorReference))
		{
		}

		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements, and considers length elements already occupied.
		 * \param capacity Number of T objects to allocate space for.
		 * \param length Number of elements to consider being already placed in vector.
		 */
		explicit Vector(const length_type capacity, const length_type length, const AllocatorReference& allocatorReference) : capacity(capacity), length(length), data(this->allocate(this->capacity, allocatorReference))
		{
		}

		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements, and considers length elements already occupied.
		 * \param capacity Number of T objects to allocate space for.
		 * \param length Number of elements to consider being already placed in vector.
		 */
		explicit Vector(const GTSL::Ranger<const T>& ranger, const AllocatorReference& allocatorReference) : capacity(ranger.ElementCount()), length(ranger.ElementCount()), data(this->allocate(this->capacity, allocatorReference))
		{
		}

		/**
		 * \brief Constructs a Vector and copies it's elements from initializerList.
		 * \param initializerList Initializer list to take elements from.
		 */
		constexpr Vector(const std::initializer_list<T>& initializerList) : capacity(initializerList.end() - initializerList.begin()), length(this->capacity), data(this->allocate(this->capacity))
		{
			copyArray(initializerList.begin(), this->data, this->length);
		}

		/**
		 * \brief Constructs a Vector from a reference to another Vector.
		 * \param other Reference to another Vector.
		 */
		Vector(const Vector& other, const AllocatorReference& allocatorReference) : capacity(other.capacity), length(other.length), data(this->allocate(this->capacity, allocatorReference))
		{
			copyArray(other.data, this->data, this->length);
		}

		/**
		 * \brief Constructs a Vector from an r-value reference to another Vector.
		 * \param other R-Value reference to other Vector to transfer from.
		 */
		Vector(Vector&& other) noexcept : capacity(other.capacity), length(other.length), data(other.data)
		{
			other.data = nullptr;
		}

		/**
		 * \brief Copies the data of other Vector to this Vector.
		 * \param other Reference to other Vector.
		 * \return Reference to self.
		 */
		Vector& operator=(const Vector& other)
		{
			GTSL_ASSERT(this != &other, "Assigning to self is not allowed!");
			reallocateIfExceeds(other.length - this->length);
			copyArray(other.data, this->data, other.length);
			this->length = other.length;
			return *this;
		}

		/**
		 * \brief Transfers the data of other Vector to this Vector.
		 * \param other Reference to other Vector.
		 * \return Reference to self.
		 */
		Vector& operator=(Vector&& other) noexcept
		{
			GTSL_ASSERT(this != &other, "Assigning to self is not allowed!");
			capacity = other.capacity;
			length = other.length;
			data = other.data;
			other.data = nullptr;
			return *this;
		}

		void Free(const AllocatorReference& allocatorReference)
		{
			freeData(allocatorReference);
		}
		
		~Vector()
		{
			for (auto& e : *this) { e.~T(); }
			GTSL_ASSERT(!this->data, "Data was not freed!")
		}
		
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
		
		/**
		 * \brief Resizes the array(if necessary, does not reallocate for downsize) to accomodate count T elements.
		 * \param count Number of new T elements to accomodate.
		 */
		void Resize(const length_type count, const AllocatorReference& allocatorReference)
		{
			if (this->length + count > this->capacity) { reallocate(count, allocatorReference); }
			this->length = count;
		}

		void ResizeDown(const length_type count) { this->length = count; }
		
		/**
		 * \brief Initializes the Vector with space for count elements. Useful for when Vector was initialized with no allocation. Calling this function when the array is not Empty will lead to a memory leak.
		 * \param count Number of T elements to allocate space for.
		 */
		void Initialize(const length_type count, const AllocatorReference& allocatorReference)
		{
			GTSL_ASSERT(!this->data, "Array pointer is not null!")
			this->data = allocate(count, allocatorReference);
			this->length = 0;
		}
		
		/**
		* \brief Initializes the Vector with space for count elements and copies data to it's array. Useful for when Vector was initialized with no allocation. Calling this function when the array is not Empty will lead to a memory leak.
		*/
		void Initialize(const GTSL::Ranger<const T>& ranger, const AllocatorReference& allocatorReference)
		{
			this->data = allocate(ranger.ElementCount(), allocatorReference);
			copyArray(ranger.begin(), this->data, ranger.ElementCount());
			this->length = ranger.ElementCount();
		}

		/**
		 * \brief Downsizes the array to fit count elements.
		 * \param count Number of T type to leave space for.
		 */
		void Shrink(const length_type count, const AllocatorReference& allocatorReference)
		{
			this->length = count;
			T* buffer = allocate(this->capacity);
			copyArray(this->data, buffer, this->length);
			freeData(allocatorReference);
			this->data = buffer;
		}

		/**
		 * \brief Places a copy of obj At the back of the vector.
		 * \param obj Object to Insert back.
		 * \return Length of vector after inserting, also index At which obj was inserted.
		 */
		length_type PushBack(const T& obj, const AllocatorReference& allocatorReference)
		{
			if (this->length + 1 > this->capacity) { reallocate(1, allocatorReference); }
			::new(static_cast<void*>(this->data + this->length)) T(obj);
			return this->length++;
		}

		/**
		 * \brief Places a copy of obj At the back of the vector.
		 * \param obj Object to Insert back.
		 * \return Length of vector after inserting, also index At which obj was inserted.
		 */
		length_type PushBack(T&& obj, const AllocatorReference& allocatorReference)
		{
			if (this->length + 1 > this->capacity) { reallocate(1, allocatorReference); }
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeTransferReference(obj));
			return this->length++;
		}

		length_type PushBack(const Ranger<T>& ranger, const AllocatorReference& allocatorReference)
		{
			if (this->length + ranger.ElementCount() > this->capacity) { reallocate(ranger.ElementCount(), allocatorReference); }
			copyArray(ranger.begin(), getIterator(this->length), ranger.ElementCount());
			return static_cast<uint32>((this->length += ranger.ElementCount()) - ranger.ElementCount());
		}

		length_type PushBack(const Ranger<const T>& ranger, const AllocatorReference& allocatorReference)
		{
			if (this->length + ranger.ElementCount() > this->capacity) { reallocate(ranger.ElementCount(), allocatorReference); }
			copyArray(ranger.begin(), getIterator(this->length), ranger.ElementCount());
			return static_cast<uint32>((this->length += ranger.ElementCount()) - ranger.ElementCount());
		}
		
		/**
		 * \brief Places a copy of the elements from the passed in Vector At the end of the vector.
		 * \param other Vector to copy elements from.
		 * \return Length of vector after inserting other's elements.
		 */
		length_type PushBack(const Vector& other, const AllocatorReference& allocatorReference)
		{
			if (this->length + (this->length - other.length) > this->capacity) { reallocate(other.GetLength(), allocatorReference); }
			copyArray(other.data, getIterator(this->length), other.length);
			return (this->length += other.length) - other.length;
		}

		/**
		 * \brief Emplaces(constructs) a T type object At the end of the vector and forwards it's arguments.
		 * \tparam ARGS Template type/s of args.
		 * \param args Arguments to be forward to the T object being constructed At the end of the vector.
		 * \return Length of vector after inserting, also index At which the object was inserted.
		 */
		template <typename... ARGS>
		length_type EmplaceBack(const AllocatorReference& allocatorReference, ARGS&&... args)
		{
			if (this->length + 1 > this->capacity) { reallocate(1, allocatorReference); }
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MakeForwardReference<ARGS>(args)...);
			return this->length++;
		}

		/**
		 * \brief Destroys the object At the end of the vector and frees one space.
		 */
		void PopBack()
		{
			GTSL_ASSERT(this->length != 0, "Cannot pop back, there are no more elements!")
			this->data[this->length].~T();
			this->length -= 1;
		}

		/**
		 * \brief Copies the passed in element At index and shifts the rest of the vector forward to fit it in.
		 * \param index Index At which to Place the object.
		 * \param obj Object to copy.
		 * \return Returns the length of the vector after inserting.
		 */
		template<typename... ARGS>
		length_type Insert(const AllocatorReference& allocatorReference, const length_type index, ARGS&&... args)
		{
			if (this->length + 1 > this->capacity) { reallocate(1, allocatorReference); }
			copyArray(getIterator(index), getIterator(index + 1), this->length - index);
			::new(static_cast<void*>(this->data + this->length)) T(MakeForwardReference<ARGS>(args)...);
			return this->length += 1;
		}

		/**
		 * \brief Copies obj to index.
		 * \param index Index At which to Place the object.
		 * \param obj Object to copy.
		 */
		void Place(const length_type index, const T& obj)
		{
			this->data[index].~T();
			::new(static_cast<void*>(this->data + index)) T(obj);
		}

		/**
		 * \brief Copies obj to index.
		 * \param index Index At which to Place the object.
		 * \param obj Object to copy.
		 */
		void Insert(const length_type index, const GTSL::Ranger<const T> ranger, const AllocatorReference& allocatorReference)
		{
			if (this->length + ranger.ElementCount() > this->capacity) { reallocate(ranger.ElementCount(), allocatorReference);	}
			MemCopy(sizeof(T) * ranger.ElementCount(), getIterator(index), getIterator(index + ranger.ElementCount()));
			this->length += ranger.ElementCount();
			MemCopy(sizeof(T) * ranger.ElementCount(), ranger, getIterator(index));
		}

		/**
		 * \brief Emplaces(constructs) an object At index.
		 * \tparam ARGS Type/s of args.
		 * \param index Index At which to Emplace the object.
		 * \param args Argument list to forward to the object constructor.
		 */
		template <typename... ARGS>
		void Emplace(const length_type index, ARGS&&... args)
		{
			this->data[index].~T();
			::new(static_cast<void*>(this->data + index)) T(GTSL::MakeForwardReference<ARGS>(args) ...);
		}

		/**
		 * \brief Destroys the element a index.
		 * \param index Index of the element to Destroy.
		 */
		void Destroy(const length_type index) { this->data[index].~T(); }

		/**
		 * \brief Removes the elements At index and moves the rest of the vector to fill the space.
		 * \param index Index of the element to Pop.
		 */
		void Pop(const length_type index)
		{
			this->data[index].~T();
			copyArray(getIterator(index + 1), getIterator(index), this->length - index);
			this->length -= 1;
		}

		/**
		 * \brief Removes all elements from index to index + length and moves the rest of the vector to fill the space.
		 * \param index Index At which to start popping.
		 * \param length Number of elements to Pop.
		 */
		void Pop(const length_type index, const length_type length)
		{
			for (auto& begin = this->data[index]; begin != this->data[index + length]; ++begin)
			{
				begin.~T();
			}

			copyArray(getIterator(index + length), getIterator(index), this->length - index);
			this->length -= length;
		}

		/**
		 * \brief Looks for an object inside the vector.
		 * \param obj Object to look for.
		 * \return Iterator to element if it was found, iterator to end if object was not found.
		 */
		iterator Find(const T& obj) noexcept
		{
			for (auto& e : *this) { if (obj == e) { return this->begin(); } }

			return this->end();
		}

		/**
		 * \brief Returns a reference to the element at index.
		 * \param index Index to element.
		 * \return Reference to the element at index.
		 */
		T& operator[](const length_type index) noexcept
		{
			GTSL_ASSERT(index < this->length, "Entered index is not accessible, array is not as large.")
			return this->data[index];
		}

		/**
		 * \brief Returns a reference to the element at index.
		 * \param index Index to element.
		 * \return Const reference to the element at index.
		 */
		const T& operator[](const length_type index) const noexcept
		{
			GTSL_ASSERT(index < this->length, "Entered index is not accessible, array is not as large.")
			return this->data[index];
		}

		/**
		 * \brief Returns a reference to the element at index.
		 * \param index Index to element.
		 * \return Reference to the element at index.
		 */
		T& At(const length_type index) noexcept { return this->data[index]; }

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

		/**
		 * \brief Return the number of remaining slots sin the vector.
		 * \return Remaining slots in the vector.
		 */
		[[nodiscard]] length_type GetRemainingLength() const noexcept { return this->capacity - this->length; }
		
		/**
		 * \brief Return a pointer to the internal array.
		 * \return Pointer to the internal array.
		 */
		T* GetData() noexcept { return this->data; }

		/**
		 * \brief Return a const pointer to the internal array.
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

		/**
		 * \brief Returns the size(bytes) of the remaining free slots.
		 * \return Size(bytes) of the remaining free slots.
		 */
		[[nodiscard]] size_t GetRemainingLengthSize() const noexcept { return this->GetRemainingLength() * sizeof(T); }

		[[nodiscard]] operator Ranger<T>() noexcept { return Ranger<T>(this->data, this->data + this->length); }
		[[nodiscard]] operator Ranger<const T>() const noexcept { return Ranger<const T>(this->data, this->data + this->length); }

	private:
		length_type capacity{ 0 };
		length_type length{ 0 };
		T* data{ nullptr };

		/**
		 * \brief Copies data from from to to.
		 * \param from Pointer from where to grab the data.
		 * \param to Pointer to write the data to.
		 * \param elementCount How many elements of this vector's T to write to to.
		 */
		static void copyArray(const T* from, T* to, const length_type elementCount)
		{
			MemCopy(elementCount * sizeof(T), from, to);
		}

		/**
		 * \brief Allocates a new a array of type T with enough space to hold elementCount elements.
		 * \param elementCount How many elements of this vector's T to allocate space for.
		 * \return T pointer to the newly allocated memory.
		 */
		T* allocate(const length_type elementCount, const AllocatorReference& allocatorReference)
		{
			T* data{ nullptr };
			uint64 allocated_size{ 0 };
			allocatorReference.Allocate(elementCount * sizeof(T), alignof(T), reinterpret_cast<void**>(&data), &allocated_size);
			this->capacity = static_cast<length_type>(allocated_size / sizeof(T));
			return data;
		}

		/**
		 * \brief Deletes the data found At this vector's data and sets data as nullptr.
		 */
		void freeData(const AllocatorReference& allocatorReference)
		{
			if (this->data)
			{
				GTSL_ASSERT(this->data != nullptr, "Data is nullptr.")
					allocatorReference.Deallocate(this->capacity * sizeof(T), alignof(T), this->data);
				this->data = nullptr;
			}
		}

		/**
		 * \brief Reallocates this->data to a new array if (this->length + additionalElements) exceeds the allocated space.\n
		 * Also deletes the data found At the old data.
		 * Growth of the array is geometric.
		 * \param additionalElements How many elements of this vector's T are you trying to check if fit in the already allocated array.\n
		 * Number can be negative.
		 */
		void reallocate(const int64 additionalElements, const AllocatorReference& allocatorReference)
		{
			const length_type new_capacity = this->length * 2;
			T* new_data = this->allocate(new_capacity, allocatorReference);
			copyArray(this->data, new_data, this->capacity);
			GTSL_ASSERT(this->data != nullptr, "Data is nullptr.")
				allocatorReference.Deallocate(this->capacity * sizeof(T), alignof(T), this->data);
			this->data = new_data;
		}

		/**
		 * \brief Returns an iterator to an specified index. DOES NOT CHECK FOR BOUNDS, but underlying getter does, only in debug builds.
		 * \param index Index to the element to be retrieved.
		 * \return iterator to the element At index.
		 */
		iterator getIterator(const length_type index) noexcept { return &this->data[index]; }
	};
}
