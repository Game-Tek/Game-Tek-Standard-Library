#include "GTSL/Gamepad.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

void GTSL::Gamepad::SetVibration(float32 lowFrequency, float32 highFrequency) const noexcept
{
	XINPUT_VIBRATION xinput_vibration;
	xinput_vibration.wLeftMotorSpeed = static_cast<WORD>(lowFrequency * 65535);
	xinput_vibration.wRightMotorSpeed = static_cast<WORD>(highFrequency * 65535);
	XInputSetState(controllerId, &xinput_vibration);
}
