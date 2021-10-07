#pragma once

#include "Allocator.h"
#include "Core.h"
#include "Memory.h"

#if (_DEBUG)
#include "Assert.h"
#endif

namespace GTSL
{	
	template<class ALLOCATOR>
	class Buffer
	{
	public:
		Buffer(const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {
		}
		
		Buffer(uint32 size, uint32 alignment, const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {
			Allocate(size, alignment);
		}
		
		~Buffer() {
			if (data) {
				allocator.Deallocate(capacity, alignment, data); data = nullptr;
			}
		}

		void* PushBytes(uint64 size) {
			void* ret = data + length;
			length += size;
			return ret;
		}

		template<typename T>
		T* AllocateStructure() {
			tryDeltaResize(sizeof(T));
			void* ret = data + length;
			length += sizeof(T);
			::new(ret) T();
			return static_cast<T*>(ret);
		}

		Buffer(Buffer&& buffer) noexcept : allocator(MoveRef(buffer.allocator)), data(buffer.data), capacity(buffer.capacity), length(buffer.length), readPos(buffer.readPos)
		{
			buffer.data = nullptr;
		}
		
		void Allocate(const uint64 bytes, const uint32 algn)
		{
			uint64 allocated_size{ 0 };
			allocator.Allocate(bytes, alignment, reinterpret_cast<void**>(&data), &allocated_size);
			capacity = allocated_size;
			alignment = algn;
			length = 0;
			readPos = 0;
		}

		void Clear() {
			length = 0;
			readPos = 0;
		}
		
		void Resize(const uint64 size) {
			tryResize(size);
		}
		
		void AddResize(const int64 delta) {
			tryDeltaResize(delta);
		}

		void AddBytes(const int64 bytes) {
			length += bytes;
		}

		void CopyBytes(const uint64 size, const byte* from) {
			tryDeltaResize(size);
			MemCopy(size, from, data + length); length += size;
		}

		void ReadBytes(const uint64 size, byte* to)
		{
			GTSL_ASSERT(readPos <= length, "Buffer doesn't contain any more bytes!");
			MemCopy(size, data + readPos, to); readPos += size;
		}
		
		[[nodiscard]] uint64 GetCapacity() const { return capacity; }
		[[nodiscard]] uint64 GetLength() const { return length; }
		[[nodiscard]] uint64 GetSize() const { return length; }
		[[nodiscard]] uint64 GetReadPosition() const { return readPos; }
		[[nodiscard]] byte* GetData() const { return data; }

		[[nodiscard]] byte* begin() const { return data; }
		[[nodiscard]] byte* end() const { return data + length; }
		
		explicit operator Range<byte*>() const { return Range<byte*>(length, data); }
		explicit operator Range<const byte*>() const { return Range<const byte*>(length, data); }

		Range<byte*> GetRange() { return Range<byte*>(length, data); }
		Range<const byte*> GetRange() const { return Range<const byte*>(length, data); }
	
	private:
		[[no_unique_address]] ALLOCATOR allocator;
		
		byte* data{ nullptr };
		uint32 alignment = 16;
		uint64 capacity = 0;
		uint64 length = 0;
		uint64 readPos = 0;

		void tryResize(const uint64 newSize) {
			if (newSize > capacity) {
				auto allocSize = (capacity + newSize) * 2ll; uint64 allocatedSize = 0;
				void* newAlloc = nullptr;
				allocator.Allocate(allocSize, alignment, &newAlloc, &allocatedSize);

				if (data) {
					MemCopy(length, data, newAlloc);
					allocator.Deallocate(capacity, alignment, reinterpret_cast<void*>(data));
				}
				
				data = static_cast<byte*>(newAlloc);
				
				capacity = allocatedSize;
			}
		}

		void tryDeltaResize(const int64 deltaSize) {
			if (length + deltaSize > capacity) {
				auto allocSize = (capacity + deltaSize) * 2ll; uint64 allocatedSize = 0;
				void* newAlloc = nullptr;
				allocator.Allocate(allocSize, alignment, &newAlloc, &allocatedSize);

				if (data) {
					MemCopy(length, data, newAlloc);
					allocator.Deallocate(capacity, alignment, reinterpret_cast<void*>(data));
				}
				
				data = static_cast<byte*>(newAlloc);
				
				capacity = allocatedSize;
			}
		}
		
	};

	template<uint32 BYTES>
	using StaticBuffer = Buffer<StaticAllocator<BYTES>>;
}