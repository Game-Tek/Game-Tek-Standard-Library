#pragma once

#include "Core.h"
#include "Delegate.hpp"
#include "Tuple.h"

namespace GTSL
{
	class Thread
	{
		template<typename FT, typename TT, uint32... INDECES>
		struct FunctionCallData
		{
			//static constexpr uint64 ParameterCount{ sizeof...(ARGS) };
			//static constexpr uint64 ParametersSize{ (sizeof(ARGS) + ...) };
			Delegate<FT> Delegate;
			//byte FunctionParameters[ParametersSize];
			TT Arguments;
		};
		
		void* handle{ nullptr };

		template<typename FT, typename TT, uint32... INDECES>
		static unsigned long launchThread(void* data)
		{
			//parameter count
			//parameters size
			//delegate
			//parameters data

			auto function_data = static_cast<FunctionCallData<FT, TT, INDECES...>*>(data);
			
			function_data->Delegate(Get<INDECES>(function_data->Arguments)...);

			return 0;
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
			//auto constexpr parameters_size = (sizeof(ARGS) + ...);
			//byte data[sizeof(uint64) * 2/*parameter count + parameters_size*/ + sizeof(delegate) + parameters_size];
			//reinterpret_cast<uint64&>(data) = sizeof...(ARGS);
			//reinterpret_cast<uint64&>(data + sizeof(uint64)) = parameters_size;
			//reinterpret_cast<Delegate<T>&>(data + sizeof(uint64) + sizeof(uint64)) = delegate;
			//reinterpret_cast<byte*>(data + sizeof(uint64) + sizeof(uint64) + sizeof(Delegate<T>)) = GTSL::MakeTransferReference(args...);
			//
			
			Tuple<ARGS...> args;
			
			FunctionCallData<T, decltype(args), sizeof...(ARGS)> function_call_data;

			function_call_data.Arguments = args;
			
			auto p = &Thread::launchThread<T, decltype(args), ARGS...>;
			handle = createThread(p, &function_call_data);
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
