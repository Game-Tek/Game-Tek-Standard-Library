#pragma once

#include "Core.h"

#include "Allocator.h"
#include "Memory.h"

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{
	class Buffer
	{
	public:
		Buffer() = default;
#if (!_DEBUG)
		~Buffer() = default;
#else
		~Buffer() { GTSL_ASSERT(!data, "Allocation was not freed!"); }
#endif
		
		void Allocate(const uint32 bytes, const uint32 alignment, const AllocatorReference& allocatorReference)
		{
			uint64 allocated_size{ 0 };
			allocatorReference.Allocate(bytes, alignment, reinterpret_cast<void**>(&data), &allocated_size);
			capacity = allocated_size;
		}

		void Resize(const uint32 size) { length = size; pos = size; }
		
		void Free(const uint32 alignment, const AllocatorReference& allocatorReference)
		{
			allocatorReference.Deallocate(capacity, alignment, data); capacity = 0; data = nullptr;
		}

		void WriteBytes(const uint32 size, const byte* from) 
		{
			GTSL_ASSERT(pos + size <= capacity, "Can't fit more!");
			MemCopy(size, from, data + pos);
			pos += size;
		}

		void ReadBytes(const uint32 size, byte* to)
		{
			GTSL_ASSERT(pos != 0, "Buffer is already empty!");
			MemCopy(size, data + (length - pos), to);
			pos -= size;
		}

		[[nodiscard]] uint32 GetRemainingSize() const { return length - pos; }
		
		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		[[nodiscard]] uint32 GetLength() const { return length; }
		[[nodiscard]] uint32 GetPosition() const { return pos; }
		[[nodiscard]] byte* GetData() const { return data; }

		operator Ranger<byte>() const { return Ranger<byte>(length, data); }
		operator Ranger<const byte>() const { return Ranger<const byte>(length, data); }
		
	private:
		byte* data{ nullptr };
		uint32 capacity{ 0 };
		uint32 length{ 0 };
		uint32 pos = 0;

		friend void Insert(bool n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Insert(uint32 n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Insert(uint64 n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Insert(float32 n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(bool& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(uint16& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(uint32& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(uint64& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(float32& n, Buffer& buffer, const AllocatorReference& allocatorReference);
	};
	
	inline void Extract(bool& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.ReadBytes(sizeof(bool), reinterpret_cast<byte*>(&n));
	}

	inline void Extract(uint16& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.ReadBytes(sizeof(uint16), reinterpret_cast<byte*>(&n));
	}

	inline void Extract(uint32& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.ReadBytes(sizeof(uint32), reinterpret_cast<byte*>(&n));
	}

	inline void Extract(uint64& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.ReadBytes(sizeof(uint64), reinterpret_cast<byte*>(&n));
	}

	inline void Extract(float32& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.ReadBytes(sizeof(float32), reinterpret_cast<byte*>(&n));
	}

	inline void Insert(const bool n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.WriteBytes(sizeof(bool), reinterpret_cast<const byte*>(&n));
	}

	inline void Insert(const uint32 n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.WriteBytes(sizeof(uint32), reinterpret_cast<const byte*>(&n));
	}

	inline void Insert(const uint64 n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.WriteBytes(sizeof(uint64), reinterpret_cast<const byte*>(&n));
	}

	inline void Insert(const float32 n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		buffer.WriteBytes(sizeof(float32), reinterpret_cast<const byte*>(&n));
	}
}
