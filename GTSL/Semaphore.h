#pragma once

#include "Core.h"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Assert.h"
#include "Atomic.hpp"

namespace GTSL {
    class Semaphore {
    public:
        Semaphore() = default;
    
        explicit Semaphore(const int32 count) noexcept : count(count) {
            GTSL_ASSERT(count > -1, "Count must be more than -1.")
        }
    
        void Add() noexcept {
            Lock lock(mutex);
            ++count;
        }
    
        Semaphore& operator++() { Add(); return *this; }
    	
        void Post() noexcept {
            {
                Lock lock(mutex);
                --count;
            }
        	
            cv.NotifyOne();
        }
    	
        Semaphore& operator--() { Post(); return *this; }
    
        void Wait() noexcept {
            Lock lock(mutex);
            cv.Wait(lock, [&]() { return count == 0; });
        }
    
    private:
        int32 count = 0;
        Mutex mutex;
        ConditionVariable cv;
    };

    class Notification {
    public:
        Notification() = default;

        void Add() noexcept { ++count; }

        Notification& operator++() { Add(); return *this; }

        void Post() noexcept {
        	--count;
            cv.NotifyAll();
        }

        Notification& operator--() { Post(); return *this; }

        void Wait() noexcept {
            if (!count) { return; }
            GTSL::Mutex lock; lock.Lock();
            cv.Wait(lock);
        }

    private:
        Atomic<uint32> count;
        ConditionVariable cv;
    };
}