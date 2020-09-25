#pragma once

#include "Core.h"
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

		Buffer(Buffer&& buffer) noexcept : data(buffer.data), capacity(buffer.capacity), length(buffer.length), readPos(buffer.readPos)
		{
			buffer.data = nullptr;
		}
		
		template<class ALLOCATOR>
		void Allocate(const uint64 bytes, const uint32 alignment, const ALLOCATOR& allocatorReference)
		{
			uint64 allocated_size{ 0 };
			allocatorReference.Allocate(bytes, alignment, reinterpret_cast<void**>(&data), &allocated_size);
			capacity = allocated_size;
		}

		void Resize(const uint64 size) { length = size; }
		void SetReadPosition(const uint64 pos) { readPos = pos; }

		template<class ALLOCATOR>
		void Free(const uint32 alignment, const ALLOCATOR& allocatorReference)
		{
			allocatorReference.Deallocate(capacity, alignment, data); capacity = 0; data = nullptr;
		}

		void WriteBytes(const uint64 size, const byte* from) 
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
		
	private:
		byte* data{ nullptr };
		uint64 capacity{ 0 };
		uint64 length{ 0 };
		uint64 readPos = 0;

		friend void Insert(bool n, Buffer& buffer);
		friend void Insert(uint8 n, Buffer& buffer);
		friend void Insert(uint16 n, Buffer& buffer);
		friend void Insert(uint32 n, Buffer& buffer);
		friend void Insert(uint64 n, Buffer& buffer);
		friend void Insert(float32 n, Buffer& buffer);
		
		friend void Extract(bool& n, Buffer& buffer);
		friend void Extract(uint8& n, Buffer& buffer);
		friend void Extract(uint16& n, Buffer& buffer);
		friend void Extract(uint32& n, Buffer& buffer);
		friend void Extract(uint64& n, Buffer& buffer);
		friend void Extract(float32& n, Buffer& buffer);
	};

	inline void Insert(bool n, Buffer& buffer) { buffer.WriteBytes(sizeof(bool), reinterpret_cast<const byte*>(&n)); }
	inline void Insert(uint8 n, Buffer& buffer) { buffer.WriteBytes(sizeof(uint8), reinterpret_cast<byte*>(&n)); }
	inline void Insert(uint16 n, Buffer& buffer) { buffer.WriteBytes(sizeof(uint16), reinterpret_cast<byte*>(&n)); }
	inline void Insert(uint32 n, Buffer& buffer) { buffer.WriteBytes(sizeof(uint32), reinterpret_cast<const byte*>(&n)); }
	inline void Insert(uint64 n, Buffer& buffer) { buffer.WriteBytes(sizeof(uint64), reinterpret_cast<const byte*>(&n)); }
	inline void Insert(float32 n, Buffer& buffer) { buffer.WriteBytes(sizeof(float32), reinterpret_cast<const byte*>(&n)); }

	inline void Extract(bool& n, Buffer& buffer) { buffer.ReadBytes(sizeof(bool), reinterpret_cast<byte*>(&n)); }
	inline void Extract(uint8& n, Buffer& buffer) { buffer.ReadBytes(sizeof(uint8), reinterpret_cast<byte*>(&n)); }
	inline void Extract(uint16& n, Buffer& buffer) { buffer.ReadBytes(sizeof(uint16), reinterpret_cast<byte*>(&n)); }
	inline void Extract(uint32& n, Buffer& buffer) { buffer.ReadBytes(sizeof(uint32), reinterpret_cast<byte*>(&n)); }
	inline void Extract(uint64& n, Buffer& buffer) { buffer.ReadBytes(sizeof(uint64), reinterpret_cast<byte*>(&n)); }
	inline void Extract(float32& n, Buffer& buffer) { buffer.ReadBytes(sizeof(float32), reinterpret_cast<byte*>(&n)); }
}
