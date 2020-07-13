#pragma once

#include "Core.h"

#include "Allocator.h"

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

		void Resize(const uint32 size) { length = size; }
		
		void Free(const uint32 alignment, const AllocatorReference& allocatorReference)
		{
			allocatorReference.Deallocate(capacity, alignment, data); capacity = 0; data = nullptr;
		}
		
		[[nodiscard]] uint32 GetCapacity() const { return capacity; }
		[[nodiscard]] uint32 GetLength() const { return length; }
		[[nodiscard]] byte* GetData() const { return data; }

		operator Ranger<byte>() const { return Ranger<byte>(length, data); }
		operator Ranger<const byte>() const { return Ranger<const byte>(length, data); }
		
	private:
		byte* data{ nullptr };
		uint32 capacity{ 0 };
		uint32 length{ 0 };

		friend void Insert(bool n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Insert(uint32 n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Insert(uint64 n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Insert(float32 n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(bool& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(uint32& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(uint64& n, Buffer& buffer, const AllocatorReference& allocatorReference);
		friend void Extract(float32& n, Buffer& buffer, const AllocatorReference& allocatorReference);
	};
	
	inline void Extract(bool& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length != 0, "Buffer is already empty!");
		n = *reinterpret_cast<bool*>(buffer.data + buffer.length); buffer.length -= sizeof(bool);
	}
	
	inline void Extract(uint32& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length != 0, "Buffer is already empty!");
		n = *reinterpret_cast<uint32*>(buffer.data + buffer.length); buffer.length -= sizeof(uint32);
	}

	inline void Extract(uint64& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length != 0, "Buffer is already empty!");
		n = *reinterpret_cast<uint64*>(buffer.data + buffer.length); buffer.length -= sizeof(uint64);
	}

	inline void Extract(float32& n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length != 0, "Buffer is already empty!");
		n = *reinterpret_cast<float32*>(buffer.data + buffer.length); buffer.length -= sizeof(float32);
	}

	inline void Insert(const bool n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length + sizeof(bool) <= buffer.capacity, "Buffer has no more space!");
		buffer.length += sizeof(bool); *reinterpret_cast<bool*>(buffer.data + buffer.length) = n;
	}

	inline void Insert(const uint32 n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length + sizeof(uint32) <= buffer.capacity, "Buffer has no more space!");
		buffer.length += sizeof(uint32); *reinterpret_cast<uint32*>(buffer.data + buffer.length) = n;
	}

	inline void Insert(const uint64 n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length + sizeof(uint64) <= buffer.capacity, "Buffer has no more space!");
		buffer.length += sizeof(uint64); *reinterpret_cast<uint64*>(buffer.data + buffer.length) = n;
	}

	inline void Insert(const float32 n, Buffer& buffer, const AllocatorReference& allocatorReference)
	{
		GTSL_ASSERT(buffer.length + sizeof(float32) <= buffer.capacity, "Buffer has no more space!");
		buffer.length += sizeof(float32); *reinterpret_cast<float32*>(buffer.data + buffer.length) = n;
	}
}
