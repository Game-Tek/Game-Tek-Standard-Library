#include "GTSL/DLL.h"

#include <Windows.h>

#undef LoadLibrary

using namespace GTSL;

void DLL::LoadLibrary(const Range<const char*> ranger)
{
	handle = LoadLibraryA(ranger.begin());
}

void DLL::UnloadLibrary()
{
	FreeLibrary(static_cast<HMODULE>(handle));
	handle = nullptr;
}

DLL::DynamicFunction DLL::LoadDynamicFunction(const Range<const char*> ranger) const
{
	return DynamicFunction(GetProcAddress(static_cast<HMODULE>(handle), ranger.begin()));
}

