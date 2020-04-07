#pragma once

#include "Application.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class WindowsApplication final : public Application
{
	HINSTANCE instance = nullptr;

public:
	explicit WindowsApplication(const ApplicationCreateInfo& applicationCreateInfo);

	void Update() override;

	void Close() override;
	
	[[nodiscard]] HINSTANCE GetInstance() const { return instance; }
};
