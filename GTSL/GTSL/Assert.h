#pragma once

extern void onAssert(const char* text, int line, const char* file, const char* function, int thread);

inline void assert(const bool condition, const char* text, const int line, const char* file, const char* function, const int thread)
{
	if (condition)
	{
		onAssert(text, line, file, function, thread);
#if (_WIN32)
		__debugbreak();
#endif
	}
}

#include "Thread.h"

#ifdef _DEBUG
#define GTSL_ASSERT(condition, text) assert(condition, text, __LINE__, __FILE__, __FUNCTION__, Thread::ThisTreadID());
#else
#define GTSL_ASSERT(consdition, text)
#endif