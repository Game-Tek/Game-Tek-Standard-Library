#include "GTSL/DLL.h"

#include <Windows.h>

#undef LoadLibrary

using namespace GTSL;

DLL::~DLL()
{
	FreeLibrary(static_cast<HMODULE>(handle));
}

void DLL::LoadLibrary(const Ranger<char>& ranger)
{
	handle = LoadLibraryA(ranger.begin());
}

DLL::DynamicFunction DLL::LoadDynamicFunction(const Ranger<char>& ranger) const
{
	return DynamicFunction(GetProcAddress(static_cast<HMODULE>(handle), ranger.begin()));
}

