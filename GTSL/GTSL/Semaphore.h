#pragma once

#include "Core.h"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Assert.h"

namespace GTSL
{
    class Semaphore
    {
    public:
        Semaphore() = default;

        explicit Semaphore(const int32 count) noexcept : count(count)
        {
            GTSL_ASSERT(count > -1, "Count must be more than -1.")
        }

        void Add() noexcept
        {
            {
                Lock lock(mutex);
                ++count;
            }
        }

        void Post() noexcept
        {
            {
                Lock lock(mutex);
                --count;
            }
            cv.NotifyOne();
        }

        void Wait() noexcept
        {
	        const Lock lock(mutex);
            cv.Wait(lock, [&]() { return count == 0; });
        }

    private:
        int32 count = 0;
        Mutex mutex;
        ConditionVariable cv;
    };
}