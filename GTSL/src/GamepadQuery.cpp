#include "GTSL/GamepadQuery.h"

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN

#endif

using namespace GTSL;

void GamepadQuery::Update(GamepadQuery& gamepadQuery, bool& connected, const uint8 controllerId) noexcept
{
	XINPUT_STATE xinput_state;

	if (XInputGetState(controllerId, &xinput_state) != ERROR_SUCCESS) { connected = false; return; }

	connected = true;

	if (xinput_state.Gamepad.bLeftTrigger != gamepadQuery.input_state.Gamepad.bLeftTrigger)
	{
		gamepadQuery.OnTriggersChange(Side::LEFT, static_cast<float>(xinput_state.Gamepad.bLeftTrigger) / 255.0f);
	}

	if (xinput_state.Gamepad.bRightTrigger != gamepadQuery.input_state.Gamepad.bRightTrigger)
	{
		gamepadQuery.OnTriggersChange(Side::RIGHT, static_cast<float>(xinput_state.Gamepad.bRightTrigger) / 255.0f);
	}

	if (xinput_state.Gamepad.sThumbLX != gamepadQuery.input_state.Gamepad.sThumbLX || xinput_state.Gamepad.sThumbLY != gamepadQuery.input_state.Gamepad.sThumbLY)
	{
		gamepadQuery.OnSticksMove(Side::LEFT, { static_cast<float>(xinput_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY) / 32768.f });
	}

	if (xinput_state.Gamepad.sThumbRX != gamepadQuery.input_state.Gamepad.sThumbRX || xinput_state.Gamepad.sThumbRY != gamepadQuery.input_state.Gamepad.sThumbRY)
	{
		gamepadQuery.OnSticksMove(Side::RIGHT, { static_cast<float>(xinput_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY) / 32768.f });
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP))
	{
		gamepadQuery.OnDPadChange(GamepadButtonPosition::TOP, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
	{
		gamepadQuery.OnDPadChange(GamepadButtonPosition::BOTTOM, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
	{
		gamepadQuery.OnDPadChange(GamepadButtonPosition::LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		gamepadQuery.OnDPadChange(GamepadButtonPosition::RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_START))
	{
		gamepadQuery.OnMenuButtonsChange(Side::RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		gamepadQuery.OnMenuButtonsChange(Side::LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
	{
		gamepadQuery.OnSticksChange(Side::LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		gamepadQuery.OnSticksChange(Side::RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		gamepadQuery.OnHatsChange(Side::LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		gamepadQuery.OnHatsChange(Side::RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_A))
	{
		gamepadQuery.OnFrontButtonsChange(GamepadButtonPosition::BOTTOM, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_B))
	{
		gamepadQuery.OnFrontButtonsChange(GamepadButtonPosition::RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_X))
	{
		gamepadQuery.OnFrontButtonsChange(GamepadButtonPosition::LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
	}

	if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != (gamepadQuery.input_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y))
	{
		gamepadQuery.OnFrontButtonsChange(GamepadButtonPosition::TOP, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
	}

	gamepadQuery.input_state = xinput_state;
}

void GamepadQuery::SetVibration(const GamepadVibration& gamepadVibration) const noexcept
{
	XINPUT_VIBRATION xinput_vibration;
	xinput_vibration.wLeftMotorSpeed = static_cast<WORD>(gamepadVibration.LowFrequency * 65535);
	xinput_vibration.wRightMotorSpeed = static_cast<WORD>(gamepadVibration.HighFrequency * 65535);
	XInputSetState(controllerId, &xinput_vibration);
}
