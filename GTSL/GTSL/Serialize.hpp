#pragma once

#include "Buffer.hpp"
#include "Extent.h"
#include "Algorithm.h"
#include "Id.h"
#include "Pair.h"
#include "ShortString.hpp"
#include "String.hpp"
#include "Math/Vectors.h"

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
	
	template<typename A, typename B>
	void Insert(const Pair<A, B>& pair, auto& buffer)
	{
		Insert(pair.First, buffer); Insert(pair.Second, buffer);
	}

	template<typename A, typename B>
	void Extract(Pair<A, B>& pair, auto& buffer)
	{
		Extract(pair.First, buffer); Extract(pair.Second, buffer);
	}

	template<class ALLOCATOR>
	void Insert(const String<ALLOCATOR>& string, auto& buffer) {
		Insert(string.GetLength(), buffer);
		buffer.CopyBytes(string.GetLength(), reinterpret_cast<const byte*>(string.begin()));
	}

	//template<Enum T>
	//void Insert(const T enu, auto& buffer)
	//{
	//	buffer.CopyBytes(sizeof(T), reinterpret_cast<const byte*>(&enu));
	//}
	//
	//template<Enum T>
	//void Extract(T& enu, auto& buffer)
	//{
	//	buffer.ReadBytes(sizeof(T), reinterpret_cast<byte*>(&enu));
	//}

	void Insert(const POD auto pod, auto& buffer)
	{
		buffer.CopyBytes(sizeof(decltype(pod)), reinterpret_cast<const byte*>(&pod));
	}

	void Extract(POD auto& pod, auto& buffer)
	{
		buffer.ReadBytes(sizeof(decltype(pod)), reinterpret_cast<byte*>(&pod));
	}

	void Insert(const Vector2 vector2, auto& buffer)
	{
		buffer.CopyBytes(sizeof(float32) * 2, reinterpret_cast<const byte*>(&vector2));
	}

	void Extract(Vector2& vector2, auto& buffer)
	{
		buffer.ReadBytes(sizeof(float32) * 2, reinterpret_cast<byte*>(&vector2));
	}
	
	void Insert(const Vector3 vector3, auto& buffer)
	{
		buffer.CopyBytes(sizeof(float32) * 3, reinterpret_cast<const byte*>(&vector3));
	}

	void Extract(Vector3& vector3, auto& buffer)
	{
		buffer.ReadBytes(sizeof(float32) * 3, reinterpret_cast<byte*>(&vector3));
	}

	void Insert(const Vector4 vector4, auto& buffer)
	{
		buffer.CopyBytes(sizeof(float32) * 4, reinterpret_cast<const byte*>(&vector4));
	}

	void Extract(Vector4& vector4, auto& buffer)
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

	void Insert(const Extent2D extent, auto& buffer)
	{
		buffer.CopyBytes(sizeof(Extent2D), reinterpret_cast<const byte*>(&extent));
	}
	
	void Extract(Extent2D& extent, auto& buffer)
	{
		buffer.ReadBytes(sizeof(Extent2D), reinterpret_cast<byte*>(&extent));
	}
	
	void Insert(const Extent3D extent, auto& buffer)
	{
		buffer.CopyBytes(sizeof(Extent3D), reinterpret_cast<const byte*>(&extent));
	}
	
	void Extract(Extent3D& extent, auto& buffer)
	{
		buffer.ReadBytes(sizeof(Extent3D), reinterpret_cast<byte*>(&extent));
	}
	
	void Insert(const Id64 id, auto& buffer)
	{
		buffer.CopyBytes(sizeof(Id64), reinterpret_cast<const byte*>(&id));
	}
	
	void Extract(Id64& id, auto& buffer)
	{
		buffer.ReadBytes(sizeof(Id64), reinterpret_cast<byte*>(&id));
	}

	template<uint8 SIZE>
	void Insert(const ShortString<SIZE>& string, auto& buffer)
	{
		buffer.CopyBytes(SIZE, reinterpret_cast<const byte*>((Range<const char8_t*>(string).begin())));
	}

	template<uint8 SIZE>
	void Extract(ShortString<SIZE>& string, auto& buffer)
	{
		buffer.ReadBytes(SIZE, reinterpret_cast<byte*>((Range<char8_t*>(string).begin())));
	}
}
