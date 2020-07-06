#pragma once

#include <synchapi.h>

namespace GTSL
{
	class ConditionVariable
	{
	public:
		ConditionVariable()
		{
			InitializeCriticalSection(&criticalSection);
			InitializeConditionVariable(&conditionVariable);
		}

		~ConditionVariable()
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
