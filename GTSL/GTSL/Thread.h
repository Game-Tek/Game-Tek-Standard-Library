#pragma once

#include "Core.h"
#include "Delegate.hpp"
#include "Tuple.h"

namespace GTSL
{
	class Thread
	{
	public:
		Thread() = default;

		template<typename T, typename... ARGS>
		Thread(Delegate<T> delegate, ARGS... args) noexcept
		{
			auto data = new FunctionCallData<T, ARGS...>(delegate, GTSL::MakeForwardReference<ARGS>(args)...);
			
			auto p = &Thread::launchThread<T, ARGS...>;
			handle = createThread(p, data);
		}

		static uint32 ThisTreadID() noexcept;

		enum class Priority : uint8
		{
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetPriority(Priority threadPriority) const noexcept;

		void Join() noexcept;
		void Detach() noexcept;

		[[nodiscard]] uint32 GetId() const noexcept;

		[[nodiscard]] bool CanBeJoined() const noexcept;

		static uint8 ThreadCount();

		template <typename FT, typename... ARGS, uint64 ... IS>
		static void Call(const Delegate<FT>& delegate, Tuple<ARGS...>& tup, Indices<IS...>)
		{
			delegate(TupleAccessor<IS>::Get(tup)...);
		}

		template <typename FT, typename... ARGS>
		static void Call(const Delegate<FT>& delegate, Tuple<ARGS...>& tup)
		{
			Call(delegate, tup, BuildIndices<sizeof...(ARGS)>{});
		}
		
	private:
		template<typename FT, typename... ARGS>
		struct FunctionCallData
		{
			FunctionCallData(Delegate<FT> delegate, ARGS&&... args) : Delegate(delegate), Parameters(GTSL::MakeForwardReference<ARGS>(args)...)
			{
			}

			Delegate<FT> Delegate;
			Tuple<ARGS...> Parameters;
		};

		void* handle{ nullptr };

		template<typename FT, typename... ARGS>
		static unsigned long launchThread(void* data)
		{
			FunctionCallData<FT, ARGS...>* function_data = static_cast<FunctionCallData<FT, ARGS...>*>(data);

			Call(function_data->Delegate, function_data->Parameters);

			delete function_data;

			return 0;
		}


		static void* createThread(unsigned long(*function)(void*), void* data) noexcept;
	};
}
