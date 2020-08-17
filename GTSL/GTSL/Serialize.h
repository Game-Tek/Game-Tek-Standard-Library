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
		Insert(map.loadFactor, buffer);

		for(uint32 keys_bucket = 0; keys_bucket < map.capacity; ++keys_bucket)
		{
			auto max_bucket_length = map.getMaxBucketLength();
			Insert(map.getBucketLength(keys_bucket, max_bucket_length), buffer);
			auto keys_bucket = map.getKeysBucket(keys_bucket, max_bucket_length);
			for(auto e : keys_bucket) { Insert(e, buffer); }
			auto values_bucket = map.getValuesBucket(keys_bucket, max_bucket_length);
			for(auto& e : values_bucket) { Insert(e, buffer); }
		}
	}

	template<typename T, class ALLOCATOR>
	void Extract(FlatHashMap<T, ALLOCATOR>& map, Buffer& buffer)
	{
		uint32 capacity{ 0 }; float32 load_factor = 0;
		Extract(capacity, buffer);
		Extract(load_factor, buffer);
		
		for (uint32 keys_bucket = 0; keys_bucket < capacity; ++keys_bucket)
		{
			auto max_bucket_length = map.getMaxBucketLength();
			Extract(map.getBucketLength(keys_bucket, max_bucket_length), buffer);
			auto keys_bucket = map.getKeysBucket(keys_bucket, max_bucket_length);
			for (auto& e : keys_bucket) { Extract(e, buffer); }
			auto values_bucket = map.getValuesBucket(keys_bucket, max_bucket_length);
			for (auto& e : values_bucket) { Extract(e, buffer); }
		}
	}
}
