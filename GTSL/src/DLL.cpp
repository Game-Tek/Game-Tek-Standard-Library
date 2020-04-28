#include "GTSL/DLL.h"

#include <Windows.h>

#undef LoadLibrary

using namespace GTSL;

void DLL::LoadLibrary(const Ranger<char>& ranger)
{
	handle = LoadLibraryA(ranger.begin());
}

void DLL::UnloadLibrary()
{
	FreeLibrary(static_cast<HMODULE>(handle));
	handle = nullptr;
}

DLL::DynamicFunction DLL::LoadDynamicFunction(const Ranger<char>& ranger) const
{
	return DynamicFunction(GetProcAddress(static_cast<HMODULE>(handle), ranger.begin()));
}

