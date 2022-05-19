#pragma once

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace GTSL
{
	template<class T>
	class Lock;
	
	class ConditionVariable {
	public:
		ConditionVariable() noexcept { InitializeConditionVariable(&conditionVariable); }

		~ConditionVariable() = default;

		void Wait(const class Mutex& mutex) {
			SleepConditionVariableCS(&conditionVariable, PCRITICAL_SECTION(&mutex), 0xFFFFFFFF);
		}
		
		void Wait(const class ReadWriteMutex& mutex, const bool isInReadMode) {
			SleepConditionVariableSRW(&conditionVariable, PSRWLOCK(&mutex), 0xFFFFFFFF, isInReadMode);
		}

		template<typename L>
		void Wait(const Lock<Mutex>& lock, L&& predicate) {
			while (!predicate()) { Wait(lock); }
		}

		void NotifyAll() { WakeAllConditionVariable(&conditionVariable); }

		void NotifyOne() { WakeConditionVariable(&conditionVariable); }
		
	private:
		CONDITION_VARIABLE conditionVariable;
	};
	
	class ConditionalLock {
	public:
		ConditionalLock() {
			InitializeCriticalSection(&criticalSection);
			InitializeConditionVariable(&conditionVariable);
		}

		~ConditionalLock() {
			DeleteCriticalSection(&criticalSection);
		}

		void Wait() {
			SleepConditionVariableCS(&conditionVariable, &criticalSection, 0xFFFFFFFF);
		}

		void NotifyAll() { WakeAllConditionVariable(&conditionVariable); }

		void NotifyOne() { WakeConditionVariable(&conditionVariable); }
		
	private:
		CRITICAL_SECTION criticalSection;
		CONDITION_VARIABLE conditionVariable;
	};
}
