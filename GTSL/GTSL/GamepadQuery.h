#pragma once

#include "Gamepad.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

namespace GTSL
{
	/**
	 * \brief Checks state of gamepad to see what changed. During this call all parameters that changed will produce an event call.
	 * \param connected Returns true if the gamepad is connected, false otherwise.
	 */
	template<typename BF, typename FF, typename VF>
	bool Update(Gamepad& gamepadQuery, BF&& buttonFunction, FF&& floatFunction, VF&& vectorFunction, uint8 controllerId) noexcept
	{
		XINPUT_STATE xinput_state;

		if (XInputGetState(controllerId, &xinput_state) != ERROR_SUCCESS) { return false; }
		if (gamepadQuery.packetNumber == xinput_state.dwPacketNumber) { return true; }

		gamepadQuery.packetNumber = xinput_state.dwPacketNumber;

		if (xinput_state.Gamepad.bLeftTrigger != gamepadQuery.leftTrigger) {
			floatFunction(Gamepad::Side::LEFT, static_cast<float>(xinput_state.Gamepad.bLeftTrigger) / 255.0f);
			gamepadQuery.leftTrigger = xinput_state.Gamepad.bLeftTrigger;
		}

		if (xinput_state.Gamepad.bRightTrigger != gamepadQuery.rightTrigger) {
			floatFunction(Gamepad::Side::RIGHT, static_cast<float>(xinput_state.Gamepad.bRightTrigger) / 255.0f);
			gamepadQuery.rightTrigger = xinput_state.Gamepad.bRightTrigger;
		}

		if (xinput_state.Gamepad.sThumbLX != gamepadQuery.thumbLX || xinput_state.Gamepad.sThumbLY != gamepadQuery.thumbLY) {
			vectorFunction(Gamepad::Side::LEFT, { static_cast<float>(xinput_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY) / 32768.f });
			gamepadQuery.thumbLX = xinput_state.Gamepad.sThumbLX; gamepadQuery.thumbLY = xinput_state.Gamepad.sThumbLY;
		}

		if (xinput_state.Gamepad.sThumbRX != gamepadQuery.thumbRX || xinput_state.Gamepad.sThumbRY != gamepadQuery.thumbRY) {
			vectorFunction(Gamepad::Side::RIGHT, { static_cast<float>(xinput_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY) / 32768.f });
			gamepadQuery.thumbRX = xinput_state.Gamepad.sThumbRX; gamepadQuery.thumbRY = xinput_state.Gamepad.sThumbRY;
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != gamepadQuery.dpadUp) {
			gamepadQuery.dpadUp = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_UP, gamepadQuery.dpadUp);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != gamepadQuery.dpadDown) {
			gamepadQuery.dpadDown = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_DOWN, gamepadQuery.dpadDown);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != gamepadQuery.dpadLeft) {
			gamepadQuery.dpadLeft = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_LEFT, gamepadQuery.dpadLeft);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != gamepadQuery.dpadRight) {
			gamepadQuery.dpadRight = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_RIGHT, gamepadQuery.dpadRight);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != gamepadQuery.start) {
			gamepadQuery.start = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
			buttonFunction(Gamepad::GamepadButtonPosition::HOME, gamepadQuery.start);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != gamepadQuery.back) {
			gamepadQuery.back = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
			buttonFunction(Gamepad::GamepadButtonPosition::BACK, gamepadQuery.back);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != gamepadQuery.leftThumb) {
			gamepadQuery.leftThumb = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
			buttonFunction(Gamepad::GamepadButtonPosition::LEFT_STICK, gamepadQuery.leftThumb);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != gamepadQuery.rightThumb) {
			gamepadQuery.rightThumb = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
			buttonFunction(Gamepad::GamepadButtonPosition::RIGHT_STICK, gamepadQuery.rightThumb);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != gamepadQuery.leftShoulder) {
			gamepadQuery.leftShoulder = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			buttonFunction(Gamepad::GamepadButtonPosition::LEFT_SHOULDER, gamepadQuery.leftShoulder);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != gamepadQuery.rightShoulder) {
			gamepadQuery.rightShoulder = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
			buttonFunction(Gamepad::GamepadButtonPosition::RIGHT_SHOULDER, gamepadQuery.rightShoulder);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != gamepadQuery.a) {
			gamepadQuery.a = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
			buttonFunction(Gamepad::GamepadButtonPosition::BOTTOM, gamepadQuery.a);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != gamepadQuery.b) {
			gamepadQuery.b = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			buttonFunction(Gamepad::GamepadButtonPosition::RIGHT, gamepadQuery.b);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != gamepadQuery.x) {
			gamepadQuery.x = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
			buttonFunction(Gamepad::GamepadButtonPosition::LEFT, gamepadQuery.x);
		}

		if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != gamepadQuery.y) {
			gamepadQuery.y = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
			buttonFunction(Gamepad::GamepadButtonPosition::TOP, gamepadQuery.y);
		}

		return true;
	}
}