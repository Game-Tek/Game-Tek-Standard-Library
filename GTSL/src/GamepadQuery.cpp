#include "GamepadQuery.h"

#if (_WIN32)
#include <winerror.h>
#endif

void GamepadQuery::Update(bool& connected)
{
	XINPUT_STATE xinput_state;

	if (XInputGetState(controllerId, &xinput_state) != ERROR_SUCCESS)
	{
		connected = false;
		return;
	}

	connected = true;

	if (xinput_state.Gamepad.bLeftTrigger != input_state.Gamepad.bLeftTrigger)
	{
		OnLeftTriggerChanged(static_cast<float>(xinput_state.Gamepad.bLeftTrigger) / 255.0f, static_cast<float>(input_state.Gamepad.bLeftTrigger - xinput_state.Gamepad.bLeftTrigger) / 255.0f);
	}

	if (xinput_state.Gamepad.bRightTrigger != input_state.Gamepad.bRightTrigger)
	{
		OnRightTriggerChanged(static_cast<float>(xinput_state.Gamepad.bRightTrigger) / 255.0f, static_cast<float>(input_state.Gamepad.bRightTrigger - xinput_state.Gamepad.bRightTrigger) / 255.0f);
	}

	if (xinput_state.Gamepad.sThumbLX != input_state.Gamepad.sThumbLX || xinput_state.Gamepad.sThumbLY != input_state.Gamepad.sThumbLY)
	{
		OnLeftStickMove(static_cast<float>(xinput_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLX - input_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY - input_state.Gamepad.sThumbLY) / 32768.f);
	}

	if (xinput_state.Gamepad.sThumbRX != input_state.Gamepad.sThumbRX || xinput_state.Gamepad.sThumbRY != input_state.Gamepad.sThumbRY)
	{
		OnRightStickMove(static_cast<float>(xinput_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRX - input_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY - input_state.Gamepad.sThumbRY) / 32768.f);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))
	{
		OnTopDPadButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
	{
		OnBottomDPadButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
	{
		OnLeftDPadButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		OnRightDPadButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_START))
	{
		OnStartButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		OnBackButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
	{
		OnLeftStickButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		OnRightStickButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		OnLeftHatChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		OnRightHatChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_A))
	{
		OnBottomFaceButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_B))
	{
		OnRightFaceButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		OnLeftFaceButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y))
	{
		OnTopFaceButtonChanged(intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y));
	}

	input_state = xinput_state;
}

void GamepadQuery::SetVibration(const GamepadVibration& gamepadVibration)
{
	XINPUT_VIBRATION xinput_vibration;
	xinput_vibration.wLeftMotorSpeed = static_cast<WORD>(gamepadVibration.LowFrequency * 65535);
	xinput_vibration.wRightMotorSpeed = static_cast<WORD>(gamepadVibration.HighFrequency * 65535);
	XInputSetState(controllerId, &xinput_vibration);
}
