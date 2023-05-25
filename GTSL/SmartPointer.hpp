#pragma once

#include "Core.hpp"
#include "Allocator.hpp"

namespace GTSL {
	template<typename T, class ALLOCATOR>
	struct SmartPointer
	{
		SmartPointer() {}

		template<typename... ARGS>
		SmartPointer(const ALLOCATOR& alloc, ARGS&&... args) : allocator(alloc)
		{
			uint64 allocatedSize = 0;
			allocator.Allocate(sizeof(T), alignof(T), reinterpret_cast<void**>(&data), &allocatedSize);
			size = static_cast<uint32>(allocatedSize);
			alignment = static_cast<uint32>(alignof(T));
			::new(data) T(ForwardRef<ARGS>(args)...);

			auto dest = [](void* data) {
				Destroy(*static_cast<T*>(data));
			};
			
			destructor = dest;
		}
		
		SmartPointer(SmartPointer&& other) noexcept : allocator(MoveRef(other.allocator)), size(other.size), alignment(other.alignment), data(other.data), destructor(other.destructor) {
			other.data = nullptr;
		}

		template<typename U>
		SmartPointer(SmartPointer<U, ALLOCATOR>&& other) noexcept : allocator(MoveRef(other.allocator)), size(other.size), alignment(other.alignment), data(other.data), destructor(other.destructor) {
			other.data = nullptr;
		}

		bool TryFree() { if (data) { Free(); return true; } return false; }
		
		void Free() {
			destructor(data);
			allocator.Deallocate(size, alignment, reinterpret_cast<void*>(data));
			data = nullptr;
		}
		
		~SmartPointer() { TryFree(); }

		template<typename TT>
		SmartPointer& operator=(SmartPointer<TT, ALLOCATOR>&& other) noexcept {
			TryFree();
			size = other.size; alignment = other.alignment; data = reinterpret_cast<T*>(other.data); destructor = other.destructor;
			other.data = nullptr;
			return *this;
		}
		
		SmartPointer& operator=(SmartPointer&& other) noexcept {
			TryFree();
			allocator = MoveRef(other.allocator); size = other.size; alignment = other.alignment; data = other.data; destructor = other.destructor;
			other.data = nullptr;
			return *this;
		}
		
		operator T*() const { return data; }
		T* operator->() const { return data; }
		T& operator*() const { return *data; }

		T* GetData() const { return data; }
	protected:
#pragma warning(disable : 4648)
		[[no_unique_address]] ALLOCATOR allocator;
#pragma warning(default : 4648)
		
		uint32 size = 0;
		uint32 alignment = 0;
		T* data = nullptr;
		void(*destructor)(void*) = nullptr;

		template<typename U, class A>
		friend struct SmartPointer;
	};
}