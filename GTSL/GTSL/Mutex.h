#pragma once

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOCOMM
#endif

namespace GTSL
{
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
    class Lock<Mutex>
    {
    public:
        explicit Lock(Mutex& mutex) noexcept : object(&mutex) { mutex.Lock(); }
        ~Lock() noexcept { object->Unlock(); }

        operator const Mutex& () const { return *object; }
    private:
        Mutex* object = nullptr;
    };

    template <class T>
    Lock(T) -> Lock<T>;

    class ReadWriteMutex
    {
    public:
        ReadWriteMutex() noexcept { InitializeSRWLock(&sharedLock); }
        ~ReadWriteMutex() noexcept = default; //SRW locks do not need to be explicitly destroyed.
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

    template <class T>
    WriteLock(T) -> WriteLock<T>;

	template <class T>
    ReadLock(T) -> ReadLock<T>;
}
