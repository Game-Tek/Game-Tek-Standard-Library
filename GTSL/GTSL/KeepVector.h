#pragma once

#include "Vector.hpp"

namespace GTSL
{
	/**
	 * \brief A vector that maintains indices for placed objects during their lifetime.
	 * \tparam T Type of the object this KeepVector will store.
	 */
	template<typename T>
	class KeepVector
	{
	public:
		using length_type = typename Vector<T>::length_type;

	private:
		Vector<T> objects;
		Vector<uint32> freeIndeces;

		/**
		 * \brief Tries to Find a free index.
		 * \param index reference to a length_type variable to store the free index.
		 * \return A bool indicating whether or not a free index was found.
		 */
		bool findFreeIndex(length_type& index)
		{
			if (freeIndeces.GetLength() == 0) //If there aren't any free indeces return false,
			{
				return false;
			}	//if there are, grab the bottom one and Pop it from the list so it is no longer available.
			const auto free_index = freeIndeces[0];
			freeIndeces.Pop(0);
			index = free_index;
			return true;
		}
	public:
		auto begin() noexcept { return objects.begin(); }
		auto end() noexcept { return objects.end(); }

		explicit KeepVector(const length_type min) : objects(min), freeIndeces(min, min)
		{
			//Allocate objects space for min objects
			//Allocate min indeces and set it's length as min so they are marked as used

			//Fill every element in freeIndeces with it's corresponding index so they are available for using(marked as free)(because every index/element in objects is now free).

			length_type i = 0;
			for (auto& e : freeIndeces) { e = i; }
		}

		/**
		 * \brief Inserts an object into the vector At the first free slot available.
		 * \param obj Object reference to insert.
		 * \return Index At which the object was inserted.
		 */
		length_type Insert(const T& obj)
		{
			length_type index = 0;

			if (findFreeIndex(index)) //If there is a free index insert there,
			{
				objects.Place(index, obj);
				return index;
			}

			//if there wasn't a free index Place a the back of the array.
			return objects.PushBack(obj);
		}

		/**
		 * \brief Emplaces an object into the vector At the first free slot available.
		 * \param args Arguments for construction of object of type T.
		 * \return Index At which it was emplaced.
		 */
		template <typename... ARGS>
		length_type Emplace(ARGS&&... args)
		{
			length_type index = 0;

			if (findFreeIndex(index)) //If there is a free index insert there,
			{
				objects.Emplace(index, GTSL::MakeForwardReference<ARGS>(args) ...);
				return index;
			}

			//if there wasn't a free index Place a the back of the array.
			return objects.EmplaceBack(GTSL::MakeForwardReference<ARGS>(args) ...);
		}

		/**
		 * \brief Destroys the object At the specified index which makes space for another object to take it's Place.
		 * \param index Index of the object to remove.
		 */
		void Destroy(const length_type index)
		{
			freeIndeces.PushBack(index); //index is now free, make it available

			if (index == objects.GetLength()) //If the object is the last in the array Pop it,
			{
				objects.PopBack();
			}

			//if it isn't (it's somewhere in the middle) Destroy the object At that index.
			objects.Destroy(index);
		}
	};
}
