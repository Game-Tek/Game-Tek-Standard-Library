#pragma once

#if(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOCOMM
#elif __linux__
#include <pthread.h>
#endif

namespace GTSL
{
    class Mutex {
    public:
        Mutex() noexcept {
#if _WIN64
            InitializeCriticalSection(&criticalSection);
#elif __linux__
#endif
        }
        ~Mutex() noexcept {
#if _WIN64
            DeleteCriticalSection(&criticalSection);
#elif __linux__
                pthread_mutex_destroy(&mutex);
#endif
        }
        Mutex(const Mutex& other) noexcept = delete;
        Mutex(Mutex&& other) noexcept = delete;
        Mutex& operator=(const Mutex& other) = delete;
        Mutex& operator=(Mutex&& other) = delete;

        void Lock() noexcept {
#if _WIN64
            EnterCriticalSection(&criticalSection);
#elif __linux__
            pthread_mutex_lock(&mutex);
#endif
        }
        bool TryLock() noexcept {
#if _WIN64
            return TryEnterCriticalSection(&criticalSection);
#elif __linux__
            return pthread_mutex_trylock(&mutex) == 0;
#endif
        }
        void Unlock() noexcept {
#if _WIN64
            LeaveCriticalSection(&criticalSection);
#elif __linux__
            pthread_mutex_unlock(&mutex);
#endif
        }
    private:
        friend class ConditionVariable;
#if _WIN64
        CRITICAL_SECTION criticalSection;
#elif __linux__
        pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#endif
    };

    template<class T>
    class Lock;

    template<>
    class Lock<Mutex>
    {
    public:
        explicit Lock(Mutex& mutex) noexcept : object(&mutex) { mutex.Lock(); }
        ~Lock() noexcept { object->Unlock(); }

        operator Mutex& () { return *object; }
        operator const Mutex& () const { return *object; }
    private:
        Mutex* object = nullptr;
    };

    template <class T>
    Lock(T) -> Lock<T>;

    class ReadWriteMutex {
    public:
        ReadWriteMutex() noexcept {
#if _WIN64
            InitializeSRWLock(&sharedLock);
#elif __linux__
#endif
        }

#if _WIN64
        ~ReadWriteMutex() noexcept = default;
#elif __linux__
        ~ReadWriteMutex() noexcept {
                pthread_rwlock_destroy(&lock);
        }
#endif

        ReadWriteMutex(const ReadWriteMutex& other) noexcept = delete;
        ReadWriteMutex(ReadWriteMutex&& other) noexcept = delete;
        ReadWriteMutex& operator=(const ReadWriteMutex& other) = delete;
        ReadWriteMutex& operator=(ReadWriteMutex&& other) = delete;

        void WriteLock() noexcept {
#if _WIN64
            AcquireSRWLockExclusive(&sharedLock);
#elif __linux__
            pthread_rwlock_wrlock(&lock);
#endif
        }

        void WriteUnlock() noexcept {
#if _WIN64
            ReleaseSRWLockExclusive(&sharedLock);
#elif __linux__
                pthread_rwlock_unlock(&lock);
#endif
        }

        void ReadLock() noexcept {
#if _WIN64
            AcquireSRWLockShared(&sharedLock);
#elif __linux__
                pthread_rwlock_rdlock(&lock);
#endif
        }

        void ReadUnlock() noexcept {
#if _WIN64
            ReleaseSRWLockShared(&sharedLock);
#elif __linux__
                pthread_rwlock_unlock(&lock);
#endif
        }

    private:
        friend class ConditionVariable;
#if _WIN64
        SRWLOCK sharedLock;
#elif __linux__
        pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
#endif
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
