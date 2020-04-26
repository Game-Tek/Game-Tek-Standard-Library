#include "GTSL/GamepadQuery.h"

#include "GTSL/Time.h"

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN

#endif

using namespace GTSL;

void GamepadQuery::Update(bool& connected) noexcept
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
		OnTriggersChange(Side::LEFT, static_cast<float>(xinput_state.Gamepad.bLeftTrigger) / 255.0f, static_cast<float>(input_state.Gamepad.bLeftTrigger - xinput_state.Gamepad.bLeftTrigger) / 255.0f);
	}

	if (xinput_state.Gamepad.bRightTrigger != input_state.Gamepad.bRightTrigger)
	{
		OnTriggersChange(Side::RIGHT, static_cast<float>(xinput_state.Gamepad.bRightTrigger) / 255.0f, static_cast<float>(input_state.Gamepad.bRightTrigger - xinput_state.Gamepad.bRightTrigger) / 255.0f);
	}

	if (xinput_state.Gamepad.sThumbLX != input_state.Gamepad.sThumbLX || xinput_state.Gamepad.sThumbLY != input_state.Gamepad.sThumbLY)
	{
		OnSticksMove(Side::LEFT, { static_cast<float>(xinput_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY) / 32768.f }, { static_cast<float>(xinput_state.Gamepad.sThumbLX - input_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY - input_state.Gamepad.sThumbLY) / 32768.f } );
	}

	if (xinput_state.Gamepad.sThumbRX != input_state.Gamepad.sThumbRX || xinput_state.Gamepad.sThumbRY != input_state.Gamepad.sThumbRY)
	{
		OnSticksMove(Side::RIGHT, { static_cast<float>(xinput_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY) / 32768.f }, { static_cast<float>(xinput_state.Gamepad.sThumbRX - input_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY - input_state.Gamepad.sThumbRY) / 32768.f } );
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))
	{
		OnDPadChange(GamepadButtonPosition::TOP, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
	{
		OnDPadChange(GamepadButtonPosition::BOTTOM, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
	{
		OnDPadChange(GamepadButtonPosition::LEFT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		OnDPadChange(GamepadButtonPosition::RIGHT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_START))
	{
		OnMenuButtonsChange(Side::RIGHT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		OnMenuButtonsChange(Side::LEFT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
	{
		OnSticksChange(Side::LEFT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		OnSticksChange(Side::RIGHT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		OnHatsChange(Side::LEFT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		OnHatsChange(Side::RIGHT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_A))
	{
		OnRightButtonsChange(GamepadButtonPosition::BOTTOM, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_B))
	{
		OnRightButtonsChange(GamepadButtonPosition::RIGHT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		OnRightButtonsChange(GamepadButtonPosition::LEFT, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X));
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != (input_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y))
	{
		OnRightButtonsChange(GamepadButtonPosition::TOP, intToGamepadButtonState(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y));
	}

	input_state = xinput_state;
}

void GamepadQuery::SetVibration(const GamepadVibration& gamepadVibration) const noexcept
{
	XINPUT_VIBRATION xinput_vibration;
	xinput_vibration.wLeftMotorSpeed = static_cast<WORD>(gamepadVibration.LowFrequency * 65535);
	xinput_vibration.wRightMotorSpeed = static_cast<WORD>(gamepadVibration.HighFrequency * 65535);
	XInputSetState(controllerId, &xinput_vibration);
}
