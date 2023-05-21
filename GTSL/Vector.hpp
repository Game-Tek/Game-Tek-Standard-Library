#pragma once

#include "Core.h"

#include "Assert.h"
#include "Algorithm.hpp"
#include <initializer_list>
#include <new>
#include "Range.hpp"
#include "Result.h"
#include "Allocator.hpp"
#include "ArrayCommon.hpp"
#include "Tuple.hpp"
#include "TypeTraits.hpp"

namespace GTSL
{	
	template <typename T, class ALLOCATOR>
	class Vector {
	public:
		typedef uint32 length_type;
		typedef ALLOCATOR allocator_t;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef T value_type;

		Vector(const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {}
		
		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements.
		 * \param capacity Number of T objects to allocate space for.
		 */
		Vector(const length_type minElements, const ALLOCATOR& allocatorReference = ALLOCATOR()) : allocator(allocatorReference),
		capacity(minElements)
		{
			Allocate(allocator, minElements, &data, &capacity);
		}

		/**
		 * \brief Constructs a Vector with enough space to accomodate capacity T elements, and considers length elements already occupied.
		 */
		Vector(const Range<const T*> ranger, const ALLOCATOR& allocatorReference = ALLOCATOR()) requires std::copy_constructible<T> : allocator(allocatorReference),
		length(static_cast<uint32>(ranger.ElementCount()))
		{
			Allocate(allocator, ranger.ElementCount(), &data, &capacity);
			buildCopyArray(ranger.begin(), data, length);
		}

		/**
		 * \brief Constructs a Vector and copies it's elements from initializerList.
		 * \param initializerList Initializer list to take elements from.
		 */
		constexpr Vector(const std::initializer_list<T>& initializerList) requires std::copy_constructible<T> : length(static_cast<uint32>(initializerList.size())),
		capacity(static_cast<uint32>(initializerList.size()))
		{
			Allocate(allocator, initializerList.size(), &data, &capacity);
			buildCopyArray(initializerList.begin(), data, length);
		}

		/**
		 * \brief Constructs a Vector from a reference to another Vector.
		 * \param other Reference to another Vector.
		 */
		Vector(const Vector& other) requires std::copy_constructible<T> : allocator(other.allocator), length(other.length),
		capacity(other.capacity)
		{
			Allocate(allocator, other.length, &data, &capacity);
			buildCopyArray(other.begin(), data, length);
		}

		/**
		 * \brief Constructs a Vector from an r-value reference to another Vector.
		 * \param other R-Value reference to other Vector to transfer from.
		 */
		Vector(Vector&& other) noexcept requires std::move_constructible<ALLOCATOR> : allocator(GTSL::MoveRef(other.allocator)), length(other.length), capacity(other.capacity), data(other.data) {
			other.data = nullptr;
		}

		Vector& operator=(const Vector& other) requires std::copy_constructible<T> {
			GTSL_ASSERT(this != &other, "Assigning to self is not allowed!");
			TryFree();
			allocator = other.allocator;
			Allocate(allocator, other.length, &data, &capacity);
			buildCopyArray(other.data, data, other.length);
			length = other.length;
			return *this;
		}

		/**
		 * \brief Transfers the data of other Vector to this Vector.
		 * \param other Reference to other Vector.
		 * \return Reference to self.
		 */
		Vector& operator=(Vector&& other) noexcept requires std::move_constructible<ALLOCATOR> {
			GTSL_ASSERT(this != &other, "Assigning to self is not allowed!");
			TryFree();
			allocator = GTSL::MoveRef(other);
			capacity = other.capacity;
			length = other.length;
			data = other.data;
			other.data = nullptr;
			return *this;
		}

		void TryFree() {
			if (data) [[likely]] {
				for (auto& e : *this) { Destroy(e); }
				Deallocate(allocator, capacity, data);
				data = nullptr;
			}
		}
		
		~Vector() {
			TryFree();
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

		Range<T*> GetRange() noexcept { return Range<T*>(length, data); }
		Range<const T*> GetRange() const noexcept { return Range<const T*>(length, data); }
		
		/**
		 * \brief Resizes the array(if necessary, does not reallocate for downsize) to accomodate count T elements.
		 * \param count Number of new T elements to accomodate.
		 */
		void Resize(const length_type count)
		{
			if (count > length) {
				tryReallocate(count - length);
			} else {
				for (uint32 i = count; i < length; ++i) { GTSL::Destroy(data[i]); }
				length = count;
			}
		}

		/**
		 * \brief Downsizes the array to fit count elements.
		 * \param count Number of T type to leave space for.
		 */
		void Shrink(const length_type count) {
            for(auto* b = begin() + count; b != end(); ++b) { GTSL::Destroy(*b);}
			GTSL::Resize(allocator, &data, &capacity, count, length);
		}

		void PushBack(const Range<const T*> other) {
			tryReallocate(static_cast<int32>(other.ElementCount()));
			copyArray(other.begin(), data + length, static_cast<uint32>(other.ElementCount()));
			length += static_cast<uint32>(other.ElementCount());
		}

		template<typename... ARGS>
		void EmplaceGroup(const uint32 count, ARGS&&... args) {
			tryReallocate(count);

			for(uint32 i = length, c = 0; c < count; ++i, ++c) {
				::new(data + i) T(GTSL::ForwardRef<ARGS>(args)...);
			}

			length += count;
		}

		/**
		 * \brief Emplaces(constructs) a T type object At the end of the vector and forwards it's arguments.
		 * \tparam ARGS Template type/s of args.
		 * \param args Arguments to be forward to the T object being constructed At the end of the vector.
		 * \return Length of vector after inserting, also index At which the object was inserted.
		 */
		template <typename... ARGS>
		T& EmplaceBack(ARGS&&... args) {
			tryReallocate(1);
			return *::new(static_cast<void*>(data + length++)) T(GTSL::ForwardRef<ARGS>(args)...);
		}

		/**
		 * \brief Destroys the object At the end of the vector and frees one space.
		 */
		void PopBack() {
			GTSL_ASSERT(this->length != 0, "Cannot pop back, there are no more elements!")
			Destroy(data[length]);
			length -= 1;
		}

		/**
		 * \brief Copies the passed in element At index and shifts the rest of the vector forward to fit it in.
		 * \param index Index At which to Place the object.
		 * \return Returns the length of the vector after inserting.
		 */
		template<typename... ARGS>
		T& Insert(const length_type index, ARGS&&... args) {
			tryReallocate(1);
			InsertElement(capacity, length, data, index, GTSL::ForwardRef<ARGS>(args)...);
			return data[length++];
		}

		/**
		 * \brief Removes the elements At index and moves the rest of the vector to fill the space.
		 * \param index Index of the element to Pop.
		 */
		void Pop(const length_type index) {
			PopElement(capacity, length, data, index);
			length -= 1;
		}

		/**
		 * \brief Removes all elements from index to index + length and moves the rest of the vector to fill the space.
		 * \param index Index At which to start popping.
		 * \param elemCount Number of elements to Pop.
		 */
		void Pop(const length_type index, const length_type elemCount) {
			for (auto* begin = getIterator(index); begin != getIterator(index + elemCount); ++begin) { begin->~T(); }
			copyArray(getIterator(index + elemCount), getIterator(index), elemCount);
			this->length -= elemCount;
		}

		/**
		 * \brief Looks for an object inside the vector.
		 * \param obj Object to look for.
		 * \return Iterator to element if it was found, iterator to end if object was not found.
		 */
		Result<uint32> Find(const T& obj) const noexcept {
			for (uint32 i = 0; i < GetLength(); ++i) { if (obj == data[i]) { return Result<uint32>(MoveRef(i), true); } } return Result<uint32>(false);
		}

		template<typename F>
		Result<uint32> LookFor(F&& function) const {
			for (uint32 i = 0; i < GetLength(); ++i) { if (function(data[i])) { return Result<uint32>(MoveRef(i), true); } }
			return Result<uint32>(false);
		}
		
		/**
		 * \brief Returns a reference to the element at index.
		 * \param index Index to element.
		 * \return Reference to the element at index.
		 */
		T& operator[](const length_type index) noexcept {
			GTSL_ASSERT(index < this->length, "Entered index is not accessible, array is not as large.")
			return this->data[index];
		}

		/**
		 * \brief Returns a reference to the element at index.
		 * \param index Index to element.
		 * \return Const reference to the element at index.
		 */
		const T& operator[](const length_type index) const noexcept {
			GTSL_ASSERT(index < this->length, "Entered index is not accessible, array is not as large.")
			return this->data[index];
		}

		explicit operator bool() const { return length; }

		friend bool operator<(const uint32 i, const Vector& vec) {
			return i < vec.GetLength();
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

		[[nodiscard]] operator Range<T*>() noexcept { return Range<T*>(this->data, this->data + this->length); }
		[[nodiscard]] operator Range<T*>() const noexcept { return Range<T*>(this->data, this->data + this->length); }
		[[nodiscard]] operator Range<const T*>() const noexcept { return Range<const T*>(this->data, this->data + this->length); }

		void SetLength(const uint32 newLength) {
			length = newLength;
			Resize(newLength);
		}
	
	private:
#pragma warning(disable : 4648)
		[[no_unique_address]] ALLOCATOR allocator;
		length_type length{ 0 };
		length_type capacity{ 0 };
		T* data{ nullptr };
#pragma warning(default : 4648)

		/**
		 * \brief Copies data from from to to.
		 * \param from Pointer from where to grab the data.
		 * \param to Pointer to write the data to.
		 * \param elementCount How many elements of this vector's T to write to to.
		 */
		static void copyArray(const T* from, T* to, const length_type elementCount) requires std::copyable<T> {
			for (uint32 i = 0; i < elementCount; ++i) {
				to[i] = from[i];
			}
		}

		static void buildCopyArray(const T* from, T* to, const length_type elementCount) requires std::copy_constructible<T> {
			for (uint32 i = 0; i < elementCount; ++i) {
				::new(to + i) T(from[i]);
			}
		}

		/**
		 * \brief Reallocates this->data to a new array if (this->length + additionalElements) exceeds the allocated space.\n
		 * Also deletes the data found At the old data.
		 * Growth of the array is geometric.
		 * Number can be negative.
		 */
		void tryReallocate(const int32 deltaElements) {
			if (length + deltaElements > capacity) {
				if (data) {
					GTSL::Resize(allocator, &data, &capacity, capacity * 2, length);
				} else {
					Allocate(allocator, deltaElements, &data, &capacity);
				}
			}
		}

		/**
		 * \brief Returns an iterator to an specified index. DOES NOT CHECK FOR BOUNDS, but underlying getter does, only in debug builds.
		 * \param index Index to the element to be retrieved.
		 * \return iterator to the element At index.
		 */
		iterator getIterator(const length_type index) noexcept { return &this->data[index]; }

		//friend class Vector;

	public:
		template<typename TT, class AA>
		friend void Insert(const Vector<TT, AA>& vector, auto& buffer);
		
		template<typename TT, class AA>
		friend void Extract(Vector<TT, AA>& vector, auto& buffer);
	};

	template<typename T, class ALLOCATOR>
	void Insert(const Vector<T, ALLOCATOR>& vector, auto& buffer) {
		Insert(vector.GetLength(), buffer);
		for (const auto& e : vector) { Insert(e, buffer); }
	}

	template<typename T, class ALLOCATOR>
	void Extract(Vector<T, ALLOCATOR>& vector, auto& buffer) {
		uint32 length{ 0 };
		Extract(length, buffer);
		::new(&vector) Vector<T, ALLOCATOR>(length);
		vector.length = length;
		for (uint32 i = 0; i < length; ++i) {
			Extract(vector.data[i], buffer);
		}
	}
	
	template<typename T, uint64 N>
	using StaticVector = Vector<T, StaticAllocator<N * sizeof(T)>>;

	template<typename T, uint64 N, class B>
	using SemiVector = Vector<T, DoubleAllocator<N * sizeof(T), B>>;

	template<class ALLOCATOR, bool AOS, typename... TYPES>
	class MultiVector {
	public:
		MultiVector(const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {}

		MultiVector(const uint32 minElements, const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {
			tryResize(minElements);
		}

		~MultiVector() {
			if (capacity) {
				Deallocate(allocator, PackSize<TYPES...>() * capacity, data);
			}
		}

		void EmplaceBack(TYPES&&... args) {
			tryResize(1);

			if constexpr (AOS) {
				[&] <uint64... I>(Indices<I...>) -> void {
					constexpr auto si = PackSize<TYPES...>();
					uint64 pos = 0;
					((::new(data + si * length + (pos += sizeof(TYPES), pos - sizeof(TYPES))) TYPES(args)), ...);
				} (BuildIndices<sizeof...(TYPES)>{});
			} else {
				[&] <uint64... I>(Indices<I...>) -> void {
					constexpr auto si = PackSize<TYPES...>();
					uint64 pos = 0;
					((::new(data + capacity * (pos += sizeof(TYPES), pos - sizeof(TYPES)) + length * sizeof(TYPES)) TYPES(args)), ...);
				} (BuildIndices<sizeof...(TYPES)>{});
			}

			++length;
		}

		template<uint32 M>
		static auto GetPointer(byte* data, const uint32 capacity, const uint32 index) -> typename TypeAt<M, TYPES...>::type* {
			if constexpr (AOS) {
				return reinterpret_cast<typename TypeAt<M, TYPES...>::type*>(data + PackSize<TYPES...>() * index + PackSizeAt<M, TYPES...>());
			}
			else {
				return reinterpret_cast<typename TypeAt<M, TYPES...>::type*>(data + capacity * PackSizeAt<M, TYPES...>() + index * sizeof(typename TypeAt<M, TYPES...>::type));
			}
		}

		template<uint32 M>
		static auto GetPointer(const byte* data, const uint32 capacity, const uint32 index) -> const typename TypeAt<M, TYPES...>::type* {
			if constexpr (AOS) {
				return reinterpret_cast<const typename TypeAt<M, TYPES...>::type*>(data + PackSize<TYPES...>() * index + PackSizeAt<M, TYPES...>());
			}
			else {
				return reinterpret_cast<const typename TypeAt<M, TYPES...>::type*>(data + capacity * PackSizeAt<M, TYPES...>() + index * sizeof(typename TypeAt<M, TYPES...>::type));
			}
		}

		template<uint32 M>
		auto GetPointer(const uint32 index) -> typename TypeAt<M, TYPES...>::type* {
			return GetPointer<M>(data, capacity, index);
		}

		template<uint32 M>
		auto At(const uint32 index) -> typename TypeAt<M, TYPES...>::type& {
			return *GetPointer<M>(index);
		}

		byte* GetData() { return data; }

		operator MultiRange<TYPES...>() requires(!AOS) {
			return [&]<uint64... I>(Indices<I...>) { return MultiRange<TYPES...>(length, GetPointer<I>(0)...); }(BuildIndices<sizeof...(TYPES)>{});
		}

	private:
		ALLOCATOR allocator;
		uint32 capacity = 0, length = 0;
		byte* data = nullptr;

		void tryResize(const uint32 delta) {
			if(length + delta > capacity) {
				if(data) {
					uint32 ccapacity = capacity * PackSize<TYPES...>();

					if constexpr (!AOS) {
						auto copy = [&](uint64 currentCapacity, byte* from, uint64 newCapacity, byte* to) {
							uint32 currCap = currentCapacity / PackSize<TYPES...>(), endCap = newCapacity / PackSize<TYPES...>();
							[&]<uint64... I>(Indices<I...>) { (Copy(length, GetPointer<I>(from, currCap, 0), GetPointer<I>(to, endCap, 0)), ...); }(BuildIndices<sizeof...(TYPES)>{});
						};

						ResizeCustom(allocator, &data, &ccapacity, capacity * 2 * PackSize<TYPES...>(), copy, 32);
					} else {
						Resize(allocator, &data, &ccapacity, capacity * 2 * PackSize<TYPES...>(), length * PackSize<TYPES...>(), 32);
					}

					capacity = ccapacity;
				} else {
					Allocate(allocator, delta * PackSize<TYPES...>(), &data, &capacity, 32);
				}

				capacity /= PackSize<TYPES...>();
			}
		}
	};

	template<typename T, class ALLOC>
	void Skim(Vector<T, ALLOC>& vec, auto&& predicate) {
		StaticVector<uint32, 512> toSkim;

		for (uint32 i = 0; i < vec; ++i) {
			if (predicate(vec[i])) {
				toSkim.EmplaceBack(i);
			}
		}

		for(auto e : toSkim) { vec.Pop(e); }
	}
}
