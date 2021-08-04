#pragma once

#include "Core.h"

#include "Delegate.hpp"
#include "Math/Vectors.h"

#if (_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

namespace GTSL
{
	/**
	 * \brief Represents a gamepad that might or might not be connected at the time, and allows querying for connection and setting it's vibration state.
	 */
	class Gamepad
	{
	protected:
#if (_WIN64)
		uint8 controllerId{ 0 };

		uint32 packetNumber = 0;
		
		byte leftTrigger = 0, rightTrigger = 0;
		int16 thumbLX = 0, thumbLY = 0, thumbRX = 0, thumbRY = 0;
		bool dpadUp : 1 = false, dpadRight : 1 = false, dpadDown : 1 = false, dpadLeft : 1 = false;
		bool start  : 1 = false, back : 1 = false;
		bool leftThumb : 1 = false, rightThumb : 1 = false;
		bool leftShoulder : 1 = false, rightShoulder : 1 = false;
		bool a : 1 = false, b : 1 = false, x : 1 = false, y : 1 = false;
#endif
		template<typename BF, typename FF, typename VF>
		friend bool Update(Gamepad& gamepadQuery, BF&& buttonFunction, FF&& floatFunction, VF&& vectorFunction, uint8 controllerId) noexcept
		{
			XINPUT_STATE xinput_state;

			if (XInputGetState(controllerId, &xinput_state) != ERROR_SUCCESS) { return false; }
			if (gamepadQuery.packetNumber == xinput_state.dwPacketNumber) { return true; }

			gamepadQuery.packetNumber = xinput_state.dwPacketNumber;

			if (xinput_state.Gamepad.bLeftTrigger != gamepadQuery.leftTrigger) {
				floatFunction(Side::LEFT, static_cast<float>(xinput_state.Gamepad.bLeftTrigger) / 255.0f);
				gamepadQuery.leftTrigger = xinput_state.Gamepad.bLeftTrigger;
			}

			if (xinput_state.Gamepad.bRightTrigger != gamepadQuery.rightTrigger) {
				floatFunction(Side::RIGHT, static_cast<float>(xinput_state.Gamepad.bRightTrigger) / 255.0f);
				gamepadQuery.rightTrigger = xinput_state.Gamepad.bRightTrigger;
			}

			if (xinput_state.Gamepad.sThumbLX != gamepadQuery.thumbLX || xinput_state.Gamepad.sThumbLY != gamepadQuery.thumbLY) {
				vectorFunction(Side::LEFT, { static_cast<float>(xinput_state.Gamepad.sThumbLX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbLY) / 32768.f });
				gamepadQuery.thumbLX = xinput_state.Gamepad.sThumbLX; gamepadQuery.thumbLY = xinput_state.Gamepad.sThumbLY;
			}

			if (xinput_state.Gamepad.sThumbRX != gamepadQuery.thumbRX || xinput_state.Gamepad.sThumbRY != gamepadQuery.thumbRY) {
				vectorFunction(Side::RIGHT, { static_cast<float>(xinput_state.Gamepad.sThumbRX) / 32768.f, static_cast<float>(xinput_state.Gamepad.sThumbRY) / 32768.f });
				gamepadQuery.thumbRX = xinput_state.Gamepad.sThumbRX; gamepadQuery.thumbRY = xinput_state.Gamepad.sThumbRY;
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != gamepadQuery.dpadUp) {
				gamepadQuery.dpadUp = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
				buttonFunction(GamepadButtonPosition::DPAD_UP, gamepadQuery.dpadUp);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != gamepadQuery.dpadDown) {
				gamepadQuery.dpadDown = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
				buttonFunction(GamepadButtonPosition::DPAD_DOWN, gamepadQuery.dpadDown);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != gamepadQuery.dpadLeft) {
				gamepadQuery.dpadLeft = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
				buttonFunction(GamepadButtonPosition::DPAD_LEFT, gamepadQuery.dpadLeft);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != gamepadQuery.dpadRight) {
				gamepadQuery.dpadRight = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
				buttonFunction(GamepadButtonPosition::DPAD_RIGHT, gamepadQuery.dpadRight);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != gamepadQuery.start) {
				gamepadQuery.start = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
				buttonFunction(GamepadButtonPosition::HOME, gamepadQuery.start);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != gamepadQuery.back) {
				gamepadQuery.back = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
				buttonFunction(GamepadButtonPosition::BACK, gamepadQuery.back);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != gamepadQuery.leftThumb) {
				gamepadQuery.leftThumb = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
				buttonFunction(GamepadButtonPosition::LEFT_STICK, gamepadQuery.leftThumb);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != gamepadQuery.rightThumb) {
				gamepadQuery.rightThumb = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
				buttonFunction(GamepadButtonPosition::RIGHT_STICK, gamepadQuery.rightThumb);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != gamepadQuery.leftShoulder) {
				gamepadQuery.leftShoulder = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
				buttonFunction(GamepadButtonPosition::LEFT_SHOULDER, gamepadQuery.leftShoulder);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != gamepadQuery.rightShoulder) {
				gamepadQuery.rightShoulder = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
				buttonFunction(GamepadButtonPosition::RIGHT_SHOULDER, gamepadQuery.rightShoulder);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != gamepadQuery.a) {
				gamepadQuery.a = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
				buttonFunction(GamepadButtonPosition::BOTTOM, gamepadQuery.a);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != gamepadQuery.b) {
				gamepadQuery.b = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
				buttonFunction(GamepadButtonPosition::RIGHT, gamepadQuery.b);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != gamepadQuery.x) {
				gamepadQuery.x = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
				buttonFunction(GamepadButtonPosition::LEFT, gamepadQuery.x);
			}

			if (static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != gamepadQuery.y) {
				gamepadQuery.y = static_cast<bool>(xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
				buttonFunction(GamepadButtonPosition::TOP, gamepadQuery.y);
			}

			return true;
		}
	public:
		enum class GamepadButtonPosition : uint8
		{
			TOP, RIGHT, BOTTOM, LEFT, BACK, HOME, DPAD_UP, DPAD_RIGHT, DPAD_DOWN, DPAD_LEFT, LEFT_SHOULDER, RIGHT_SHOULDER,
			LEFT_STICK, RIGHT_STICK
		};

		enum class Side : uint8
		{
			RIGHT, LEFT
		};

		/**
		 * \brief Sets the level of vibration on the gamepad controllers.
		 * \param lowFrequency Amount of low frequency vibration, 0 is none, 1 is max.
		 * \param highFrequency Amount of high frequency vibration, 0 is none, 1 is max.
		 */
		void SetVibration(float32 lowFrequency, float32 highFrequency) const noexcept
		{
			XINPUT_VIBRATION xinput_vibration;
			xinput_vibration.wLeftMotorSpeed = static_cast<WORD>(lowFrequency * 65535);
			xinput_vibration.wRightMotorSpeed = static_cast<WORD>(highFrequency * 65535);
			XInputSetState(controllerId, &xinput_vibration);
		}
	};
}