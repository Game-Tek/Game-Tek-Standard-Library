#pragma once

#include "Stream.h"
#include "Buffer.h"
#include "FlatHashMap.h"

namespace GTSL
{
	template<typename T>
	void Insert(const Vector<T>& vector, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		Insert(vector.GetLength(), buffer, allocatorReference);
		for (auto& e : vector) { Insert(e, buffer, allocatorReference); }
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
		ForEach(map, [&](const T& e) { Insert(e, buffer, allocatorReference); });
	}

	template<typename T>
	void Extract(FlatHashMap<T>& map, Buffer& buffer, const AllocatorReference& containerAllocator)
	{
		uint32 capacity{ 0 }; uint64 key;
		Extract(capacity, buffer, containerAllocator);
		map.Emplace(containerAllocator, key);
		for(uint32 i = 0; i < capacity; ++i) {}
		//ForEach(map, [&](const T& e) { Insert(e, buffer, allocatorReference); });
	}
}
