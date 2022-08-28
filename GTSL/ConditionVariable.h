#pragma once

#include "Mutex.h"

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif __linux__
#include <pthread.h>
#endif

namespace GTSL
{
	template<class T>
	class Lock;
	
	class ConditionVariable {
	public:
		ConditionVariable() noexcept {
#if(_WIN64)
			InitializeConditionVariable(&conditionVariable);
#elif __linux__
#endif
		}

		~ConditionVariable() = default;

		void Wait(Mutex& mutex) {
#if(_WIN64)
			SleepConditionVariableCS(&conditionVariable, PCRITICAL_SECTION(&mutex), 0xFFFFFFFF);
#elif __linux__
			pthread_cond_wait(&conditionVariable, &mutex.mutex);
#endif
		}
		
//		void Wait(ReadWriteMutex& mutex, const bool isInReadMode) {
//#if(_WIN64)
//			SleepConditionVariableSRW(&conditionVariable, PSRWLOCK(&mutex), 0xFFFFFFFF, isInReadMode);
//#elif __linux__
//			pthread_cond_wait(&conditionVariable, &mutex.mutex);
//#endif
//		}

		template<typename L>
		void Wait(Lock<Mutex>& lock, L&& predicate) {
			while (!predicate()) { Wait(lock); }
		}

		void NotifyAll() {
#if(_WIN64)
			WakeAllConditionVariable(&conditionVariable);
#elif __linux__
			pthread_cond_broadcast(&conditionVariable);
#endif
		}

		void NotifyOne() {
#if(_WIN64)
			WakeConditionVariable(&conditionVariable);
#elif __linux__
			pthread_cond_signal(&conditionVariable);
#endif
		}
		
	private:
#if _WIN64
		CONDITION_VARIABLE conditionVariable;
#elif __linux__
		pthread_cond_t conditionVariable = PTHREAD_COND_INITIALIZER;
#endif
	};
	
	class ConditionalLock {
	public:
		ConditionalLock() {
#if(_WIN64)
			InitializeCriticalSection(&criticalSection);
			InitializeConditionVariable(&conditionVariable);
#elif __linux__
#endif
		}

		~ConditionalLock() {
#if(_WIN64)
			DeleteCriticalSection(&criticalSection);
#elif __linux__
#endif
		}

		void Wait() {
#if(_WIN64)
			SleepConditionVariableCS(&conditionVariable, &criticalSection, 0xFFFFFFFF);
#elif __linux__
#endif
		}

		void NotifyAll() {
#if(_WIN64)
			WakeAllConditionVariable(&conditionVariable);
#elif __linux__
#endif
		}

		void NotifyOne() {
#if(_WIN64)
			WakeConditionVariable(&conditionVariable);
#elif __linux__
#endif
		}
		
	private:
#if _WIN64
		CRITICAL_SECTION criticalSection;
		CONDITION_VARIABLE conditionVariable;
#elif __linux__
#endif
	};
}
