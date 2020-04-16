#pragma once

#if (_DEBUG)
extern void onAssert(const char* text, int line, const char* file, const char* function);

inline void assert(const bool condition, const char* text, const int line, const char* file, const char* function)
{
	if (condition)
	{
		onAssert(text, line, file, function);
#if (_WIN32)
		__debugbreak();
#endif
	}
}
#endif

#ifdef _DEBUG
#define GTSL_ASSERT(condition, text) assert(condition, text, __LINE__, __FILE__, __FUNCTION__);
#else
#define GTSL_ASSERT(condition, text)
#endif