#pragma once

#include "Core.h"
#include "Memory.h"

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{	
	class BufferInterface
	{
	public:
		void CopyBytes(const uint64 size, const byte* from)
		{
			GTSL_ASSERT(*length + size <= *capacity, "Buffer can't fit more!");
			MemCopy(size, from, *data + *length); *length += size;
		}

		byte* begin() { return *data; }
		byte* end() { return *data; }

		uint64 GetLength() const { return *length; }
		
		void Resize(uint64 newSize)
		{
			*length = newSize;
		}

		void AddResize(int64 delta)
		{
			*length += delta;
		}
	
	private:
		byte** data;
		uint64* length;
		uint64* capacity;

		template<class ALLOCATOR>
		friend class Buffer;
		
		BufferInterface(byte** d, uint64* len, uint64* cap) : data(d), length(len), capacity(cap) {}
	};
	
	template<class ALLOCATOR>
	class Buffer
	{
	public:
		Buffer() = default;
		
		~Buffer()
		{
			if (data)
			{
				allocator.Deallocate(capacity, alignment, data); data = nullptr;
			}
		}

		Buffer(Buffer&& buffer) noexcept : data(buffer.data), capacity(buffer.capacity), length(buffer.length), readPos(buffer.readPos)
		{
			buffer.data = nullptr;
		}
		
		template<class ALLOCATOR>
		void Allocate(const uint64 bytes, const uint32 alignment, const ALLOCATOR& allocatorReference)
		{
			uint64 allocated_size{ 0 };
			this->allocator = allocatorReference;
			this->allocator.Allocate(bytes, alignment, reinterpret_cast<void**>(&data), &allocated_size);
			capacity = allocated_size;
		}

		void Resize(const uint64 size) { length = size; }
		void AddResize(const int64 delta) { length += delta; }
		void SetReadPosition(const uint64 pos) { readPos = pos; }

		void CopyBytes(const uint64 size, const byte* from)
		{
			GTSL_ASSERT(length + size <= capacity, "Buffer can't fit more!");
			MemCopy(size, from, data + length); length += size;
		}

		void ReadBytes(const uint64 size, byte* to)
		{
			GTSL_ASSERT(readPos <= length, "Buffer doesn't contain any more bytes!");
			MemCopy(size, data + readPos, to); readPos += size;
		}
		
		[[nodiscard]] uint64 GetCapacity() const { return capacity; }
		[[nodiscard]] uint64 GetLength() const { return length; }
		[[nodiscard]] uint64 GetReadPosition() const { return readPos; }
		[[nodiscard]] byte* GetData() const { return data; }

		operator Range<byte*>() const { return Range<byte*>(length, data); }
		operator Range<const byte*>() const { return Range<const byte*>(length, data); }

		Range<byte*> GetRange() { return Range<byte*>(length, data); }
		Range<const byte*> GetRange() const { return Range<const byte*>(length, data); }
		
		BufferInterface GetBufferInterface() { return BufferInterface(&data, &length, &capacity); }
	
	private:
		byte* data{ nullptr };
		uint32 alignment = 0;
		uint64 capacity = 0;
		uint64 length = 0;
		uint64 readPos = 0;

		ALLOCATOR allocator;
	};
}
