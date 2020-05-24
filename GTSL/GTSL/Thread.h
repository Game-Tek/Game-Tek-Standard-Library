#pragma once

#include "Core.h"
#include "Delegate.hpp"

namespace GTSL
{
	class Thread
	{
		void* handle{ nullptr };

		template<typename T, typename... ARGS>
		static unsigned long launchThread(void* data)
		{
			//parameter count
			//parameters size
			//delegate
			//parameters data

			*reinterpret_cast<Delegate<T>*>(static_cast<byte*>(data) + 8)(reinterpret_cast<ARGS>(static_cast<byte*>(data) + sizeof(uint64) * 2)...);
		}
		
		static void* createThread(void* function, void* data) noexcept;
	public:
		//template<typename F, typename... P>
		//explicit Thread(F&& lambda, P&&... params) : thread(std::forward<F>(lambda)..., std::forward<P>(params)...)
		//{
		//}

		template<typename T, typename... ARGS>
		Thread(Delegate<T> delegate, ARGS&&... args)
		{
			auto constexpr parameters_size = (sizeof(ARGS) + ...);
			byte data[sizeof(uint64) * 2/*parameter count + parameters_size*/ + sizeof(delegate) + parameters_size];
			reinterpret_cast<uint64&>(data) = sizeof...(ARGS);
			reinterpret_cast<uint64&>(data + sizeof(uint64)) = parameters_size;
			reinterpret_cast<Delegate<T>&>(data + sizeof(uint64) + sizeof(uint64)) = delegate;
			reinterpret_cast<byte*>(data + sizeof(uint64) + sizeof(uint64) + sizeof(Delegate<T>)) = GTSL::MakeTransferReference(args...);
			auto p = &Thread::launchThread<T, ARGS...>;
			handle = createThread(p, data);
		}

		static uint32 ThisTreadID() noexcept;
		
		enum class Priority : uint8
		{
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetPriority(Priority threadPriority) const noexcept;

		void Join() const noexcept;
		void Detach();

		[[nodiscard]] uint32 GetId() const noexcept;

		[[nodiscard]] bool CanBeJoined() const noexcept;
	};
}
