#pragma once

#include "Buffer.h"
#include "FlatHashMap.h"
#include "Array.hpp"
#include "Vector.hpp"

namespace GTSL
{
	template<typename T, uint32 N>
	void Insert(const Array<T, N>& array, Buffer& buffer)
	{
		Insert(array.GetLength(), buffer);
		for (const auto& e : array) { Insert(e, buffer); }
	}

	template<typename T, uint32 N>
	void Extract(Array<T, N>& array, Buffer& buffer)
	{
		uint32 length{ 0 };
		Extract(length, buffer); array.Resize(length);
		for (auto& e : array) { Extract(e, buffer); }
	}

	template<typename T, class ALLOCATOR>
	void Insert(const Vector<T, ALLOCATOR>& vector, Buffer& buffer)
	{
		Insert(vector.GetLength(), buffer);
		for (const auto& e : vector) { Insert(e, buffer); }
	}
	
	template<typename T, class ALLOCATOR>
	void Extract(Vector<T, ALLOCATOR>& vector, Buffer& buffer)
	{
		uint32 length{ 0 };
		Extract(length, buffer); vector.Initialize(length);
		vector.Resize(length);
		for (auto& e : vector) { Extract(e, buffer); }
	}
	
	template<typename T, class ALLOCATOR>
	void Insert(const FlatHashMap<T, ALLOCATOR>& map, Buffer& buffer)
	{
		Insert(map.capacity, buffer);
		Insert(map.maxBucketLength, buffer);

		for(uint32 bucket = 0; bucket < map.capacity; ++bucket)
		{
			Insert(map.getBucketLength(bucket), buffer);
			auto keysBucket = map.getKeysBucket(bucket);
			for(auto e : keysBucket) { Insert(e, buffer); }
			auto valuesBucket = map.getValuesBucket(bucket);
			for(auto& e : valuesBucket) { Insert(e, buffer); }
		}
	}

	template<typename T, class ALLOCATOR>
	void Extract(FlatHashMap<T, ALLOCATOR>& map, Buffer& buffer)
	{
		uint32 capacity{ 0 }, maxBucketLength = 0;
		Extract(capacity, buffer);
		Extract(maxBucketLength, buffer);
		
		for (uint32 bucket = 0; bucket < capacity; ++bucket)
		{
			Extract(map.getBucketLength(bucket), buffer);
			auto keysBucket = map.getKeysBucket(bucket);
			for (auto& e : keysBucket) { Extract(e, buffer); }
			auto valuesBucket = map.getValuesBucket(bucket);
			for (auto& e : valuesBucket) { Extract(e, buffer); }
		}
	}
}
