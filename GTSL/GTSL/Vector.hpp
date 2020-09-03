#pragma once

#include "Core.h"

#include "Memory.h"
#include "Assert.h"
#include "Allocator.h"
#include <initializer_list>
#include <new>
#include "Ranger.h"

namespace GTSL
{
	template <typename T, class ALLOCATOR>
	class Vector
	{
	public:
		typedef uint32 length_type;

		typedef T* iterator;
		typedef const T* const_iterator;
		typedef T value_type;

		Vector() = default;

		explicit Vector(const ALLOCATOR& allocator) : allocator(allocator) {}
		
		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements.
		 * \param capacity Number of T objects to allocate space for.
		 */
		explicit Vector(const length_type capacity, const ALLOCATOR& allocatorReference) : allocator(allocatorReference),
		capacity(capacity), data(this->allocate(this->capacity, this->capacity))
		{
		}

		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements, and considers length elements already occupied.
		 * \param capacity Number of T objects to allocate space for.
		 * \param length Number of elements to consider being already placed in vector.
		 */
		explicit Vector(const length_type capacity, const length_type length, const ALLOCATOR& allocatorReference) : allocator(allocatorReference), capacity(capacity),
		length(length), data(this->allocate(this->capacity, this->capacity))
		{
		}

		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements, and considers length elements already occupied.
		 */
		explicit Vector(const GTSL::Ranger<const T>& ranger, const ALLOCATOR& allocatorReference) : allocator(allocatorReference),
		capacity(static_cast<uint32>(ranger.ElementCount())), length(static_cast<uint32>(ranger.ElementCount())), data(this->allocate(this->capacity, this->capacity))
		{
		}

		/**
		 * \brief Constructs a Vector and copies it's elements from initializerList.
		 * \param initializerList Initializer list to take elements from.
		 */
		constexpr Vector(const std::initializer_list<T>& initializerList) : capacity(static_cast<uint32>(initializerList.end() - initializerList.begin())),
		length(this->capacity), data(this->allocate(this->capacity, this->capacity))
		{
			copyArray(initializerList.begin(), this->data, this->length);
		}

		/**
		 * \brief Constructs a Vector from a reference to another Vector.
		 * \param other Reference to another Vector.
		 */
		Vector(const Vector& other) : allocator(other.allocator), capacity(other.capacity),
		length(other.length), data(this->allocate(this->capacity, this->capacity))
		{
			for (auto& e : other) { ::new(this->data + GTSL::RangeForIndex(e, other)) T(e); }
		}

		/**
		 * \brief Constructs a Vector from a reference to another Vector.
		 * \param other Reference to another Vector.
		 */
		template<class ALLOC>
		Vector(Vector<T, ALLOC>&& other, const ALLOCATOR& allocatorReference) : allocator(allocatorReference), capacity(other.capacity), length(other.length), data(other.data)
		{
			other.data = nullptr;
		}

		/**
		 * \brief Constructs a Vector from a reference to another Vector.
		 * \param other Reference to another Vector.
		 */
		template<class ALLOC>
		Vector(const Vector<T, ALLOC>& other, const ALLOCATOR& allocatorReference) : allocator(allocatorReference), capacity(other.capacity),
		length(other.length), data(this->allocate(this->capacity, this->capacity))
		{
			for (auto& e : other) { ::new(this->data + GTSL::RangeForIndex(e, other)) T(e); }
		}

		/**
		 * \brief Constructs a Vector from an r-value reference to another Vector.
		 * \param other R-Value reference to other Vector to transfer from.
		 */
		Vector(Vector&& other) noexcept : allocator(GTSL::MoveRef(other.allocator)), capacity(other.capacity), length(other.length), data(other.data)
		{
			other.data = nullptr;
		}

