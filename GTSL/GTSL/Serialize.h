#pragma once

#include "Buffer.hpp"
#include "FlatHashMap.h"
#include "Array.hpp"
#include "Extent.h"
#include "Vector.hpp"
#include "Algorithm.h"
#include "Id.h"
#include "Pair.h"
#include "ShortString.hpp"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace GTSL
{
	template<class ALLOCATOR>
	void Insert(bool n, Buffer<ALLOCATOR>& buffer) { buffer.CopyBytes(sizeof(bool), reinterpret_cast<const byte*>(&n)); }
	template<class ALLOCATOR>
	void Insert(uint8 n, Buffer<ALLOCATOR>& buffer) { buffer.CopyBytes(sizeof(uint8), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Insert(uint16 n, Buffer<ALLOCATOR>& buffer) { buffer.CopyBytes(sizeof(uint16), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Insert(uint32 n, Buffer<ALLOCATOR>& buffer) { buffer.CopyBytes(sizeof(uint32), reinterpret_cast<const byte*>(&n)); }
	template<class ALLOCATOR>
	void Insert(uint64 n, Buffer<ALLOCATOR>& buffer) { buffer.CopyBytes(sizeof(uint64), reinterpret_cast<const byte*>(&n)); }
	template<class ALLOCATOR>
	void Insert(float32 n, Buffer<ALLOCATOR>& buffer) { buffer.CopyBytes(sizeof(float32), reinterpret_cast<const byte*>(&n)); }

	template<class ALLOCATOR>
	void Extract(bool& n, Buffer<ALLOCATOR>& buffer) { buffer.ReadBytes(sizeof(bool), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Extract(uint8& n, Buffer<ALLOCATOR>& buffer) { buffer.ReadBytes(sizeof(uint8), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Extract(uint16& n, Buffer<ALLOCATOR>& buffer) { buffer.ReadBytes(sizeof(uint16), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Extract(uint32& n, Buffer<ALLOCATOR>& buffer) { buffer.ReadBytes(sizeof(uint32), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Extract(uint64& n, Buffer<ALLOCATOR>& buffer) { buffer.ReadBytes(sizeof(uint64), reinterpret_cast<byte*>(&n)); }
	template<class ALLOCATOR>
	void Extract(float32& n, Buffer<ALLOCATOR>& buffer) { buffer.ReadBytes(sizeof(float32), reinterpret_cast<byte*>(&n)); }
	
	template<typename A, typename B, class ALLOCATOR>
	void Insert(const Pair<A, B>& pair, Buffer<ALLOCATOR>& buffer)
	{
		Insert(pair.First, buffer); Insert(pair.Second, buffer);
	}

	template<typename A, typename B, class ALLOCATOR>
	void Extract(Pair<A, B>& pair, Buffer<ALLOCATOR>& buffer)
	{
		Extract(pair.First, buffer); Extract(pair.Second, buffer);
	}

	template<typename T, uint32 N, class ALLOCATOR>
	void Insert(const Array<T, N>& array, Buffer<ALLOCATOR>& buffer)
	{
		Insert(array.GetLength(), buffer);
		for (const auto& e : array) { Insert(e, buffer); }
	}

	template<typename T, uint32 N, class ALLOCATOR>
	void Extract(Array<T, N>& array, Buffer<ALLOCATOR>& buffer)
	{
		uint32 length{ 0 };
		Extract(length, buffer); array.Resize(length);
		for (auto& e : array) { Extract(e, buffer); }
	}

	template<typename T, class ALLOCATOR1, class ALLOCATOR2>
	void Insert(const Vector<T, ALLOCATOR1>& vector, Buffer<ALLOCATOR2>& buffer)
	{
		Insert(vector.GetLength(), buffer);
		for (const auto& e : vector) { Insert(e, buffer); }
	}
	
	template<typename T, class ALLOCATOR1, class ALLOCATOR2>
	void Extract(Vector<T, ALLOCATOR1>& vector, Buffer<ALLOCATOR2>& buffer)
	{
		uint32 length{ 0 };
		Extract(length, buffer); vector.Initialize(length);
		vector.Resize(length);
		for (auto& e : vector) { Extract(e, buffer); }
	}
	
	template<typename T, class ALLOCATOR1, class ALLOCATOR2>
	void Insert(const FlatHashMap<T, ALLOCATOR1>& map, Buffer<ALLOCATOR2>& buffer)
	{
		Insert(map.bucketCount, buffer);
		Insert(map.maxBucketLength, buffer);

		for(uint32 bucket = 0; bucket < map.bucketCount; ++bucket)
		{
			Insert(map.getBucketLength(bucket), buffer);
			auto keysBucket = map.getKeysBucket(bucket);
			for(auto e : keysBucket) { Insert(e, buffer); }
			auto valuesBucket = map.getValuesBucket(bucket);
			for(auto& e : valuesBucket) { Insert(e, buffer); }
		}
	}

	template<typename T, class ALLOCATOR1, class ALLOCATOR2>
	void Extract(FlatHashMap<T, ALLOCATOR1>& map, Buffer<ALLOCATOR2>& buffer)
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

	template<Enum T, class ALLOCATOR>
	void Insert(const T enu, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(T), reinterpret_cast<const byte*>(&enu));
	}

	template<Enum T, class ALLOCATOR>
	void Extract(T& enu, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(T), reinterpret_cast<byte*>(&enu));
	}

	template<POD T, class ALLOCATOR>
	void Insert(const T pod, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(T), reinterpret_cast<const byte*>(&pod));
	}

	template<POD T, class ALLOCATOR>
	void Extract(T& pod, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(T), reinterpret_cast<byte*>(&pod));
	}

	template<class ALLOCATOR>
	void Insert(const Vector2 vector2, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(float32) * 2, reinterpret_cast<const byte*>(&vector2));
	}

	template<class ALLOCATOR>
	void Extract(Vector2& vector2, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(float32) * 2, reinterpret_cast<byte*>(&vector2));
	}
	
	template<class ALLOCATOR>
	void Insert(const Vector3 vector3, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(float32) * 3, reinterpret_cast<const byte*>(&vector3));
	}

	template<class ALLOCATOR>
	void Extract(Vector3& vector3, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(float32) * 3, reinterpret_cast<byte*>(&vector3));
	}

	template<class ALLOCATOR>
	void Insert(const Vector4 vector4, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(float32) * 4, reinterpret_cast<const byte*>(&vector4));
	}

	template<class ALLOCATOR>
	void Extract(Vector4& vector4, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(float32) * 4, reinterpret_cast<byte*>(&vector4));
	}
	
	//template<Union U, class ALLOCATOR>
	//void Insert(const U& uni, Buffer<ALLOCATOR>& buffer) //if trivially copyable
	//{
	//	buffer.CopyBytes(sizeof(U), reinterpret_cast<const byte*>(&uni));
	//}
	//
	//template<Union U, class ALLOCATOR>
	//void Extract(U& uni, Buffer<ALLOCATOR>& buffer)
	//{
	//	buffer.ReadBytes(sizeof(U), reinterpret_cast<byte*>(&uni));
	//}

	template<class ALLOCATOR>
	inline void Insert(const Extent2D extent, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(Extent2D), reinterpret_cast<const byte*>(&extent));
	}
	
	template<class ALLOCATOR>
	inline void Extract(Extent2D& extent, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(Extent2D), reinterpret_cast<byte*>(&extent));
	}
	template<class ALLOCATOR>
	inline void Insert(const Extent3D extent, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(Extent3D), reinterpret_cast<const byte*>(&extent));
	}
	template<class ALLOCATOR>
	inline void Extract(Extent3D& extent, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(Extent3D), reinterpret_cast<byte*>(&extent));
	}
	template<class ALLOCATOR>
	inline void Insert(const Id64 id, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(sizeof(Id64), reinterpret_cast<const byte*>(&id));
	}
	template<class ALLOCATOR>
	inline void Extract(Id64& id, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(sizeof(Id64), reinterpret_cast<byte*>(&id));
	}

	template<uint8 SIZE, class ALLOCATOR>
	inline void Insert(const ShortString<SIZE>& string, Buffer<ALLOCATOR>& buffer)
	{
		buffer.CopyBytes(SIZE, reinterpret_cast<const byte*>((Range<const UTF8*>(string).begin())));
	}

	template<uint8 SIZE, class ALLOCATOR>
	inline void Extract(ShortString<SIZE>& string, Buffer<ALLOCATOR>& buffer)
	{
		buffer.ReadBytes(SIZE, reinterpret_cast<byte*>((Range<UTF8*>(string).begin())));
	}
}
