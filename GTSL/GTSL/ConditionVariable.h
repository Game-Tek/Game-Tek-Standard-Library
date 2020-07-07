#pragma once

#include <synchapi.h>

namespace GTSL
{
	class ConditionVariable
	{
	public:
		ConditionVariable()
		{
			InitializeConditionVariable(&conditionVariable);
		}

		~ConditionVariable() = default;

		void Wait(const class Mutex& mutex)
		{
			SleepConditionVariableCS(&conditionVariable, (PCRITICAL_SECTION)&mutex, 0xFFFFFFFF);
		}
		
		void Wait(const class ReadWriteMutex& mutex, bool isInReadMode)
		{
			SleepConditionVariableSRW(&conditionVariable, (PSRWLOCK)&mutex, 0xFFFFFFFF, isInReadMode);
		}

		template<typename L>
		void Wait(const class Lock<Mutex>& lock, L predicate)
		{
			while (!predicate()) { Wait(lock); }
		}

		void NotifyAll() { WakeAllConditionVariable(&conditionVariable); }

		void NotifyOne() { WakeConditionVariable(&conditionVariable); }
		
	private:
		CONDITION_VARIABLE conditionVariable;
	};
	
	class ConditionalLock
	{
	public:
		ConditionalLock()
		{
			InitializeCriticalSection(&criticalSection);
			InitializeConditionVariable(&conditionVariable);
		}

		~ConditionalLock()
		{
			DeleteCriticalSection(&criticalSection);
		}

		void Wait()
		{
			SleepConditionVariableCS(&conditionVariable, &criticalSection, 0xFFFFFFFF);
		}

		void NotifyAll() { WakeAllConditionVariable(&conditionVariable); }

		void NotifyOne() { WakeConditionVariable(&conditionVariable); }
		
	private:
		CRITICAL_SECTION criticalSection;
		CONDITION_VARIABLE conditionVariable;
	};
}
