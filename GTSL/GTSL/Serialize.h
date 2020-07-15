#pragma once

#include "Buffer.h"
#include "FlatHashMap.h"
#include "Array.hpp"

namespace GTSL
{
	template<typename T, uint32 N>
	void Insert(const Array<T, N>& array, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		Insert(array.GetLength(), buffer, allocatorReference);
		for (const auto& e : array) { Insert(e, buffer, allocatorReference); }
	}

	template<typename T, uint32 N>
	void Extract(Array<T, N>& array, Buffer& buffer, const AllocatorReference& containerAllocator)
	{
		uint32 length{ 0 };
		Extract(length, buffer, containerAllocator); array.Resize(length, containerAllocator);
		for (auto& e : array) { Extract(e, buffer, containerAllocator); }
	}

	template<typename T>
	void Insert(const Vector<T>& vector, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		Insert(vector.GetLength(), buffer, allocatorReference);
		for (const auto& e : vector) { Insert(e, buffer, allocatorReference); }
	}
	
	template<typename T>
	void Extract(Vector<T>& vector, Buffer& buffer, const AllocatorReference& containerAllocator)
	{
		uint32 length{ 0 };
		Extract(length, buffer, containerAllocator); vector.Initialize(length, containerAllocator);
		vector.Resize(length, containerAllocator);
		for (auto& e : vector) { Extract(e, buffer, containerAllocator); }
	}
	
	template<typename T>
	void Insert(const FlatHashMap<T>& map, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		Insert(map.capacity, buffer, allocatorReference);
		Insert(map.loadFactor, buffer, allocatorReference);

		for(uint32 bucket = 0; bucket < map.capacity; ++bucket)
		{
			auto max_bucket_length = map.getMaxBucketLength();
			Insert(map.getBucketLength(bucket, max_bucket_length), buffer, allocatorReference);
			auto keys_bucket = map.getKeysBucket(bucket, max_bucket_length);
			for(auto e : keys_bucket) { Insert(e, buffer, allocatorReference); }
			auto values_bucket = map.getValuesBucket(bucket, max_bucket_length);
			for(auto& e : values_bucket) { Insert(e, buffer, allocatorReference); }
		}
	}

	template<typename T>
	void Extract(FlatHashMap<T>& map, Buffer& buffer, const AllocatorReference& containerAllocator)
	{
		uint32 capacity{ 0 }; float32 load_factor = 0;
		Extract(capacity, buffer, containerAllocator);
		Extract(load_factor, buffer, containerAllocator);
		
		::new(&map) FlatHashMap<T>(capacity, load_factor, containerAllocator);
		
		for (uint32 bucket = 0; bucket < capacity; ++bucket)
		{
			auto max_bucket_length = map.getMaxBucketLength();
			Extract(map.getBucketLength(bucket, max_bucket_length), buffer, containerAllocator);
			auto keys_bucket = map.getKeysBucket(bucket, max_bucket_length);
			for (auto& e : keys_bucket) { Extract(e, buffer, containerAllocator); }
			auto values_bucket = map.getValuesBucket(bucket, max_bucket_length);
			for (auto& e : values_bucket) { Extract(e, buffer, containerAllocator); }
		}
	}
}
