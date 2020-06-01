#pragma once

#if (_DEBUG)
extern void onAssert(bool condition, const char* text, int line, const char* file, const char* function);
#endif

#ifdef _DEBUG
#define GTSL_ASSERT(condition, text) onAssert(condition, text, __LINE__, __FILE__, __FUNCTION__);
#else
#define GTSL_ASSERT(condition, text)
#endif