		/**
		 * \brief Copies the data of other Vector to this Vector.
		 * \param other Reference to other Vector.
		 * \return Reference to self.
		 */
		template<class OTHER_ALLOCATOR>
		Vector& operator=(const Vector<T, OTHER_ALLOCATOR>& other)
		{
			GTSL_ASSERT(this != &other, "Assigning to self is not allowed!");
			if (other.length > this->length) { reallocate(); };
			for (auto& e : other) { this->data[GTSL::RangeForIndex(e, other)] = e; }
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
		
		~Vector()
		{
			if(this->data) [[likely]]
			{
				for (auto& e : *this) { e.~T(); }
				deallocate();
			}
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
		T& back() noexcept { return this->data[this->length - 1]; }

		/**
		 * \brief Returns a const reference to the first element.
		 * \return Const reference to the first element.
		 */
		[[nodiscard]] const T& front() const noexcept { return this->data[0]; }

		/**
		 * \brief Returns a const reference to the last element.
		 * \return Const reference to the last element.
		 */
		[[nodiscard]] const T& back() const noexcept { return this->data[this->length - 1]; }
		
		/**
		 * \brief Resizes the array(if necessary, does not reallocate for downsize) to accomodate count T elements.
		 * \param count Number of new T elements to accomodate.
		 */
		void Resize(const length_type count)
		{
			if (this->length + count > this->capacity) { reallocate(); }
			this->length = count;
		}

		void ResizeDown(const length_type count)
		{
			for (auto* begin = this->begin() + count; begin != this->end(); ++begin) { begin->~T(); }
			this->length = count;
		}
		
		/**
		 * \brief Initializes the Vector with space for count elements. Useful for when Vector was initialized with no allocation. Calling this function when the array is not Empty will lead to a memory leak.
		 * \param count Number of T elements to allocate space for.
		 */
		void Initialize(const length_type count)
		{
			GTSL_ASSERT(!this->data, "Array pointer is not null!")
			this->data = allocate(count, this->capacity);
			this->length = 0;
		}

		void Initialize(const length_type count, const ALLOCATOR& allocator)
		{
			GTSL_ASSERT(!this->data, "Array pointer is not null!")
			this->allocator = allocator;
			this->data = allocate(count, this->capacity);
			this->length = 0;
		}
		
		/**
		* \brief Initializes the Vector with space for count elements and copies data to it's array. Useful for when Vector was initialized with no allocation. Calling this function when the array is not Empty will lead to a memory leak.
		*/
		void Initialize(const GTSL::Ranger<const T>& ranger, const ALLOCATOR& allocatorReference)
		{
			this->allocator = allocatorReference;
			this->data = allocate(ranger.ElementCount(), this->capacity);
			copyArray(ranger.begin(), this->data, ranger.ElementCount());
			this->length = ranger.ElementCount();
		}

		/**
		 * \brief Downsizes the array to fit count elements.
		 * \param count Number of T type to leave space for.
		 */
		void Shrink(const length_type count)
		{
			for (auto begin = this->begin() + count; begin != this->end(); ++begin) { begin->~T(); }
			length_type new_capacity;
			T* buffer = allocate(count, new_capacity);
			copyArray(this->data, buffer, this->length);
			deallocate();
			this->capacity = new_capacity; this->data = buffer; this->length = count;
		}

		/**
		 * \brief Places a copy of obj At the back of the vector.
		 * \param obj Object to Insert back.
		 * \return Length of vector after inserting, also index At which obj was inserted.
		 */
		length_type PushBack(const T& obj)
		{
			if (this->length + 1 > this->capacity) [[unlikely]] { reallocate(); }
			::new(static_cast<void*>(this->data + this->length)) T(obj);
			return this->length++;
		}

		/**
		 * \brief Places a copy of obj At the back of the vector.
		 * \param obj Object to Insert back.
		 * \return Index At which obj was inserted.
		 */
		length_type PushBack(T&& obj)
		{
			if (this->length + 1 > this->capacity) [[unlikely]] { reallocate(); }
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::MoveRef(obj));
			return this->length++;
		}

		length_type PushBack(const Ranger<T>& ranger)
		{
			if (this->length + ranger.ElementCount() > this->capacity) { reallocate(); }
			for (auto& e : ranger) { this->data[GTSL::RangeForIndex(e, ranger)] = e; }
			return static_cast<uint32>((this->length += static_cast<uint32>(ranger.ElementCount())) - ranger.ElementCount());
		}

		length_type PushBack(const Ranger<const T>& ranger)
		{
			if (this->length + ranger.ElementCount() > this->capacity) { reallocate(); }
			for (auto& e : ranger) { this->data[GTSL::RangeForIndex(e, ranger)] = e; }
			return static_cast<uint32>((this->length += static_cast<uint32>(ranger.ElementCount())) - ranger.ElementCount());
		}

		/**
		 * \brief Places a copy of the elements from the passed in Vector At the end of the vector.
		 * \param other Vector to copy elements from.
		 * \return Length of vector after inserting other's elements.
		 */
		template<class ALLOC>
		void PushBack(const Vector<T, ALLOC>& other)
		{
			const auto new_length = this->length + other.GetLength();
			if (this->length + (this->length - other.length) > this->capacity) { reallocate(); }
			for (uint32 i = this->length, o_i = 0; o_i < other.GetLength(); ++i, ++o_i) { this->data[i] = other.data[o_i]; }
			this->length = new_length;
		}

		/**
		 * \brief Emplaces(constructs) a T type object At the end of the vector and forwards it's arguments.
		 * \tparam ARGS Template type/s of args.
		 * \param args Arguments to be forward to the T object being constructed At the end of the vector.
		 * \return Length of vector after inserting, also index At which the object was inserted.
		 */
		template <typename... ARGS>
		length_type EmplaceBack(ARGS&&... args)
		{
			if (this->length + 1 > this->capacity) [[unlikely]] { reallocate(); }
			::new(static_cast<void*>(this->data + this->length)) T(GTSL::ForwardRef<ARGS>(args)...);
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
		 * \return Returns the length of the vector after inserting.
		 */
		template<typename... ARGS>
		length_type Insert(const length_type index, ARGS&&... args)
		{
			if (this->length + 1 > this->capacity) [[unlikely]] { reallocate(); }
			copyArray(getIterator(index), getIterator(index + 1), this->length - index);
			::new(this->data + this->length) T(ForwardRef<ARGS>(args)...);
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
		 */
		void Insert(const length_type index, const GTSL::Ranger<const T> ranger)
		{
			if (this->length + ranger.ElementCount() > this->capacity) { reallocate(); }
			MemCopy(sizeof(T) * ranger.ElementCount(), getIterator(index), getIterator(static_cast<uint32>(index + ranger.ElementCount())));
			this->length += static_cast<uint32>(ranger.ElementCount());
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
			::new(static_cast<void*>(this->data + index)) T(GTSL::ForwardRef<ARGS>(args) ...);
		}

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
		const_iterator Find(const T& obj) const noexcept
		{
			for (const auto& e : *this) { if (obj == e) { return &e; } } return this->end();
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
		[[nodiscard]] operator Ranger<T>() const noexcept { return Ranger<T>(this->data, this->data + this->length); }
		[[nodiscard]] operator Ranger<const T>() const noexcept { return Ranger<const T>(this->data, this->data + this->length); }

	private:
#pragma warning(disable : 4648)
		[[no_unique_address]] ALLOCATOR allocator;
		length_type capacity{ 0 };
		length_type length{ 0 };
		T* data{ nullptr };
#pragma warning(default : 4648)

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
		T* allocate(const length_type elementCount, length_type& newCapacity)
		{
			T* data{ nullptr }; uint64 allocated_size{ 0 };
			this->allocator.Allocate(elementCount * sizeof(T), alignof(T), reinterpret_cast<void**>(&data), &allocated_size);
			newCapacity = static_cast<length_type>(allocated_size / sizeof(T));
			return data;
		}

		/**
		 * \brief Deletes the data found At this vector's data and sets data as nullptr.
		 */
		void deallocate() { this->allocator.Deallocate(this->capacity * sizeof(T), alignof(T), static_cast<void*>(this->data)); }

		/**
		 * \brief Reallocates this->data to a new array if (this->length + additionalElements) exceeds the allocated space.\n
		 * Also deletes the data found At the old data.
		 * Growth of the array is geometric.
		 * Number can be negative.
		 */
		void reallocate()
		{
			length_type new_capacity = this->length * 2;
			T* new_data = this->allocate(new_capacity, new_capacity);
			copyArray(this->data, new_data, this->length);
			deallocate();
			this->data = new_data; this->capacity = new_capacity;
		}

		/**
		 * \brief Returns an iterator to an specified index. DOES NOT CHECK FOR BOUNDS, but underlying getter does, only in debug builds.
		 * \param index Index to the element to be retrieved.
		 * \return iterator to the element At index.
		 */
		iterator getIterator(const length_type index) noexcept { return &this->data[index]; }

		friend class Vector;
		
		friend void Insert(const Vector&, class Buffer&);
		friend void Extract(Vector&, class Buffer&);
	};
}
