#pragma once


#include "Core.h"
#include "Delegate.hpp"
#include "Tuple.h"
#include <tuple>

namespace GTSL
{
	class Thread
	{
	public:
		Thread() = default;
		
		template<typename T, typename... ARGS>
		Thread(Delegate<T> delegate, ARGS&&... args) noexcept
		{				
			FunctionCallData<typename decltype(delegate)::return_type, ARGS...> function_call_data(delegate, MakeForwardReference<ARGS>(args)...);
			
			auto p = &Thread::launchThread<typename decltype(delegate)::return_type, ARGS..., BuildIndices<sizeof...(ARGS)>> ;
			handle = createThread(p, &function_call_data);
		}

		static uint32 ThisTreadID() noexcept;
		
		enum class Priority : uint8
		{
			LOW, LOW_MID, MID, MID_HIGH, HIGH
		};
		void SetPriority(Priority threadPriority) const noexcept;

		void Join() const noexcept;
		void Detach() noexcept;

		[[nodiscard]] uint32 GetId() const noexcept;

		[[nodiscard]] bool CanBeJoined() const noexcept;

	private:
		template<typename FT, typename... ARGS>
		struct FunctionCallData
		{
			FunctionCallData(const Delegate<FT(ARGS...)>& delegate, ARGS&&... args) : Delegate(delegate), Parameters(std::make_tuple(MakeForwardReference<ARGS>(args)...))
			{
			}
			
			Delegate<FT(ARGS...)> Delegate;
			std::tuple<ARGS...> Parameters;
		};

		void* handle{ nullptr };

		template <typename RET, typename... ARGS, uint64 ... IS>
		static void func(const Delegate<RET(ARGS...)>& delegate, std::tuple<ARGS...>& tup, Indices<IS...>)
		{
			delegate(std::get<IS>(tup)...);
		}

		template <typename RET, typename... ARGS>
		static void func(const Delegate<RET(ARGS...)>& delegate, std::tuple<ARGS...>& tup)
		{
			func(delegate, tup, BuildIndices<sizeof...(ARGS)>{});
		}
		
		template<typename RET, typename... ARGS>
		static unsigned long launchThread(void* data)
		{
			FunctionCallData<RET, ARGS...>* function_data = static_cast<FunctionCallData<RET, ARGS...>&>(data);
			
			func(function_data->Delegate, function_data->Parameters);

			return 0;
		}

		
		static void* createThread(unsigned long(*function)(void*), void* data) noexcept;
	};
}
