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
	inline bool Update(Gamepad& gamepadQuery, BF&& buttonFunction, FF&& floatFunction, VF&& vectorFunction, uint8 controllerId) noexcept
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

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != gamepadQuery.dpadUp) {
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_UP, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
			gamepadQuery.dpadUp = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != gamepadQuery.dpadDown) {
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_DOWN, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			gamepadQuery.dpadUp = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != gamepadQuery.dpadLeft) {
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			gamepadQuery.dpadUp = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != gamepadQuery.dpadRight) {
			buttonFunction(Gamepad::GamepadButtonPosition::DPAD_RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
			gamepadQuery.dpadUp = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != gamepadQuery.start) {
			buttonFunction(Gamepad::GamepadButtonPosition::HOME, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
			gamepadQuery.start = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != gamepadQuery.back) {
			buttonFunction(Gamepad::GamepadButtonPosition::BACK, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
			gamepadQuery.back = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != gamepadQuery.leftThumb) {
			buttonFunction(Gamepad::GamepadButtonPosition::LEFT_STICK, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
			gamepadQuery.leftThumb = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != gamepadQuery.rightThumb) {
			buttonFunction(Gamepad::GamepadButtonPosition::RIGHT_STICK, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
			gamepadQuery.rightThumb = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != gamepadQuery.leftShoulder) {
			buttonFunction(Gamepad::GamepadButtonPosition::LEFT_SHOULDER, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			gamepadQuery.leftShoulder = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != gamepadQuery.rightShoulder) {
			buttonFunction(Gamepad::GamepadButtonPosition::RIGHT_SHOULDER, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
			gamepadQuery.rightShoulder = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != gamepadQuery.a) {
			buttonFunction(Gamepad::GamepadButtonPosition::BOTTOM, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
			gamepadQuery.a = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != gamepadQuery.b) {
			buttonFunction(Gamepad::GamepadButtonPosition::RIGHT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			gamepadQuery.b = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != gamepadQuery.x) {
			buttonFunction(Gamepad::GamepadButtonPosition::LEFT, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
			gamepadQuery.x = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
		}

		if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != gamepadQuery.y) {
			buttonFunction(Gamepad::GamepadButtonPosition::TOP, xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
			gamepadQuery.y = (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
		}

		return true;
	}
}