#pragma once

#include "Buffer.hpp"
#include "Extent.h"
#include "Algorithm.hpp"
#include "Id.h"
#include "Pair.hpp"
#include "Math/Vectors.hpp"

namespace GTSL
{
	//void Insert(std::integral auto n, auto& buffer) { buffer.Write(sizeof n, reinterpret_cast<const byte*>(&n)); }
	//
	//void Extract(std::integral auto& n, auto& buffer) { buffer.Read(sizeof n, reinterpret_cast<byte*>(&n)); }

	auto& operator<<(auto& buffer, std::integral auto a) {
		buffer.Write(sizeof a, reinterpret_cast<byte*>(&a));
		return buffer;
	}

	auto& operator>>(auto& buffer, std::integral auto& a) {
		buffer.Read(sizeof a, reinterpret_cast<byte*>(&a));
		return buffer;
	}

	template<typename A, typename B>
	void Insert(const Pair<A, B>& pair, auto& buffer) {
		Insert(pair.First, buffer); Insert(pair.Second, buffer);
	}

	template<typename A, typename B>
	void Extract(Pair<A, B>& pair, auto& buffer) {
		Extract(pair.First, buffer); Extract(pair.Second, buffer);
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

	void Insert(const POD auto pod, auto& buffer) {
		buffer.Write(sizeof(decltype(pod)), reinterpret_cast<const byte*>(&pod));
	}

	void Extract(POD auto& pod, auto& buffer) {
		buffer.Read(sizeof(decltype(pod)), reinterpret_cast<byte*>(&pod));
	}

	void Insert(const Vector2 vector2, auto& buffer) {
		buffer.Write(sizeof(float32) * 2, reinterpret_cast<const byte*>(&vector2));
	}

	void Extract(Vector2& vector2, auto& buffer) {
		buffer.Read(sizeof(float32) * 2, reinterpret_cast<byte*>(&vector2));
	}
	
	void Insert(const Vector3 vector3, auto& buffer) {
		buffer.Write(sizeof(float32) * 3, reinterpret_cast<const byte*>(&vector3));
	}

	void Extract(Vector3& vector3, auto& buffer) {
		buffer.Read(sizeof(float32) * 3, reinterpret_cast<byte*>(&vector3));
	}

	void Insert(const Vector4 vector4, auto& buffer) {
		buffer.Write(sizeof(float32) * 4, reinterpret_cast<const byte*>(&vector4));
	}

	void Extract(Vector4& vector4, auto& buffer) {
		buffer.Read(sizeof(float32) * 4, reinterpret_cast<byte*>(&vector4));
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

	void Insert(const Extent2D extent, auto& buffer) {
		buffer.Write(sizeof(Extent2D), reinterpret_cast<const byte*>(&extent));
	}
	
	void Extract(Extent2D& extent, auto& buffer) {
		buffer.Read(sizeof(Extent2D), reinterpret_cast<byte*>(&extent));
	}
	
	void Insert(const Extent3D extent, auto& buffer) {
		buffer.Write(sizeof(Extent3D), reinterpret_cast<const byte*>(&extent));
	}
	
	void Extract(Extent3D& extent, auto& buffer) {
		buffer.Read(sizeof(Extent3D), reinterpret_cast<byte*>(&extent));
	}
	
	void Insert(const Id64 id, auto& buffer) {
		buffer.Write(sizeof(Id64), reinterpret_cast<const byte*>(&id));
	}
	
	void Extract(Id64& id, auto& buffer) {
		buffer.Read(sizeof(Id64), reinterpret_cast<byte*>(&id));
	}
}
