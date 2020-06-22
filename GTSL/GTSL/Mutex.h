#pragma once

#include "Core.h"
#include "Signal.h"
#include <atomic>
#include <shared_mutex>
#include <synchapi.h>

namespace GTSL
{

    class FastMutex
    {
        //https://vorbrodt.blog/2019/02/12/fast-mutex/
    public:
        FastMutex() : state(0) {}

        void Lock()
        {
            if (state.exchange(1, std::memory_order_acquire))
                while (state.exchange(2, std::memory_order_acquire))
                    waitset.Wait();
        }

        void Unlock()
        {
            if (state.exchange(0, std::memory_order_release) == 2)
                waitset.Flag();
        }

    private:
        std::atomic<uint32> state;
        Signal waitset;
    };

    class Mutex
    {
    public:
        Mutex() noexcept { InitializeCriticalSection(&criticalSection); }
        ~Mutex() noexcept { DeleteCriticalSection(&criticalSection); }
        Mutex(const Mutex& other) noexcept = delete;
        Mutex(Mutex&& other) noexcept = delete;
        Mutex& operator=(const Mutex& other) = delete;
        Mutex& operator=(Mutex&& other) = delete;

        void Lock() noexcept { EnterCriticalSection(&criticalSection); }
        bool TryLock() noexcept { return TryEnterCriticalSection(&criticalSection); }
        void Unlock() noexcept { LeaveCriticalSection(&criticalSection); }

    private:
        CRITICAL_SECTION criticalSection;
    };

    template<class T>
    class Lock;

    template<>
    class Lock<FastMutex>
    {
    public:
        explicit Lock(FastMutex& mutex) noexcept : object(&mutex) { mutex.Lock(); }
        ~Lock() noexcept { object->Unlock(); }

    private:
        FastMutex* object = nullptr;
    };

    template<>
    class Lock<Mutex>
    {
    public:
        explicit Lock(Mutex& mutex) noexcept : object(&mutex) { mutex.Lock(); }
        ~Lock() noexcept { object->Unlock(); }

    private:
        Mutex* object = nullptr;
    };

    class ReadWriteMutex
    {
    public:
        ReadWriteMutex() noexcept { InitializeSRWLock(&sharedLock); }
        ~ReadWriteMutex() noexcept = default;
        ReadWriteMutex(const ReadWriteMutex& other) noexcept = delete;
        ReadWriteMutex(ReadWriteMutex&& other) noexcept = delete;
        ReadWriteMutex& operator=(const ReadWriteMutex& other) = delete;
        ReadWriteMutex& operator=(ReadWriteMutex&& other) = delete;

        void WriteLock() noexcept { AcquireSRWLockExclusive(&sharedLock); }
        void ReadLock() noexcept { AcquireSRWLockShared(&sharedLock); }
        void WriteUnlock() noexcept { ReleaseSRWLockExclusive(&sharedLock); }
        void ReadUnlock() noexcept { ReleaseSRWLockShared(&sharedLock); }

    private:
        SRWLOCK sharedLock;
    };

    template<class T>
    class ReadLock;

    template<class T>
    class WriteLock;

    template<>
    class ReadLock<ReadWriteMutex>
    {
    public:
        ReadLock(ReadWriteMutex& readWriteMutex) noexcept : readWriteMutex(&readWriteMutex) { readWriteMutex.ReadLock(); }
        ~ReadLock() noexcept { readWriteMutex->ReadUnlock(); }
    	
    private:
        ReadWriteMutex* readWriteMutex{ nullptr };
    };

    template<>
    class WriteLock<ReadWriteMutex>
    {
    public:
        WriteLock(ReadWriteMutex& readWriteMutex) noexcept : readWriteMutex(&readWriteMutex) { readWriteMutex.WriteLock(); }
        ~WriteLock() noexcept { readWriteMutex->WriteUnlock(); }

    private:
        ReadWriteMutex* readWriteMutex{ nullptr };
    };
}
